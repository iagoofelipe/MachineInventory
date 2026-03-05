from flask_sqlalchemy import SQLAlchemy
import flask, configparser
from sqlalchemy import desc
from uuid import uuid4
import datetime as dt
from typing import Literal, Iterable, TypeVar
from enum import Enum, auto
from dataclasses import dataclass

_T1 = TypeVar('_T1')
_T2 = TypeVar('_T2')

app = flask.Flask(__name__)
cfg = configparser.ConfigParser()
with open('server.cfg') as f:
    cfg.read_file(f)

app.json.sort_keys = False
app.json.ensure_ascii = False

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + 'tests_version.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['SECRET_KEY'] = cfg['flask']['secret_key']

db = SQLAlchemy(app)

@dataclass
class MachineVersionData:
    version: 'MachineVersionModel'
    machine: 'MachineModel'
    disks: list['DiskModel']
    adapters: list['NetworkAdapterModel']
    programs: list['ProgramModel']
    memories: list['PhysicalMemoryModel']

def model_to_dict(obj, ignore:set[str]=None) -> dict:
    fields = { c.name for c in obj.__table__.columns }
    if ignore:
        fields -= ignore
    return { f: getattr(obj, f) for f in fields }

class MachineVersionModel(db.Model):
    __tablename__ = 'machine_version'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    datetime:dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
    mac:str = db.Column(db.String(25), nullable=False)
    previous_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'))
    previous = db.relationship('MachineVersionModel', remote_side=[id], backref=db.backref('next_version', uselist=False), uselist=False)
    disks = db.relationship('DiskModel', backref='machine_version', lazy=True)
    adapters = db.relationship('NetworkAdapterModel', backref='machine_version', lazy=True)
    memories = db.relationship('PhysicalMemoryModel', backref='machine_version', lazy=True)
    programs = db.relationship('ProgramModel', backref='machine_version', lazy=True)
    machines = db.relationship('MachineModel', backref='machine_version', lazy=True)
    removed = db.relationship('MachineRemoveModel', backref='machine_version', lazy=True)

    def __repr__(self):
        return f'<MachineVersionModel id="{self.id}">'

    @classmethod
    def get(cls, arg:str, column:Literal['id', 'datetime', 'mac'], as_model=True) -> None | MachineVersionData:
        """ monta a versão seguindo o versionamento
        \nretorna: dict(version=MachineVersionModel, machine=MachineModel, disks=list[DiskModel], adapters=NetworkAdapterModel, programs=ProgramModel, memories=PhysicalMemoryModel)
        """
        match column:
            case 'mac':         version = cls.query.filter(cls.mac == arg).order_by(desc(cls.datetime)).first()
            case 'id':          version = cls.query.get(arg)
            case 'datetime':    version = cls.query.filter_by(datetime=arg).first()
            case _:             raise ValueError(f'{column} is not a valid param')
        
        if not version:
            return

        # iterar sobre as versões
        v = version
        to_ignore = { table: [] for table in TablesRemove }
        versions_id = []

        while v:
            # coletar linhas removidas
            for removed in v.removed:
                to_ignore[removed.table].append(removed.ref_id)

            versions_id.append(v.id)
            v = v.previous

        # gerar estrutura, desconsiderando removidos
        v = MachineVersionData(
            version=version, machine=None, programs=None, disks=None, adapters=None, memories=None
        )
        props = (
            ('machine', MachineModel, TablesRemove.MACHINE),
            ('programs', ProgramModel, TablesRemove.PROGRAM),
            ('disks', DiskModel, TablesRemove.DISK),
            ('adapters', NetworkAdapterModel, TablesRemove.NETWORK_ADAPTER),
            ('memories', PhysicalMemoryModel, TablesRemove.PHYSICAL_MEMORY),
        )

        for key, model, to_ignore_key in props:
            query_result = model.query.filter(
                model.machine_version_id.in_(versions_id),
                ~model.id.in_(to_ignore[to_ignore_key])
            ).all()

            setattr(v, key, query_result)

        v.machine = v.machine[0]

        return v

    @classmethod
    def new(cls, data:MachineVersionData, return_id=False) -> MachineVersionData | str:
        latest_version = cls.get(data.version.mac, 'mac')
        new_version = cls(mac=data.version.mac)
        
        db.session.add(new_version)
        props = (
            ('machine', TablesRemove.MACHINE),
            ('programs', TablesRemove.PROGRAM),
            ('disks', TablesRemove.DISK),
            ('adapters', TablesRemove.NETWORK_ADAPTER),
            ('memories', TablesRemove.PHYSICAL_MEMORY),
        )

        # adicionando dados extras
        for field_name, _ in props:
            if field_name == 'machine':
                data.machine.machine_version_id = new_version.id
                data.machine.id = str(uuid4())
            else:
                for o in getattr(data, field_name):
                    o.machine_version_id = new_version.id
                    o.id = str(uuid4())


        if latest_version:
            new_version.previous_id = latest_version.version.id

            for field_name, table_to_remove in props:
                if field_name == 'machine':
                    old, new = [getattr(latest_version, field_name)], [getattr(data, field_name)]
                else:
                    old, new = getattr(latest_version, field_name), getattr(data, field_name)
                models_to_remove, models_to_add = cls._from_to(old, new)

                # inserindo valores adicionados
                for model in models_to_add:
                    model.machine_version = new_version
                db.session.add_all(models_to_add)

                # removendo valores desatualizados
                to_remove = [ MachineRemoveModel(table=table_to_remove, ref_id=o.id, machine_version=new_version) for o in models_to_remove ]
                db.session.add_all(to_remove)
        else:
            for field_name, _ in props:
                if field_name == 'machine':
                    db.session.add(getattr(data, field_name))
                else:
                    db.session.add_all(getattr(data, field_name))

        db.session.commit()
        return new_version.id if return_id else cls.get(new_version.mac, 'mac')

    @staticmethod
    def _from_to(models1:Iterable[_T1], models2:Iterable[_T2]) -> tuple[list[_T1], list[_T2]]:
        """ retorna a lista de models1 a serem removidos e a lista de models2 a serem adicionados  """
        models_by_tuple = {}
        set_d1 = set()
        set_d2 = set()

        for _set, models in ((set_d1, models1), (set_d2, models2)):
            for model in models:
                d = model_to_dict(model, {'id', 'machine_version_id'})
                t = tuple(d.values())
                _set.add(t)
                models_by_tuple[t] = model

        to_remove = set_d1 - set_d2
        to_add = set_d2 - set_d1

        # coletando IDs a partir dos objetos
        ids_to_remove = list(map(models_by_tuple.pop, to_remove))
        ids_to_add = list(map(models_by_tuple.pop, to_add))

        return ids_to_remove, ids_to_add

        
