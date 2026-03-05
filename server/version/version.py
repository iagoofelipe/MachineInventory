import sqlite3
from uuid import uuid4
from typing import Literal, Iterable
import datetime as dt
from dataclasses import dataclass

@dataclass
class Disk:
    id: str
    name: str
    serialNumber: str
    model: str

@dataclass
class NetworkAdapter:
    id: str
    name: str
    mac: str

@dataclass
class PhysicalMemory:
    id: str
    name: str
    capacity: str
    speed: str

@dataclass
class Program:
    id: str
    name: str
    publisher: str
    version: str
    estimatedSize: str
    currentUserOnly: bool

@dataclass
class MachineData:
    id: str
    os: str
    title: str
    osArchitecture: str
    osInstallDate: str
    osVersion: str
    osSerialNumber: str
    organization: str
    motherboard: str
    motherboardManufacturer: str
    processor: str
    processorClockSpeed: str

@dataclass
class MachineVersion:
    id: str
    datetime: str
    mac: str
    previous_id: str
    machine: MachineData
    disks: list[Disk]
    network_adapters: list[NetworkAdapter]
    physical_memories: list[PhysicalMemory]
    programs: list[Program]

class Machine:
    COLUMNS_BY_TABLE = { # todas as tabelas devem possuir machine_version_id e id (suprimidos para funcionar em todas as funções)
        'machine': ['os', 'title', "osArchitecture", "osInstallDate", "osVersion", "osSerialNumber", "organization", "motherboard", "motherboardManufacturer", "processor", "processorClockSpeed"],
        'disk': ['name', 'serialNumber', 'model'],
        'network_adapter': ['name', 'mac'],
        'physical_memory': ['name', 'capacity', 'speed'],
        'program': ['name', 'publisher', 'version', 'estimatedSize', 'currentUserOnly'],
    }
    
    def __init__(self):
        self._conn = sqlite3.connect('database.db')
        self._cursor = self._conn.cursor()

    def close(self):
        self._cursor.close()

    def getVersion(self, arg:str, column:Literal['id', 'datetime', 'mac'], as_dataclass=True) -> None | tuple[str, dict[str, dict[str, tuple]]] | MachineVersion:
        # coletar a machine_version de acordo com o datetime desejado
        if column == 'mac':
            self._cursor.execute('SELECT id, previous_id FROM machine_version WHERE mac=? ORDER BY `datetime` DESC LIMIT 1', (arg, ))
        elif column in ('id', 'datetime'):
            self._cursor.execute(f'SELECT id, previous_id FROM machine_version WHERE {column}=?', (arg, ))
        else:
            raise ValueError(f'{column} is not a valid param')
        
        r = self._cursor.fetchone()
        if not r:
            return
        
        # criar a lista ligada a partir de previous_id até que o mesmo seja vazio (sinalizando início das alterações)
        machine_version_id, previous_id = r
        linked_list_versions = [machine_version_id] # end to init

        while previous_id: # coletando previous_id
            linked_list_versions.append(previous_id)
            self._cursor.execute('SELECT previous_id FROM machine_version WHERE id=?', (previous_id, ))
            previous_id = self._cursor.fetchone()[0]

        data = { table: {} for table in self.COLUMNS_BY_TABLE }
        to_ignore = [] # dados que foram removidos ao decorrer das versões

        # coletar ids da versão
        for v in linked_list_versions:
            # coletando dados removidos entre a versão anterior e a atual
            self._cursor.execute('SELECT ref_id FROM version_remove WHERE machine_version_id=?', (v, ))
            to_ignore.extend(r[0] for r in self._cursor.fetchall())

            for table in self.COLUMNS_BY_TABLE:
                # coletar dados referente a essa table nessa versão
                self._cursor.execute(f'SELECT id,{','.join(self.COLUMNS_BY_TABLE[table])} FROM {table} WHERE machine_version_id=?', (v, ))
                data[table].update({ d[0]: d[1:] for d in self._cursor.fetchall() if d[0] not in to_ignore })

        if not as_dataclass:
            return machine_version_id, data
        
        self._cursor.execute('SELECT datetime, mac, previous_id FROM machine_version WHERE id=?', (machine_version_id, ))
        r = self._cursor.fetchone()

        return MachineVersion(
            id=machine_version_id,
            datetime=r[0],
            mac=r[1],
            previous_id=r[2],
            machine=[ MachineData(k, *v) for k, v in data['machine'].items() ][0],
            disks=[ Disk(k, *v) for k, v in data['disk'].items() ],
            network_adapters=[ NetworkAdapter(k, *v) for k, v in data['network_adapter'].items() ],
            physical_memories=[ PhysicalMemory(k, *v) for k, v in data['physical_memory'].items() ],
            programs=[ Program(k, *v) for k, v in data['program'].items() ],
        )
    
    def uploadNewVersion(self, data:MachineVersion) -> MachineVersion:
        latest_version = self.getVersion(data.mac, 'mac', as_dataclass=False)
        previous_id, previous_data = latest_version if latest_version else (None, None)
        version_id = str(uuid4())
        machine_id = str(uuid4())

        # table: { id: data }      data deve ser uma lista seguindo a mesma ordem de COLUMNS_BY_TABLE
        props = {
            'machine': { machine_id: tuple(getattr(data.machine, f) for f in self.COLUMNS_BY_TABLE['machine']) },
            'disk': { str(uuid4()): (o.name, o.serialNumber, o.model) for o in data.disks },
            'network_adapter': { str(uuid4()): (o.name, o.mac) for o in data.network_adapters },
            'physical_memory': { str(uuid4()): (o.name, o.capacity, o.speed) for o in data.physical_memories },
            'program': { str(uuid4()): (o.name, o.publisher, o.version, o.estimatedSize, o.currentUserOnly) for o in data.programs },
        }

        datetime = dt.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        self._cursor.execute('INSERT INTO machine_version (id, datetime, mac, previous_id) VALUES (?,?,?,?)', (version_id, datetime, data.mac, previous_id))
        
        if latest_version:
            changes = []

            for table, prop in props.items():
                # comparando dados
                ids_to_remove, ids_to_add = self._from_to(previous_data[table], prop)
                
                # inserindo valores adicionados
                extra_cols = ','.join(self.COLUMNS_BY_TABLE[table])
                extra_places = ','.join('?' for _ in self.COLUMNS_BY_TABLE[table])
                if ids_to_add:
                    self._cursor.executemany(f'INSERT INTO {table} (id, {extra_cols}, machine_version_id) VALUES (?,{extra_places},?)', [ [_id, *prop[_id], version_id] for _id in ids_to_add ])

                # removendo valores desatualizados COLUMNS: id, table_name, ref_id, machine_version_id
                changes.extend([ [str(uuid4()), table, id_to_remove, version_id] for id_to_remove in ids_to_remove ])

            self._cursor.executemany('INSERT INTO version_remove (id, table_name, ref_id, machine_version_id) VALUES (?,?,?,?)', changes)

        else:
            for table, prop in props.items():
                extra_cols = ','.join(self.COLUMNS_BY_TABLE[table])
                extra_places = ','.join('?' for _ in self.COLUMNS_BY_TABLE[table])
                self._cursor.executemany(f'INSERT INTO {table} (id, {extra_cols}, machine_version_id) VALUES (?,{extra_places},?)', [ [k, *v, version_id] for k, v in prop.items() ])

        self._conn.commit()

        return self.getVersion(version_id, 'id')

    @staticmethod
    def _from_to(d1:dict[str, Iterable], d2:dict[str, Iterable]):
        """ retorna a lista de d1 a serem removidos e a lista de d2 a serem adicionados  """
        ids_by_obj = {}
        set_d1 = set()
        set_d2 = set()

        for _set, d in ((set_d1, d1), (set_d2, d2)):
            for k, v in d.items():
                t = tuple(v) if type(v) is not tuple else v
                _set.add(t)
                ids_by_obj[t] = k
        
        to_remove = set_d1 - set_d2
        to_add = set_d2 - set_d1

        # coletando IDs a partir dos objetos
        ids_to_remove = list(map(ids_by_obj.pop, to_remove))
        ids_to_add = list(map(ids_by_obj.pop, to_add))

        return ids_to_remove, ids_to_add

