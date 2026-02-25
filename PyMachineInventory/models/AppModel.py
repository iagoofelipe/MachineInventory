from PySide6.QtCore import QObject, Signal, QThreadPool

from .api.server import ServerAPI
from . import structs
from . import structs
from tools.utils import Worker, get_machine_data, get_default_mac

class AppModel(QObject):
    _instance = None # instanciado por MachineInventoryApp

    initializationFinished = Signal()
    initializationMessageChanged = Signal(str)
    authenticationFinished = Signal(bool)
    newUserFinished = Signal(bool)
    syncMachineFinished = Signal(bool)

    def __init__(self):
        super().__init__()
        self._server = ServerAPI()
        self._machine = None
        self._isAuthenticated = False
        self._user = None
        self._previousTitle = ''
        self._threadpool = QThreadPool(self)

    #-------------------------------------------------------------------------------
    # Métodos Públicos
    def initialize(self):
        def func():
            self.initializationMessageChanged.emit('coletando dados do servidor...')
            self._isAuthenticated = self._server.readtoken() and self._server.validateToken()
            
            if self._isAuthenticated:
                self.initializationMessageChanged.emit('coletando dados do usuário autenticado...')
                self._user = self._server.getUser()

            self.initializationMessageChanged.emit('coletando dados da máquina...')
            self._machine = get_machine_data()
            
            mac = get_default_mac(self._machine.networkAdapters)
            if mac:
                machine = self._server.getMachineByIdOrMac(mac)
                self._previousTitle = machine['title'] if machine else ''
            
            self.initializationMessageChanged.emit('inicialização finalizada')
        
        self._threadpool.start(Worker(func, callback=self.initializationFinished.emit))

    def getLastError(self): return self._server.getLastError()

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    #-------------------------------------------------------------------------------
    # Métodos da Máquina
    def machine(self) -> structs.Machine:
        return self._machine

    def machinePreviousTitle(self) -> str:
        return self._previousTitle

    def syncMachine(self, owner:structs.User, machineTitle:str):
        self._threadpool.start(Worker(self._server.syncMachine, machineTitle=machineTitle, machine=self._machine, ownerId=owner.id, callback=self.syncMachineFinished.emit, use_return=True))

    #-------------------------------------------------------------------------------
    # Métodos de Autenticação
    def logout(self):
        self._isAuthenticated = False
        self._user = None
        self._server.logout()

    def isAuthenticated(self):
        return self._isAuthenticated
    
    def auth(self, cpf:str, password:str):
        def func():
            success = self._server.generateToken(cpf, password)
            if success:
                self._user = self._server.getUser()
                self._isAuthenticated = True
            
            return success

        self._threadpool.start(Worker(func, callback=self.authenticationFinished.emit, use_return=True))
    
    #-------------------------------------------------------------------------------
    # Métodos de Usuário
    def authenticatedUser(self) -> structs.User | None:
        return self._user
    
    def getUser(self, cpf_or_id:str):
        return self._server.getUser(cpf_or_id)
    
    def newUser(self, data:structs.NewUser):
        self._threadpool.start(Worker(self._server.newUser, data, callback=self.newUserFinished.emit, use_return=True))

    #-------------------------------------------------------------------------------