class TablesRemove(Enum):
    DISK = auto()
    NETWORK_ADAPTER = auto()
    PHYSICAL_MEMORY = auto()
    PROGRAM = auto()
    MACHINE = auto()

class MachineRemoveModel(db.Model):
    __tablename__ = 'machine_remove'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    table:str = db.Column(db.Enum(TablesRemove), nullable=False)
    ref_id:str = db.Column(db.String(50), nullable=False)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<MachineRemoveModel id="{self.id}">'

class DiskModel(db.Model):
    __tablename__ = 'machine_disk'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    serial_number:str = db.Column(db.String(100))
    model:str = db.Column(db.String(100))
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<DiskModel id="{self.id}">'

class NetworkAdapterModel(db.Model):
    __tablename__ = 'machine_network_adapter'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    mac:str = db.Column(db.String(100), nullable=False)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<NetworkAdapterModel id="{self.id}">'

class PhysicalMemoryModel(db.Model):
    __tablename__ = 'machine_physical_memory'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    capacity:str = db.Column(db.String(100), nullable=False)
    speed:str = db.Column(db.String(100))
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<PhysicalMemoryModel id="{self.id}">'

class ProgramModel(db.Model):
    __tablename__ = 'machine_program'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    publisher:str = db.Column(db.String(100))
    version:str = db.Column(db.String(100))
    estimated_size:int = db.Column(db.Integer, nullable=False, default=0)
    current_user_only:bool = db.Column(db.Boolean, nullable=False)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<ProgramModel id="{self.id}">'