m = Machine()
# print(m.getVersion('D8:5E:D3:E8:B6:ED', 'mac'))

data = MachineVersion(
    id=None,
    datetime=None,
    mac='D8:5E:D3:E8:B6:ED',
    previous_id=None,
    machine=MachineData(
        id=None,
        os='Microsoft Windows 11 Pro',
        title='GUI',
        osArchitecture="64 bits",
        osInstallDate="2026-01-19 15:15:23",
        osVersion="10.0.26200",
        osSerialNumber="00330-80000-00000-AA779",
        organization=None,
        motherboard="B450M DS3H V2",
        motherboardManufacturer="Gigabyte Technology Co., Ltd.",
        processor="AMD Ryzen 5 4500 6-Core Processor",
        processorClockSpeed="3.60 GHz"
    ),
    disks=[
        Disk(id=None, name="KINGSTON SNV2S1000G", serialNumber=None, model="KINGSTON SNV2S1000G"),
    ],
    network_adapters=[
        NetworkAdapter(id=None, name="Realtek Gaming GbE Family Controller", mac="D8:5E:D3:E8:B6:ED"),
        NetworkAdapter(id=None, name="WAN Miniport (IP)", mac="04:76:20:52:41:53"),
    ],

    physical_memories=[
        PhysicalMemory(id=None, name="Physical Memory 0", capacity="8 GB", speed="2133 MT/s"),
        PhysicalMemory(id=None, name="Physical Memory 2", capacity="8 GB", speed="2133 MT/s"),
    ],
    programs=[
        Program(
            id=None,
            name="Git",
            publisher="The Git Development Community",
            version="2.52.0",
            estimatedSize="346 MB",
            currentUserOnly=False,
        ),
        Program(
            id=None,
            name="IntelliJ IDEA 2025.3.2",
            publisher="JetBrains s.r.o.",
            version="253.30387.90",
            estimatedSize="0 MB",
            currentUserOnly=False,
        ),
        Program(
            id=None,
            name="Riot Vanguard",
            publisher="Riot Games, Inc.",
            version="2.52.0",
            estimatedSize="0 MB",
            currentUserOnly=False,
        ),
        Program(
            id=None,
            name="Valorant",
            publisher="Riot Games, Inc.",
            version="2.52.0",
            estimatedSize="25 GB",
            currentUserOnly=True,
        ),
    ]
)

print(m.uploadNewVersion(data))

m.close()
del m
