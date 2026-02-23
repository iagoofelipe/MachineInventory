from PySide6.QtCore import QObject, Signal, QThreadPool
import time, json

from .api.server import ServerAPI
from . import dto
from . import structs
from tools.utils import Worker, get_machine_data

class AppModel(QObject):
    _instance = None # instanciado por MachineInventoryApp

    initializationFinished = Signal()
    initializationMessageChanged = Signal(str)
    authenticationFinished = Signal(bool)
    newUserFinished = Signal(bool)

    def __init__(self):
        super().__init__()
        self._server = ServerAPI()
        self._machine = None
        self._isAuthenticated = False
        self._user = None
        self._owner = None
        self._threadpool = QThreadPool(self)

    def getLastError(self): return self._server.getLastError()

    def machine(self) -> structs.Machine:
        return self._machine
    
    def logout(self):
        self._isAuthenticated = False
        self._user = None
        self._owner = None
        self._server.logout()
    
    def initialize(self):
        def func():
            self.initializationMessageChanged.emit('coletando dados do servidor...')
            self._isAuthenticated = self._server.readToken() and self._server.validateToken()
            
            if self._isAuthenticated:
                self.initializationMessageChanged.emit('coletando dados do usuário autenticado...')
                self._user = self._server.getUser()

            self.initializationMessageChanged.emit('coletando dados da máquina...')
            self._machine = get_machine_data()
            self.initializationMessageChanged.emit('inicialização finalizada')
        
        self._threadpool.start(Worker(func, callback=self.initializationFinished.emit))

    def isAuthenticated(self):
        return self._isAuthenticated
    
    def authenticatedUser(self) -> dto.UserDTO | None:
        return self._user
    
    def ownerUser(self) -> dto.UserDTO | None:
        return self._owner
    
    def getUser(self, cpf_or_id:str):
        return self._server.getUser(cpf_or_id)
    
    def auth(self, cpf:str, password:str):
        def func():
            success = self._server.generateToken(cpf, password)
            if success:
                self._owner = self._user = self._server.getUser()
                self._isAuthenticated = True
            
            return success

        self._threadpool.start(Worker(func, callback=self.authenticationFinished.emit, use_return=True))
    
    def newUser(self, data:dto.NewUserDTO):
        self._threadpool.start(Worker(self._server.newUser, data, callback=self.newUserFinished.emit, use_return=True))

    @classmethod
    def instance(cls) -> 'AppModel':
        return cls._instance
    
