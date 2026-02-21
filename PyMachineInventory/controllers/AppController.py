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
    # Eventos AppModel
    def on_AppModel_initializationFinished(self):
        isAuth = self._model.isAuthenticated()
        self._view.setUi(AppView.UI.MAIN if isAuth else AppView.UI.AUTH)
        
    def on_AppModel_authenticationFinished(self, success:bool):
        if success:
            self._view.setUi(AppView.UI.MAIN)
        else:
            form = self._view.authForm()
            form.showMessage(self._model.getLastError())
            form.blockChanges(False)

    def on_AppModel_newUserFinished(self, success:bool):
        form = self._view.authForm()
        if success:
            form.setState(form.STATE_AUTH)
        else:
            form.showMessage(self._model.getLastError())
            form.blockChanges(False)

    #------------------------------------------------------------------------
    # Eventos AppView    
    def on_AppView_uiChanged(self, ui:AppView.UI):
        match ui:
            case AppView.UI.MAIN:
                form = self._view.syncForm()
                # form.authenticationRequired.connect(self._model.auth)
                # form.newUserRequired.connect(self._model.newUser)
                form.syncRequired.connect(self.on_SyncForm_syncRequired)

            case AppView.UI.AUTH:
                form = self._view.authForm()
                form.authenticationRequired.connect(self._model.auth)
                form.createAccountRequired.connect(self._model.newUser)

    #------------------------------------------------------------------------

    def on_SyncForm_syncRequired(self, owner:dto.UserDTO):
        print('Syncronization required to owner', owner)