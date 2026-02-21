from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal, QTimer

from src.ui.autofiles.Ui_AuthForm import Ui_AuthForm
from models.dto import NewUserDTO
from .AbstractForm import AbstractForm

class AuthForm(AbstractForm):
    # AbstractForm Params
    WIDGETS_TO_BLOCK = [ 'leCpf', 'leName', 'lePass', 'lePassConfirm', 'btnAccess', 'btnCreateAccount' ]
    WIDGETS_TO_CLEAR = [ 'leCpf', 'leName', 'lePass', 'lePassConfirm', 'lbMessage' ]

    # Events
    authenticationRequired = Signal(str, str) # CPF, password
    createAccountRequired = Signal(NewUserDTO)

    # AuthForm Params
    STATE_AUTH = 0
    STATE_CREATE_ACCOUNT = 1

    def __init__(self, parent:QWidget=None, state=STATE_AUTH):
        super().__init__(parent)
        self._state = None

        self._ui = Ui_AuthForm()
        self._ui.setupUi(self)
        self.LABEL_MESSAGE = self._ui.lbMessage

        # eventos
        self._ui.btnAccess.clicked.connect(self.on_btnAccess_clicked)
        self._ui.btnCreateAccount.clicked.connect(self.on_btnCreateAccount_clicked)

        self.setState(state)

    #--------------------------------------------------------------------------
    # Métodos Públicos
    def setState(self, state:int):
        self.blockChanges(False)
        self.clear()

        if self._state == state:
            return
        
        self._state = state
        
        match state:
            case self.STATE_AUTH:
                self._ui.leName.hide()
                self._ui.lbName.hide()
                self._ui.lePassConfirm.hide()
                self._ui.lbPassConfirm.hide()
                self._ui.btnAccess.show()
            
            case self.STATE_CREATE_ACCOUNT:
                self._ui.leName.show()
                self._ui.lbName.show()
                self._ui.lePassConfirm.show()
                self._ui.lbPassConfirm.show()
                self._ui.btnAccess.hide()


    #--------------------------------------------------------------------------
    # Eventos
    def on_btnCreateAccount_clicked(self):
        match self._state:
            case self.STATE_AUTH:
                self.setState(self.STATE_CREATE_ACCOUNT)
            
            case self.STATE_CREATE_ACCOUNT:
                cpf = self._ui.leCpf.text()
                name = self._ui.leName.text()
                password = self._ui.lePass.text()
                password_confirm = self._ui.lePassConfirm.text()

                if '' in (cpf, name, password):
                    self.showMessage('Preencha todos os campos para prosseguir!', 5000)
                    return
                
                if password != password_confirm:
                    self.showMessage('As senhas são diferentes!', 5000)
                    return

                self.blockChanges()
                self.createAccountRequired.emit(NewUserDTO(cpf, password, name))

    def on_btnAccess_clicked(self):
        cpf = self._ui.leCpf.text()
        password = self._ui.lePass.text()

        if not cpf or not password:
            self.showMessage('Preencha todos os campos para prosseguir!', 5000)
            return
        
        self.blockChanges()
        self.authenticationRequired.emit(cpf, password)

    #--------------------------------------------------------------------------