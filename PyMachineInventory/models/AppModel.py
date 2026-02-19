from PySide6.QtCore import QObject, Signal, QThreadPool
import time

from .server import ServerConnection
from . import dto
from tools.utils import Worker

class AppModel(QObject):
    _instance = None # instanciado por MachineInventoryApp

    initializationFinished = Signal()
    authenticationFinished = Signal(bool)
    newUserFinished = Signal(bool)

    def __init__(self):
        super().__init__()
        self._server = ServerConnection()
        self._isAuthenticated = False
        self._user = None
        self._threadpool = QThreadPool(self)

    def getLastError(self): return self._server.getLastError()
    
    def initialize(self):
        def func():
            self._isAuthenticated = self._server.readToken() and self._server.validateToken()
            
            if self._isAuthenticated:
                self._user = self._server.getUser()
            
            time.sleep(2) # TODO: coletar dados da máquina com wmi
        
        self._threadpool.start(Worker(func, callback=self.initializationFinished.emit))

    def isAuthenticated(self):
        return self._isAuthenticated
    
    def authenticatedUser(self) -> dto.UserDTO | None:
        return self._user
    
    def getUser(self, cpf_or_id:str):
        return self._server.getUser(cpf_or_id)
    
    def auth(self, cpf:str, password:str):
        def func():
            success = self._server.generateToken(cpf, password)
            if success:
                self._user = self._server.getUser()
                self._isAuthenticated = True
            
            return success

        self._threadpool.start(Worker(func, callback=self.authenticationFinished.emit, use_return=True))
    
    def newUser(self, data:dto.NewUserDTO):
        self._threadpool.start(Worker(self._server.newUser, data, callback=self.newUserFinished.emit, use_return=True))

    @classmethod
    def instance(cls) -> 'AppModel':
        return cls._instance
    
