from PySide6.QtCore import QObject, Signal, Qt
from PySide6.QtWidgets import QMainWindow, QTabWidget, QWidget, QVBoxLayout, QLabel
from PySide6.QtGui import QFont
from enum import Enum, auto

from views.forms.SyncForm import SyncForm
from views.forms.MachineForm import MachineForm
from views.forms.AuthForm import AuthForm

class AppView(QObject):
    _instance = None # instanciado por MachineInventoryApp

    uiChanged = Signal(Enum)

    class UI(Enum):
        LOADING = auto()
        MAIN = auto()
        AUTH = auto()

    def __init__(self):
        super().__init__()
        self._syncForm = None
        self._machineForm = None
        self._authForm = None
        self._currentUi = None

        self._win = QMainWindow()
        self._win.setFont(QFont('Segoe UI', 11))
        self._win.setMinimumSize(900, 600)

        self.setUi(self.UI.LOADING)

    #-----------------------------------------------------------------------
    # Métodos Públicos
    def initialize(self):
        self._win.show()
    
    @classmethod
    def instance(cls) -> 'AppView':
        return cls._instance
    
    def syncForm(self): return self._syncForm if self._currentUi == self.UI.MAIN else None
    def machineForm(self): return self._machineForm if self._currentUi == self.UI.MAIN else None
    def authForm(self): return self._authForm if self._currentUi == self.UI.AUTH else None
    
    def setUi(self, ui:UI, *args, **kwargs):
        if self._currentUi == ui and ui:
            return
        
        match ui:
            case self.UI.MAIN:
                self._wid = QWidget()
                self._layout = QVBoxLayout(self._wid)
                
                self._tabWid = QTabWidget(self._wid)
                self._layout.addWidget(self._tabWid)

                self._syncForm = SyncForm(parent=self._tabWid, *args, **kwargs)
                self._tabWid.addTab(self._syncForm, 'Sincronizar')

                self._machineForm = MachineForm(parent=self._tabWid)
                self._tabWid.addTab(self._machineForm, 'Máquina')
            
            case self.UI.LOADING:
                self._wid = QLabel('inicializando componentes...', alignment=Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignHCenter)
            
            case self.UI.AUTH:
                self._wid = self._authForm = AuthForm()

            case _:
                raise RuntimeError(f'invalid argument {ui}')

        self._currentUi = ui
        self._win.setCentralWidget(self._wid)
        self.uiChanged.emit(ui)
        
    #-----------------------------------------------------------------------