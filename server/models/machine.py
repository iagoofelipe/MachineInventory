from sqlalchemy import desc
from uuid import uuid4
import datetime as dt
from typing import Literal, Iterable, TypeVar
from enum import Enum, auto
from dataclasses import dataclass

from models.database import db
from tools.utils import model_to_dict

_T1 = TypeVar('_T1')
_T2 = TypeVar('_T2')

@dataclass
class MachineVersionData:
    version: 'MachineVersionModel'
    machine: 'MachineModel'
    disks: list['DiskModel']
    adapters: list['NetworkAdapterModel']
    programs: list['ProgramModel']
    memories: list['PhysicalMemoryModel']

class MachineVersionModel(db.Model):
    __tablename__ = 'machine_version'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    datetime:dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
    mac:str = db.Column(db.String(25), nullable=False)
    previous_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'))
    owner_id:str = db.Column(db.String(50), db.ForeignKey('user.id'), nullable=False)
    previous = db.relationship('MachineVersionModel', remote_side=[id], backref=db.backref('next_version', uselist=False), uselist=False)
    disks = db.relationship('DiskModel', backref='machine_version', lazy=True)
    adapters = db.relationship('NetworkAdapterModel', backref='machine_version', lazy=True)
    memories = db.relationship('PhysicalMemoryModel', backref='machine_version', lazy=True)
    programs = db.relationship('ProgramModel', backref='machine_version', lazy=True)
    machines = db.relationship('MachineModel', backref='machine_version', lazy=True)
    removed = db.relationship('MachineRemoveModel', backref='machine_version', lazy=True)

    def __repr__(self):
        return f'<MachineVersionModel id="{self.id}">'

    def dto(self):
        return self.get(self.id, 'id', as_dict=True, dict_ignore={'machine_version_id'})

    @classmethod
    def get(cls, arg:str, column:Literal['id', 'datetime', 'mac'], owner_id:str=None, as_dict=False, dict_ignore:set[str]=None) -> None | MachineVersionData | dict:
        """ monta a versão seguindo o versionamento
        \nretorna: dict(version=MachineVersionModel, machine=MachineModel, disks=list[DiskModel], adapters=NetworkAdapterModel, programs=ProgramModel, memories=PhysicalMemoryModel)
        """
        match column:
            case 'mac':         version = cls.query.filter(cls.mac == arg,  cls.owner_id == owner_id).order_by(desc(cls.datetime)).first()
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
        d = model_to_dict(version) if as_dict else None
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

            if as_dict:
                d[key] = [ model_to_dict(o, dict_ignore) for o in query_result ]
            else:
                setattr(v, key, query_result)

        if as_dict:
            d['machine'] = d['machine'][0]
        else:
            v.machine = v.machine[0]

        return d if as_dict else v

    @classmethod
    def new(cls, data:MachineVersionData, return_type:Literal['str', 'dataclass', 'dict']='dataclass', dict_ignore:set[str]=None) -> MachineVersionData | str | dict:
        latest_version = cls.get(data.version.mac, 'mac', data.version.owner_id)
        new_version = cls(id=str(uuid4()), mac=data.version.mac, owner_id=data.version.owner_id)
        
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

        if return_type == 'str':
             return new_version.id
        return cls.get(new_version.id, 'id', as_dict=return_type == 'dict')

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
    size:str = db.Column(db.String(50), nullable=False, default='0 MB')
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
    speed:str = db.Column(db.String(100), nullable=False)
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

# class Machine(db.Model):
#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     mac:str = db.Column(db.String(100), nullable=False)
#     owner_id:int = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
#     versions = db.relationship('MachineExtra', backref='machine', lazy=True)

#     def __repr__(self):
#         return f"<Machine id='{self.id}'>"

#     def dto(self, versions=False, owner=False):
#         d = {
#             'id': self.id,
#             'mac': self.mac,
#         }

#         if owner:
#             d['ownerId'] = self.owner.id
#             d['ownerName'] = self.owner.name

#         if versions:
#             d['versions'] = [ v.dto() for v in self.versions ]

#         return d

