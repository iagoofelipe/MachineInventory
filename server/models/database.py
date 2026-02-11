
#############################################################
# MODELS/DATABASE
# ---------------
# Descrição: Funções e classes para a manipulação da base de dados
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash

from models.dto import UserDTO

db = SQLAlchemy()

class UserModel(db.Model):
    id:int = db.Column(db.String(50), primary_key=True)
    name:str = db.Column(db.String(100), nullable=False)
    cpf:str = db.Column(db.String(50), unique=True, nullable=False)
    password_hash = db.Column(db.String(128), nullable=False)

    def __repr__(self):
        return f'<User id={self.id} name={self.name}>'
    
    def set_password(self, psw:str):
        self.password_hash = generate_password_hash(psw)

    def check_password(self, psw:str):
        return check_password_hash(self.password_hash, psw)
    
    def userDTO(self, as_dict=False) -> UserDTO | dict[str, str]:
        return {
            'id': self.id,
            'name': self.name, 
            'cpf': self.cpf, 
        } if as_dict else UserDTO(self.id, self.name, self.cpf)