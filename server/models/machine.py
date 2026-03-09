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
    accounts: list['MachineAccountModel']
    groups: list['MachineGroupModel']
    group_members: list['MachineGroupMemberModel']

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
    accounts = db.relationship('MachineAccountModel', backref='machine_version', lazy=True)
    groups = db.relationship('MachineGroupModel', backref='machine_version', lazy=True)
    group_members = db.relationship('MachineGroupMemberModel', backref='machine_version', lazy=True)
    removed = db.relationship('MachineRemoveModel', backref='machine_version', lazy=True)

    def __repr__(self):
        return f'<MachineVersionModel id="{self.id}">'

    def dto(self) -> dict:
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
            version=version, machine=None, programs=None, disks=None, adapters=None, memories=None, groups=None, accounts=None, group_members=None
        )
        d = model_to_dict(version, dict_ignore) if as_dict else None
        props = (
            ('machine', MachineModel, TablesRemove.MACHINE),
            ('programs', ProgramModel, TablesRemove.PROGRAM),
            ('disks', DiskModel, TablesRemove.DISK),
            ('adapters', NetworkAdapterModel, TablesRemove.NETWORK_ADAPTER),
            ('memories', PhysicalMemoryModel, TablesRemove.PHYSICAL_MEMORY),
            ('accounts', MachineAccountModel, TablesRemove.ACCOUNT),
            ('groups', MachineGroupModel, TablesRemove.GROUP),
            ('group_members', MachineGroupMemberModel, TablesRemove.GROUP_MEMBER),
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
            d['datetime'] = d['datetime'].strftime('%Y-%m-%d %H:%M:%S')
        else:
            v.machine = v.machine[0]

        return d if as_dict else v

    @classmethod
    def new(cls, data:MachineVersionData, group_members:list[dict], return_type:Literal['str', 'dataclass', 'dict']='dataclass', dict_ignore:set[str]=None) -> MachineVersionData | str | dict:
        latest_version = cls.get(data.version.mac, 'mac', data.version.owner_id)
        new_version = cls(id=str(uuid4()), mac=data.version.mac, owner_id=data.version.owner_id)
        
        db.session.add(new_version)

        props = (
            ('machine', TablesRemove.MACHINE),
            ('programs', TablesRemove.PROGRAM),
            ('disks', TablesRemove.DISK),
            ('adapters', TablesRemove.NETWORK_ADAPTER),
            ('memories', TablesRemove.PHYSICAL_MEMORY),
            ('accounts', TablesRemove.ACCOUNT),
            ('groups', TablesRemove.GROUP),
            ('group_members', TablesRemove.GROUP_MEMBER), # deve estar obrigatoriamente após accounts e groups
        )
        group_ids_by_sid = {}
        account_ids_by_sid = {}

        # adicionando dados extras
        for field_name, _ in props:
            if field_name == 'machine':
                data.machine.machine_version_id = new_version.id
                data.machine.id = str(uuid4())
            
            elif field_name == 'group_members':
                continue # gerado após a definição dos ids de groups e accounts

            else:
                for o in getattr(data, field_name):
                    o.machine_version_id = new_version.id
                    o.id = str(uuid4())

                    if field_name == 'groups':
                        group_ids_by_sid[o.sid] = o.id
                    elif field_name == 'accounts':
                        account_ids_by_sid[o.sid] = o.id

        
        # criando dados de group_members
        data.group_members = [
            MachineGroupMemberModel(
                id=str(uuid4()),
                machine_account_id=account_ids_by_sid[o['accountSid']],
                machine_group_id=group_ids_by_sid[o['groupSid']],
                machine_version_id=new_version.id,
            ) for o in group_members if o['accountSid'] in account_ids_by_sid and o['groupSid'] in group_ids_by_sid
        ]

        if latest_version:
            new_version.previous_id = latest_version.version.id
            account_ids_to_remove = []
            group_ids_to_remove = []
            account_ids_to_add = []
            group_ids_to_add = []

            for field_name, table_to_remove in props:
                if field_name == 'machine':
                    models_to_remove, models_to_add = cls._from_to([getattr(latest_version, field_name)], [getattr(data, field_name)])
                
                elif field_name == 'group_members':
                    # necessária a verificação de alteração em accounts e groups
                    old, new = getattr(latest_version, field_name), getattr(data, field_name)
                    models_to_remove, models_to_add = [], []

                    for o in old:
                        if o.machine_group_id in group_ids_to_remove or o.machine_account_id in account_ids_to_remove:
                            models_to_remove.append(o)

                    for o in new:
                        #in_old = MachineGroupMemberModel.query.filter_by(machine_group_id=o.machine_group_id, machine_account_id=o.machine_account_id, machine_version_id=latest_version.version.id).first()
                        #TODO: verificar se existe uma relação entre a conta e o grupo na versão anterior
                        in_old = True
                        if not in_old or o.machine_group_id in group_ids_to_add or o.machine_account_id in account_ids_to_add:
                            models_to_add.append(o)

                else:
                    old, new = getattr(latest_version, field_name), getattr(data, field_name)
                    models_to_remove, models_to_add = cls._from_to(old, new)

                if field_name == 'accounts':
                    account_ids_to_remove.extend([o.id for o in models_to_remove])
                    account_ids_to_add.extend([o.id for o in models_to_add])

                elif field_name == 'groups':
                    group_ids_to_remove.extend([o.id for o in models_to_remove])
                    group_ids_to_add.extend([o.id for o in models_to_add])

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
        return cls.get(new_version.id, 'id', as_dict=return_type == 'dict', dict_ignore=dict_ignore)

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

        # if to_remove:
        #     print(models1, models2, set_d1, set_d2, to_remove, to_add, sep='\n')

        return ids_to_remove, ids_to_add

        
class TablesRemove(Enum):
    DISK = auto()
    NETWORK_ADAPTER = auto()
    PHYSICAL_MEMORY = auto()
    PROGRAM = auto()
    MACHINE = auto()
    ACCOUNT = auto()
    GROUP = auto()
    GROUP_MEMBER = auto()

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

class MachineAccountModel(db.Model):
    __tablename__ = 'machine_account'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    sid:str = db.Column(db.String(100), nullable=False)
    name:str = db.Column(db.String(100), nullable=False)
    full_name:str = db.Column(db.String(100))
    description:str = db.Column(db.String(100))
    domain:str = db.Column(db.String(100))
    status:str = db.Column(db.String(100))
    disabled:bool = db.Column(db.Boolean)
    local:bool = db.Column(db.Boolean)
    lockout:bool = db.Column(db.Boolean)
    password_changeable:bool = db.Column(db.Boolean)
    password_expires:bool = db.Column(db.Boolean)
    password_required:bool = db.Column(db.Boolean)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<MachineAccountModel id="{self.id}">'

class MachineGroupModel(db.Model):
    __tablename__ = 'machine_group'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    sid:str = db.Column(db.String(100), nullable=False)
    name:str = db.Column(db.String(100), nullable=False)
    description:str = db.Column(db.String(100))
    domain:str = db.Column(db.String(100))
    status:str = db.Column(db.String(100))
    local:bool = db.Column(db.Boolean, nullable=False)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

class MachineGroupMemberModel(db.Model):
    __tablename__ = 'machine_group_member'
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    machine_group_id:str = db.Column(db.String(50), db.ForeignKey('machine_group.id'), nullable=False)
    machine_account_id:str = db.Column(db.String(50), db.ForeignKey('machine_account.id'), nullable=False)
    machine_version_id:str = db.Column(db.String(50), db.ForeignKey('machine_version.id'), nullable=False)

    def __repr__(self):
        return f'<MachineGroupMemberModel id="{self.id}">'