class MachineModel(db.Model):
    __tablename__ = 'machine'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    os:str = db.Column(db.String(100), nullable=False)
    title:str = db.Column(db.String(100), nullable=False)
    os_architecture:str = db.Column(db.String(100))
    os_install_date:dt.datetime = db.Column(db.DateTime)
    os_version:str = db.Column(db.String(100))
    os_serial_number:str = db.Column(db.String(100))
    organization:str = db.Column(db.String(100))
    motherboard:str = db.Column(db.String(100))
    motherboard_manufacturer:str = db.Column(db.String(100))
    processor:str = db.Column(db.String(100))
    processor_clock_speed:int = db.Column(db.String(50))
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)
    
    def __repr__(self):
        return f'<MachineModel id="{self.id}">'

with app.app_context():
    db.create_all()

    data = MachineVersionData(
        version=MachineVersionModel(mac='D8:5E:D3:E8:B6:ED'),
        machine=MachineModel(
            os='Microsoft Windows 11 Pro',
            title='GUI',
            os_architecture="64 bits",
            os_install_date=dt.datetime.strptime("2026-01-19 15:15:23", "%Y-%m-%d %H:%M:%S"),
            os_version="10.0.26200",
            os_serial_number="00330-80000-00000-AA779",
            organization=None,
            motherboard="B450M DS3H V2",
            motherboard_manufacturer="Gigabyte Technology Co., Ltd.",
            processor="AMD Ryzen 5 4500 6-Core Processor",
            processor_clock_speed="3.60 GHz"
        ),
        disks=[
            DiskModel(name="KINGSTON SNV2S1000G", serial_number=None, model="KINGSTON SNV2S1000G"),
        ],
        adapters=[
            NetworkAdapterModel(name="Realtek Gaming GbE Family Controller", mac="D8:5E:D3:E8:B6:ED"),
            NetworkAdapterModel(name="WAN Miniport (IP)", mac="04:76:20:52:41:53"),
        ],

        memories=[
            PhysicalMemoryModel(name="Physical Memory 0", capacity="8 GB", speed="2133 MT/s"),
            PhysicalMemoryModel(name="Physical Memory 2", capacity="8 GB", speed="2133 MT/s"),
        ],
        programs=[
            ProgramModel(
                name="Git", publisher="The Git Development Community", version="2.52.0",
                estimated_size="346 MB", current_user_only=False,
            ),
            ProgramModel(
                name="IntelliJ IDEA 2025.3.2", publisher="JetBrains s.r.o.", version="253.30387.90",
                estimated_size="0 MB", current_user_only=False,
            ),
            ProgramModel(
                name="Riot Vanguard", publisher="Riot Games, Inc.", version="2.52.0",
                estimated_size="0 MB",
                current_user_only=False,
            ),
            ProgramModel(
                name="Valorant", publisher="Riot Games, Inc.", version="2.55.0",
                estimated_size="28 GB",
                current_user_only=True,
            ),
        ]
    )

    MachineVersionModel.new(data, return_id=True)

    # version = MachineVersionModel.get('D8:5E:D3:E8:B6:ED', 'mac')
    # print(model_to_dict(version.version))
    # print(model_to_dict(version.machine))
    # print(*[model_to_dict(o) for o in version.disks], sep='\n')
    # print(*[model_to_dict(o) for o in version.adapters], sep='\n')
    # print(*[model_to_dict(o) for o in version.memories], sep='\n')
    # print(*[model_to_dict(o) for o in version.programs], sep='\n')
