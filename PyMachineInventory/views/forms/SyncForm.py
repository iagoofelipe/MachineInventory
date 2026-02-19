from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal, Qt, QTimer
from enum import Enum, auto

from src.ui.autofiles.Ui_SyncForm import Ui_SyncForm
from models.dto import NewUserDTO, UserDTO
from models.AppModel import AppModel

class SyncForm(QWidget):
    authenticationRequired = Signal(str, str) # CPF, password
    newUserRequired = Signal(NewUserDTO)

    class State(Enum):
        AUTH = auto()
        NEW_USER = auto()
        MACHINE_OWNER = auto()
        READY_TO_SYNC = auto()
    
    _WIDGETS_TO_READONLY = [ 'leCpf', 'lePass', 'lePassConfirm', 'checkBox', 'leName', 'btnContinue', 'btnSecondary' ]

    def __init__(self, initial_state=State.READY_TO_SYNC, parent:QWidget=None):
        super().__init__(parent)
        self._blocked_wids = []
        self._state = None
        self._model = AppModel.instance()

        self._ui = Ui_SyncForm()
        self._ui.setupUi(self)
        self._ui.lbMessage.hide()

        self.setState(initial_state)

        # conectando eventos
        self._ui.lePass.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.lePassConfirm.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.leName.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.leCpf.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.btnContinue.clicked.connect(self.on_btnContinue_clicked)
        self._ui.btnSecondary.clicked.connect(self.on_btnSecondary_clicked)
        self._ui.checkBox.checkStateChanged.connect(self.on_checkBox_checkStateChanged)

    def setUser(self, data:UserDTO):
        self._ui.leCpf.setText(data.cpf)
        self._ui.leName.setText(data.name)

    def setState(self, state:State):
        if self._state == state:
            return
        
        self._state = state

        match state:
            case self._state.AUTH:
                self._ui.checkBox.hide()
                self._ui.lbCpf.show()
                self._ui.leCpf.show()
                self._ui.lbPass.show()
                self._ui.lePass.show()
                self._ui.lbPassConfirm.hide()
                self._ui.lePassConfirm.hide()
                self._ui.lbName.hide()
                self._ui.leName.hide()
                self._ui.widBtns.show()
                self._ui.btnSecondary.show()
                self._ui.btnContinue.show()

                self.showMessage('')
                self._ui.lbTitle.setText('Autenticação do Usuário')
                self._ui.btnSecondary.setText('novo usuário')
                self.secondaryState = self._state.NEW_USER

            case self._state.NEW_USER:
                self._ui.checkBox.hide()
                self._ui.lbCpf.show()
                self._ui.leCpf.show()
                self._ui.lbPass.show()
                self._ui.lePass.show()
                self._ui.lbPassConfirm.show()
                self._ui.lePassConfirm.show()
                self._ui.lbName.show()
                self._ui.leName.show()
                self._ui.widBtns.show()
                self._ui.btnSecondary.show()
                self._ui.btnContinue.show()

                self.showMessage('')
                self._ui.lbTitle.setText('Cadastro de Usuário')
                self._ui.btnSecondary.setText('tela de login')
                self.secondaryState = self._state.AUTH

            case self._state.MACHINE_OWNER:
                self.setUser(self._model.authenticatedUser())

                self._ui.checkBox.show()
                self._ui.lbCpf.show()
                self._ui.leCpf.show()
                self._ui.lbPass.hide()
                self._ui.lePass.hide()
                self._ui.lbPassConfirm.hide()
                self._ui.lePassConfirm.hide()
                self._ui.lbName.show()
                self._ui.leName.show()
                self._ui.widBtns.show()
                self._ui.btnSecondary.show()
                self._ui.btnContinue.show()

                self.showMessage('')
                self._ui.lbTitle.setText('Dados do Proprietário')
                self._ui.btnSecondary.setText('tela de login')
                self._ui.leCpf.setEnabled(False)
                self._ui.leName.setEnabled(False)
                self._ui.checkBox.setChecked(False)
                self.secondaryState = self._state.AUTH

            case self._state.READY_TO_SYNC:
                self._ui.checkBox.hide()
                self._ui.lbCpf.show()
                self._ui.leCpf.show()
                self._ui.lbPass.hide()
                self._ui.lePass.hide()
                self._ui.lbPassConfirm.hide()
                self._ui.lePassConfirm.hide()
                self._ui.lbName.show()
                self._ui.leName.show()
                self._ui.widBtns.show()
                self._ui.btnSecondary.show()
                self._ui.btnContinue.show()

                self.showMessage('')
                self._ui.lbTitle.setText('Dados para Sincronização')
                self._ui.leCpf.setEnabled(False)
                self._ui.leName.setEnabled(False)
                self._ui.btnSecondary.setText('voltar')
                self.secondaryState = self._state.MACHINE_OWNER

    def showMessage(self, text:str, timeout:int=None):
        self._ui.lbMessage.setText(text)
        self._ui.lbMessage.show()

        if timeout is not None and timeout > 0:
            QTimer.singleShot(timeout, self, self._ui.lbMessage.hide)

    def blockChanges(self, block=True):
        if block:
            for widName in self._WIDGETS_TO_READONLY:
                wid = getattr(self._ui, widName)
                if not wid.isReadOnly():
                    wid.setReadOnly(True)
                    self._blocked_wids.append(wid)
            
        else:
            for wid in self._blocked_wids:
                wid.setReadOnly(False)

            self._blocked_wids.clear()
            
    #------------------------------------------------------------------------
    # Eventos
    def on_btnSecondary_clicked(self):
        self.setState(self.secondaryState)

    def on_checkBox_checkStateChanged(self, state:Qt.CheckState):
        checked = state == Qt.CheckState.Checked

        match self._state:
            case self._state.MACHINE_OWNER:
                self._ui.leCpf.setEnabled(checked)
                self._ui.lbName.setHidden(checked)
                self._ui.leName.setHidden(checked)

    def on_btnContinue_clicked(self):
        self.showMessage('')

        match self._state:
            
            # gerar token com dados de login
            case self._state.AUTH:
                cpf = self._ui.leCpf.text()
                password = self._ui.lePass.text()

                if '' in (cpf, password):
                    self.showMessage('Preencha todos os campos para prosseguir!')
                    return

                self.authenticationRequired.emit(cpf, password)

            # gerar novo usuário
            case self._state.NEW_USER:
                cpf = self._ui.leCpf.text()
                password = self._ui.lePass.text()
                passwordConfirm = self._ui.lePassConfirm.text()
                name = self._ui.leName.text()

                if '' in (cpf, password, passwordConfirm, name):
                    self.showMessage('Preencha todos os campos para prosseguir!')
                    return
                
                if password != passwordConfirm:
                    self.showMessage('As senhas são diferentes!')
                    return
                
                if len(password) < 4:
                    self.showMessage('A senha deve conter pelo menos 4 caracteres!')
                    return
                
                self.newUserRequired.emit(NewUserDTO(cpf, password, name))
                
            # verificar se é para outro usuário e requisitar os dados do usuário desejado
            case self._state.MACHINE_OWNER:                
                if not self._ui.checkBox.isChecked():
                    self.setState(self._state.READY_TO_SYNC)
                    return
                
                owner = self._model.getUser(self._ui.leCpf.text())
                if not owner:
                    self.showMessage(self._model.getLastError())
                    return
                
                self.setUser(owner)
                self.setState(self._state.READY_TO_SYNC)

            # enviar dados da máquina
            case self._state.READY_TO_SYNC:
                self._ui.widBtns.hide()
                self.showMessage('máquina sincronizada com êxito!')

    #------------------------------------------------------------------------
