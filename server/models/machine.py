from uuid import uuid4
import datetime as dt

from models.database import db

class Machine(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    mac:str = db.Column(db.String(100), nullable=False)
    owner_id:int = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    versions = db.relationship('MachineExtra', backref='machine', lazy=True)

    def __repr__(self):
        return f"<Machine id='{self.id}'>"

    def dto(self, versions=False, owner=False):
        d = {
            'id': self.id,
            'mac': self.mac,
        }

        if owner:
            d['ownerId'] = self.owner.id
            d['ownerName'] = self.owner.name

        if versions:
            d['versions'] = [ v.dto() for v in self.versions ]

        return d

class MachineExtra(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    config_date:dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
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
    processor_clock_speed:int = db.Column(db.Integer)
    machine_id:int = db.Column(db.Integer, db.ForeignKey('machine.id'), nullable=False)
    disks = db.relationship('MachineDisk', backref='machine_extra', lazy=True)
    networkAdapters = db.relationship('MachineNetworkAdapter', backref='machine_extra', lazy=True)
    physicalMemories = db.relationship('MachinePhysicalMemory', backref='machine_extra', lazy=True)
    programs = db.relationship('MachineProgram', backref='machine_extra', lazy=True)

    def __repr__(self):
        return f"<MachineExtra id='{self.id}' config_date='{self.config_date}'>"

    def dto(self, machine=False, optimized_programs=False):
        d = {
            'id': self.id,
            'configDate': self.config_date.strftime('%Y-%m-%d %H:%M:%S'),
            'os': self.os,
            'title': self.title,
            'osArchitecture': self.os_architecture,
            'osInstallDate': self.os_install_date.strftime('%Y-%m-%d %H:%M:%S'),
            'osVersion': self.os_version,
            'osSerialNumber': self.os_serial_number,
            'organization': self.organization,
            'motherboard': self.motherboard,
            'motherboardManufacturer': self.motherboard_manufacturer,
            'processor': self.processor,
            'processorClockSpeed': self.processor_clock_speed,
            'disks': [ x.dto() for x in self.disks ],
            'networkAdapters': [ x.dto() for x in self.networkAdapters ],
            'physicalMemories': [ x.dto() for x in self.physicalMemories ],
            'programs': dict(columns=MachineProgram.COLUMNS, data=[ x.dto(as_list=True) for x in self.programs ]) if optimized_programs else [ x.dto() for x in self.programs ],
        }

        if machine:
            _d = self.machine.dto()
            _d.update(d)
            d = _d
        
        return d
    
class MachineDisk(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    serialNumber:str = db.Column(db.String(100))
    size:int = db.Column(db.Integer, nullable=False, default=0)
    model:str = db.Column(db.String(100))
    machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

    def __repr__(self):
        return f"<MachineDisk id='{self.id}'>"

    def dto(self, machineVersionId=False):
        d = {
            'id': self.id,
            'name': self.name,
            'serialNumber': self.serialNumber,
            'model': self.model,
        }
    
        if machineVersionId:
            d['machineVersionId'] = self.machine_extra_id
        
        return d
    
class MachineNetworkAdapter(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    mac:str = db.Column(db.String(100), nullable=False)
    machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

    def dto(self, machineVersionId=False):
        d = {
            'id': self.id,
            'name': self.name,
            'mac': self.mac,
        }
    
        if machineVersionId:
            d['machineVersionId'] = self.machine_extra_id
        
        return d
    
class MachinePhysicalMemory(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    capacity:int = db.Column(db.Integer, nullable=False)
    speed:int = db.Column(db.Integer, nullable=False)
    machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

    def dto(self, machineVersionId=False):
        d = {
            'id': self.id,
            'name': self.name,
            'capacity': self.capacity,
            'speed': self.speed,
        }
    
        if machineVersionId:
            d['machineVersionId'] = self.machine_extra_id
        
        return d
    
class MachineProgram(db.Model):
    COLUMNS = ['id', 'name', 'publisher', 'version', 'estimatedSize', 'currentUserOnly']

    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    publisher:str = db.Column(db.String(100))
    version:str = db.Column(db.String(100))
    estimatedSize:int = db.Column(db.Integer, nullable=False, default=0)
    currentUserOnly:bool = db.Column(db.Boolean, nullable=False)
    machine_extra_id:int = db.Column(db.Integer, db.ForeignKey('machine_extra.id'), nullable=False)

    def dto(self, machineVersionId=False, as_list=False):
        d = [ getattr(self, col) for col in self.COLUMNS ] if as_list else { col: getattr(self, col) for col in self.COLUMNS }

        if machineVersionId:
            if as_list:
                d.append(self.machine_extra_id)
            else:
                d['machineVersionId'] = self.machine_extra_id
        
        return d