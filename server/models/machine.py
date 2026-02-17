from uuid import uuid4
import datetime as dt

from models.database import db

class Machine(db.Model):
    id :str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    mac:str = db.Column(db.String(100), nullable=False)
    owner_id: int = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    versions = db.relationship('MachineExtra', backref='machine', lazy=True)

    def __repr__(self):
        return f"<Machine id='{self.id}' title='{self.title}'>"

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
    id :str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    config_date: dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
    os: str = db.Column(db.String(100), nullable=False)
    title: str = db.Column(db.String(100), nullable=False)
    os_architecture: str = db.Column(db.String(100))
    os_install_date: dt.datetime = db.Column(db.DateTime)
    os_version: str = db.Column(db.String(100))
    os_serial_number: str = db.Column(db.String(100))
    organization: str = db.Column(db.String(100))
    motherboard: str = db.Column(db.String(100))
    motherboard_manufacturer: str = db.Column(db.String(100))
    processor: str = db.Column(db.String(100))
    processor_clock_speed: int = db.Column(db.Integer)
    machine_id: int = db.Column(db.Integer, db.ForeignKey('machine.id'), nullable=False)

    def __repr__(self):
        return f"<MachineExtra id='{self.id}' config_date='{self.config_date}'>"

    def dto(self):
        return {
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
        }