
#############################################################
# MODELS/DATABASE
# ---------------
# Descrição: Funções e classes para a manipulação da base de dados
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

from flask_sqlalchemy import SQLAlchemy
from uuid import uuid4
import datetime as dt

from models.dto import UserDTO

db = SQLAlchemy()

class User(db.Model):
    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    cpf:str = db.Column(db.String(50), unique=True, nullable=False)
    machines = db.relationship('Machine', backref='owner', lazy=True)
    
    def dto(self, as_dict=False) -> UserDTO | dict[str, str]:
        return {
            'id': self.id,
            'name': self.name, 
            'cpf': self.cpf, 
        } if as_dict else UserDTO(self.id, self.name, self.cpf)

class MachineExtra(db.Model):
    id :str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    config_date: dt.datetime = db.Column(db.DateTime, default=dt.datetime.now, nullable=False)
    os: str = db.Column(db.String(100), nullable=False)
    os_architecture: str = db.Column(db.String(100))
    os_install_date: dt.datetime = db.Column(db.DateTime)
    os_version: str = db.Column(db.String(100))
    os_serial_number: str = db.Column(db.String(100))
    organization: str = db.Column(db.String(100))
    motherboard_manufacturer: str = db.Column(db.String(100))
    processor: str = db.Column(db.String(100))
    machine_id: int = db.Column(db.Integer, db.ForeignKey('machine.id'), nullable=False)

    def dto(self):
        return {
            'id': self.id,
            'config_date': self.config_date.strftime('%Y-%m-%d %H:%M:%S'),
            'os': self.os,
            'os_architecture': self.os_architecture,
            'os_install_date': self.os_install_date.strftime('%Y-%m-%d %H:%M:%S'),
            'os_version': self.os_version,
            'os_serial_number': self.os_serial_number,
            'organization': self.organization,
            'motherboard_manufacturer': self.motherboard_manufacturer,
            'processor': self.processor,
            'title': self.machine.title,
            'mac': self.machine.mac,
            'owner_id': self.machine.owner_id,
        }

class Machine(db.Model):
    id :str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    title: str = db.Column(db.String(100), nullable=False)
    mac:str = db.Column(db.String(100), nullable=False)
    owner_id: int = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    versions = db.relationship('MachineExtra', backref='machine', lazy=True)

    def dto(self):
        return {
            'id': self.id,
            'title': self.title,
            'mac': self.mac,
            'owner_id': self.owner_id,
        }