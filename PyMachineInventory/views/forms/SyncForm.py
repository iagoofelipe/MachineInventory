from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal, Qt
from enum import Enum, auto

from src.ui.autofiles.Ui_SyncForm import Ui_SyncForm
from models.structs import User
from models.AppModel import AppModel
from .AbstractForm import AbstractForm

class SyncForm(AbstractForm):
    # AbstractForm Params
    WIDGETS_TO_BLOCK = [ 'leCpf', 'checkBox', 'leName', 'btnContinue', 'btnContinue' ]
    WIDGETS_TO_CLEAR = [ 'leCpf', 'leName', 'lbMessage' ]
    
    # Events
    syncRequired = Signal(User, str) # Owner User, machineTitle

    # SyncForm Params
    class State(Enum):
        MACHINE_OWNER = auto()
        READY_TO_SYNC = auto()

    def __init__(self, parent:QWidget=None):
        super().__init__(parent)
        self._state = None
        self._previousState = None
        self._model = AppModel.instance()
        self._user = self._model.authenticatedUser()
        self._userRuleAddMachine = 'ADD_MACHINE' in self._user.rules

        self._ui = Ui_SyncForm()
        self._ui.setupUi(self)
        self.LABEL_MESSAGE = self._ui.lbMessage

        # eventos
        self._ui.leName.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.leCpf.returnPressed.connect(self._ui.btnContinue.click)
        self._ui.btnContinue.clicked.connect(self.on_btnContinue_clicked)
        self._ui.btnBack.clicked.connect(self.on_btnBack_clicked)
        self._ui.checkBox.checkStateChanged.connect(self.on_checkBox_checkStateChanged)
        
        self.setState(self.State.MACHINE_OWNER if self._userRuleAddMachine else self.State.READY_TO_SYNC)

    #--------------------------------------------------------------------------
    # Métodos Públicos
    def setUser(self, data:User|None):
        self._user = data

        self._ui.leCpf.setText(data.cpf if data else '')
        self._ui.leCpf.setEnabled(False)

        self._ui.leName.setText(data.name if data else '')
        self._ui.leName.setEnabled(False)
        self._ui.lbName.show()
        self._ui.leName.show()

    def setState(self, state:State):
        self.blockChanges(False)
        self.clear()

        if self._state == state:
            return
        
        if state == self.State.MACHINE_OWNER and not self._userRuleAddMachine:
            raise RuntimeError("State.MACHINE_OWNER not allowed without user's rule ADD_MACHINE")
        
        self._state = state
        self._previousState = None
        self.setUser(self._user)

        match state:
            case self.State.MACHINE_OWNER:
                self._ui.lbTitle.setText('Dados do Proprietário')

                self._ui.checkBox.show()
                self._ui.btnBack.hide()
                self._ui.lbMachineTitle.hide()
                self._ui.leMachineTitle.hide()
                self._ui.checkBox.setChecked(False)

            case self.State.READY_TO_SYNC:
                self._ui.leMachineTitle.setText(self._model.machinePreviousTitle())
                self._ui.lbTitle.setText('Dados para Sincronização')
                self._previousState = self.State.MACHINE_OWNER

                self._ui.checkBox.hide()
                self._ui.lbMachineTitle.show()
                self._ui.leMachineTitle.show()
                self._ui.btnBack.setVisible(self._userRuleAddMachine)
            
    #------------------------------------------------------------------------
    # Eventos
    def on_checkBox_checkStateChanged(self, state:Qt.CheckState):
        checked = state == Qt.CheckState.Checked
        self.showMessage('')

        match self._state:
            case self.State.MACHINE_OWNER:
                self._ui.leCpf.setEnabled(checked)
                self._ui.lbName.setHidden(checked)
                self._ui.leName.setHidden(checked)

                if checked:
                    self._ui.leCpf.setText('')
                else:
                    self._user = self._model.authenticatedUser()
                    self._ui.leCpf.setText(self._user.cpf)
                    self._ui.leName.setText(self._user.name)

    def on_btnContinue_clicked(self):
        self.showMessage('')

        match self._state:
            
            # verificar se é para outro usuário e requisitar os dados do usuário desejado
            case self._state.MACHINE_OWNER:                
                if not self._ui.checkBox.isChecked():
                    self.setState(self._state.READY_TO_SYNC)
                    return
                
                owner = self._model.getUser(self._ui.leCpf.text())
                if not owner:
                    self.showMessage(self._model.getLastError())
                    return
                
                self._user = owner
                self.setState(self._state.READY_TO_SYNC)

            # enviar dados da máquina
            case self._state.READY_TO_SYNC:
                self.blockChanges()
                self.syncRequired.emit(self._user, self._ui.leMachineTitle.text())

    def on_btnBack_clicked(self):
        self.setState(self._previousState)

    #------------------------------------------------------------------------