# class MachineExtra(db.Model):
#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     config_date:dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
#     os:str = db.Column(db.String(100), nullable=False)
#     title:str = db.Column(db.String(100), nullable=False)
#     os_architecture:str = db.Column(db.String(100))
#     os_install_date:dt.datetime = db.Column(db.DateTime)
#     os_version:str = db.Column(db.String(100))
#     os_serial_number:str = db.Column(db.String(100))
#     organization:str = db.Column(db.String(100))
#     motherboard:str = db.Column(db.String(100))
#     motherboard_manufacturer:str = db.Column(db.String(100))
#     processor:str = db.Column(db.String(100))
#     processor_clock_speed:int = db.Column(db.Integer)
#     machine_id:int = db.Column(db.Integer, db.ForeignKey('machine.id'), nullable=False)
#     disks = db.relationship('MachineDisk', backref='machine_extra', lazy=True)
#     networkAdapters = db.relationship('MachineNetworkAdapter', backref='machine_extra', lazy=True)
#     physicalMemories = db.relationship('MachinePhysicalMemory', backref='machine_extra', lazy=True)
#     programs = db.relationship('MachineProgram', backref='machine_extra', lazy=True)

#     def __repr__(self):
#         return f"<MachineExtra id='{self.id}' config_date='{self.config_date}'>"

#     def dto(self, machine=False, optimized_programs=False):
#         d = {
#             'id': self.id,
#             'configDate': self.config_date.strftime('%Y-%m-%d %H:%M:%S'),
#             'os': self.os,
#             'title': self.title,
#             'osArchitecture': self.os_architecture,
#             'osInstallDate': self.os_install_date.strftime('%Y-%m-%d %H:%M:%S'),
#             'osVersion': self.os_version,
#             'osSerialNumber': self.os_serial_number,
#             'organization': self.organization,
#             'motherboard': self.motherboard,
#             'motherboardManufacturer': self.motherboard_manufacturer,
#             'processor': self.processor,
#             'processorClockSpeed': self.processor_clock_speed,
#             'disks': [ x.dto() for x in self.disks ],
#             'networkAdapters': [ x.dto() for x in self.networkAdapters ],
#             'physicalMemories': [ x.dto() for x in self.physicalMemories ],
#             'programs': dict(columns=MachineProgram.COLUMNS, data=[ x.dto(as_list=True) for x in self.programs ]) if optimized_programs else [ x.dto() for x in self.programs ],
#         }

#         if machine:
#             _d = self.machine.dto()
#             _d.update(d)
#             d = _d
        
#         return d
    
# class MachineDisk(db.Model):
#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     name:str = db.Column(db.String(100), nullable=False)
#     serialNumber:str = db.Column(db.String(100))
#     size:int = db.Column(db.Integer, nullable=False, default=0)
#     model:str = db.Column(db.String(100))
#     machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

#     def __repr__(self):
#         return f"<MachineDisk id='{self.id}'>"

#     def dto(self, machineVersionId=False):
#         d = {
#             'id': self.id,
#             'name': self.name,
#             'serialNumber': self.serialNumber,
#             'model': self.model,
#         }
    
#         if machineVersionId:
#             d['machineVersionId'] = self.machine_extra_id
        
#         return d
    
# class MachineNetworkAdapter(db.Model):
#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     name:str = db.Column(db.String(100), nullable=False)
#     mac:str = db.Column(db.String(100), nullable=False)
#     machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

#     def dto(self, machineVersionId=False):
#         d = {
#             'id': self.id,
#             'name': self.name,
#             'mac': self.mac,
#         }
    
#         if machineVersionId:
#             d['machineVersionId'] = self.machine_extra_id
        
#         return d
    
# class MachinePhysicalMemory(db.Model):
#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     name:str = db.Column(db.String(100), nullable=False)
#     capacity:int = db.Column(db.Integer, nullable=False)
#     speed:int = db.Column(db.Integer, nullable=False)
#     machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

#     def dto(self, machineVersionId=False):
#         d = {
#             'id': self.id,
#             'name': self.name,
#             'capacity': self.capacity,
#             'speed': self.speed,
#         }
    
#         if machineVersionId:
#             d['machineVersionId'] = self.machine_extra_id
        
#         return d
    
# class MachineProgram(db.Model):
#     COLUMNS = ['id', 'name', 'publisher', 'version', 'estimatedSize', 'currentUserOnly']

#     id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
#     name:str = db.Column(db.String(100), nullable=False)
#     publisher:str = db.Column(db.String(100))
#     version:str = db.Column(db.String(100))
#     estimatedSize:int = db.Column(db.Integer, nullable=False, default=0)
#     currentUserOnly:bool = db.Column(db.Boolean, nullable=False)
#     machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

#     def dto(self, machineVersionId=False, as_list=False):
#         d = [ getattr(self, col) for col in self.COLUMNS ] if as_list else { col: getattr(self, col) for col in self.COLUMNS }

#         if machineVersionId:
#             if as_list:
#                 d.append(self.machine_extra_id)
#             else:
#                 d['machineVersionId'] = self.machine_extra_id
        
#         return d