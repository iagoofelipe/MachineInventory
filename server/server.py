import flask, os, secrets
from argparse import ArgumentParser
from configparser import ConfigParser
import logging as log

from tools.consts import PATH_TEMPLATES, PATH_ROOT, PATH_ASSETS, FILE_CONFIG
from models.database import db
from routes.machine import machine_bp
from routes.user import user_bp


class Server:
    def __init__(self):
        self.__config = ConfigParser()
        self.__parser = ArgumentParser('Machine Inventory Server')
        self.__parser.add_argument('command', choices=['runserver', 'generatecfg'], help='opções de execução do servidor')
        
    def exec(self, argv:list[str]=None):
        args = self.__parser.parse_args(argv)

        if os.path.exists(FILE_CONFIG):
            self.__config.read(FILE_CONFIG)

        if args.command == 'generatecfg':
            self.generate_secret_key()
            return
        
        elif args.command != 'runserver':
            raise RuntimeError('unknown command')
        
        # caso command == runserver
        app = flask.Flask(__file__, template_folder=PATH_TEMPLATES, static_folder=PATH_ASSETS, static_url_path='/assets')
        app.json.sort_keys = False

        app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(PATH_ROOT, 'database.db')
        app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
        app.config['SECRET_KEY'] = os.environ['SECRET_KEY'] = self.get_secret_key()
        
        app.register_blueprint(machine_bp)
        app.register_blueprint(user_bp)

        db.init_app(app)

        with app.app_context():
            db.create_all()
            
        app.run(debug=True)

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



if __name__ == "__main__":
    server = Server()
    server.exec()