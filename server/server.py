import flask, os, secrets
from argparse import ArgumentParser
from configparser import ConfigParser
import logging as log
import getpass

from tools.consts import PATH_TEMPLATES, PATH_ROOT, PATH_ASSETS, FILE_CONFIG
from tools.utils import validate_cpf
from models.database import db
from models.user import User, ADMIN_RULES

from routes.machine import machine_bp
from routes.user import user_bp
from routes.auth import auth_bp

class Server:
    def __init__(self):
        self.__config = ConfigParser()
        self.__parser = ArgumentParser('Machine Inventory Server')
        self.__parser.add_argument('command', choices=['runserver', 'generatecfg', 'createadmin'], help='opções de execução do servidor')
        self.__parser.add_argument('--name', help='nome do usuário')
        self.__parser.add_argument('--password', help='senha do usuário')
        self.__parser.add_argument('--cpf', help='CPF do usuário')
    
    def command_runserver(self):
        app = self.__setupApp()
        app.run(debug=True)
        
    def command_generatecfg(self):
        self.generate_secret_key()

    def command_createadmin(self, name:str=None, cpf:str=None, password:str=None):
        if cpf is None:
            cpf = input('CPF: ')

        if not validate_cpf(cpf):
            print('CPF deve possuir 11 dígitos e apenas números!')
            return

        if name is None:
            name = input('Nome: ')

        if password is None:
            password = getpass.getpass('Senha: ')
            password_confirm = getpass.getpass('Confirmar Senha: ')

            if password != password_confirm:
                print('As senhas são divergentes!')
                return

        if len(password) < 4:
            print('A senha deve conter pelo menos 4 caracteres')
            return
        
        app = self.__setupApp()

        with app.app_context():
            if User.query.filter_by(cpf=cpf).first():
                print('Já existe um usuário cadastrado com esse CPF!')
                return

            admin = User(name=name, cpf=cpf)
            admin.update_rules(ADMIN_RULES)
            admin.set_password(password)
            db.session.add(admin)
            db.session.commit()

    def exec(self, argv:list[str]=None):
        args = self.__parser.parse_args(argv)

        if os.path.exists(FILE_CONFIG):
            self.__config.read(FILE_CONFIG)

        match args.command:
            case 'generatecfg':     self.command_generatecfg()
            case 'createadmin':     self.command_createadmin(args.name, args.cpf, args.password)
            case 'runserver':       self.command_runserver()
            case _:                 print('undefined command')

    def generate_secret_key(self, update_config=True):
        key = secrets.token_urlsafe(32)
        
        if not update_config:
            return key
        
        if not self.__config.has_section('flask'):
            self.__config.add_section('flask')

        self.__config['flask']['SECRET_KEY'] = key
        
        with open(FILE_CONFIG, 'w') as f:
            self.__config.write(f)

        return key
    
    def get_secret_key(self) -> str:
        if 'SECRET_KEY' in os.environ:
            return os.environ['SECRET_KEY']

        if self.__config.has_option('flask', 'SECRET_KEY'):
            return self.__config.get('flask', 'SECRET_KEY')
        
        log.warning(f'[flask][SECRET_KEY] not set, verify the "{FILE_CONFIG}" file')
        return self.generate_secret_key(False)

    def __setupApp(self):
        app = flask.Flask(__file__, template_folder=PATH_TEMPLATES, static_folder=PATH_ASSETS, static_url_path='/assets')
        app.json.sort_keys = False
        app.json.ensure_ascii = False

        app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(PATH_ROOT, 'database.db')
        app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
        app.config['SECRET_KEY'] = os.environ['SECRET_KEY'] = self.get_secret_key()
        
        app.register_blueprint(machine_bp)
        app.register_blueprint(user_bp)
        app.register_blueprint(auth_bp)

        db.init_app(app)

        with app.app_context():
            db.create_all()

        return app


if __name__ == "__main__":
    server = Server()
    server.exec()