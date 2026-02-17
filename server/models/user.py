from werkzeug.security import generate_password_hash, check_password_hash
from uuid import uuid4
from typing import Iterable

from models.database import db

# RULE: (Flag, Description)
USER_RULES = {
    'CREATE_USERS': (1 << 1, "Criação de novos usuários"),
    'QUERY_OTHER_USERS': (1 << 2, "Consulta de outros usuários"),
    'ADD_MACHINE': (1 << 3, "Adição de novas máquinas vinculadas a outros usuários"),
    'UPDATE_USER_RULES': (1 << 4, "Alteração de regras a si e outros usuários"),
}

USER_RULE_BY_FLAGS = { v[0]: k for k, v in USER_RULES.items() }
ADMIN_RULES = tuple(USER_RULES)

class User(db.Model):
    SEP_RULES = '|'

    # User Rules
    RULE_CREATE_USERS = USER_RULES['CREATE_USERS'][0]
    RULE_QUERY_OTHER_USERS = USER_RULES['QUERY_OTHER_USERS'][0]
    RULE_ADD_MACHINE = USER_RULES['ADD_MACHINE'][0]
    RULE_UPDATE_USER_RULES = USER_RULES['UPDATE_USER_RULES'][0]

    id:str = db.Column(db.String(50), primary_key=True, default=lambda: str(uuid4()))
    name:str = db.Column(db.String(100), nullable=False)
    cpf:str = db.Column(db.String(11), unique=True, nullable=False)
    password_hash:str = db.Column(db.String(128), nullable=False)
    rules_combinated: int = db.Column(db.Integer, nullable=False, default=0)
    rules_splitted: str = db.Column(db.String(100), nullable=False, default='')
    machines = db.relationship('Machine', backref='owner', lazy=True)

    def __repr__(self):
        return f"<User id='{self.id}' name='{self.name}'>"
    
    def set_password(self, psw:str):
        self.password_hash = generate_password_hash(psw)

    def check_password(self, psw:str):
        return check_password_hash(self.password_hash, psw)
    
    def check_rule(self, rule:int|str):
        if type(rule) == str:
            rule = USER_RULES[rule][0]
        
        return bool(self.rules_combinated & rule)
    
    def update_rules(self, to_add:Iterable[str|int]=None, to_remove:Iterable[str|int]=None):
        if not to_add and not to_remove:
            return
        
        # converte o nome da regra em sua respectiva flag
        final_rules = list(map(lambda x: USER_RULES[x][0], self.rules_splitted.split(self.SEP_RULES))) if self.rules_splitted else []
        
        if to_add:
            for add in to_add:
                rule = USER_RULES[add][0] if type(add) is str else add
                final_rules.append(rule)
        
        if to_remove:
            for remove in to_remove:
                rule = USER_RULES[remove][0] if type(remove) is str else remove
                
                if rule in final_rules:
                    final_rules.remove(rule)
        
        rules_combinated = 0
        for rule in set(final_rules):
            rules_combinated |= rule

        self.rules_combinated = rules_combinated
        self.rules_splitted = self.SEP_RULES.join(map(lambda x: USER_RULE_BY_FLAGS[x], final_rules))
    
    def dto(self, machines=False, **kwargs):
        d = {
            'id': self.id,
            'name': self.name, 
            'cpf': self.cpf,
            'rules': self.rules_splitted.split(self.SEP_RULES) if self.rules_splitted else []
        }

        if machines:
            d['machines'] = [ m.dto(**kwargs) for m in self.machines ]

        return d