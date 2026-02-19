from PySide6.QtCore import QObject, Signal

from models.AppModel import AppModel
from models import dto
from views.AppView import AppView
from views.forms.SyncForm import SyncForm

class AppController(QObject):
    _instance = None # instanciado por MachineInventoryApp

    def __init__(self):
        super().__init__()
        self._model = AppModel.instance()
        self._view = AppView.instance()

        self._model.initializationFinished.connect(self.on_AppModel_initializationFinished)
        self._model.authenticationFinished.connect(self.on_AppModel_authenticationFinished)
        self._model.newUserFinished.connect(self.on_AppModel_newUserFinished)
        self._view.uiChanged.connect(self.on_AppView_uiChanged)

    #------------------------------------------------------------------------
    # Métodos Públicos
    def initialize(self):
        self._view.initialize()
        self._model.initialize()

    @classmethod
    def instance(cls) -> 'AppController':
        return cls._instance
    
    #------------------------------------------------------------------------
    # Eventos
    def on_AppModel_initializationFinished(self):
        isAuth = self._model.isAuthenticated()
        state = SyncForm.State.MACHINE_OWNER if isAuth else SyncForm.State.AUTH
        self._view.setUi(AppView.UI.MAIN, initial_state=state)

    def on_AppModel_authenticationFinished(self, success:bool):
        form = self._view.syncForm()
        if success:
            form.setState(SyncForm.State.MACHINE_OWNER)
        else:
            form.showMessage(self._model.getLastError())

    def on_AppModel_newUserFinished(self, success:bool):
        form = self._view.syncForm()
        if success:
            form.setState(SyncForm.State.AUTH)
        else:
            form.showMessage(self._model.getLastError())

        
    def on_AppView_uiChanged(self, ui:AppView.UI):
        match ui:
            case AppView.UI.MAIN:
                form = self._view.syncForm()
                form.authenticationRequired.connect(self._model.auth)
                form.newUserRequired.connect(self._model.newUser)

    # def on_SyncForm_authenticationRequired(self, cpf:str, password:str):
    #     self._model.auth(cpf, password)

    # def on_SyncForm_newUserRequired(self, data:dto.NewUserDTO):
    #     self._model.newUser(data)

    #------------------------------------------------------------------------
    # Métodos Privados
    #------------------------------------------------------------------------