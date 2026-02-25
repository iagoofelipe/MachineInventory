from PySide6.QtCore import QObject, Signal, Qt
from PySide6.QtWidgets import QMainWindow, QTabWidget, QWidget, QVBoxLayout, QLabel
from PySide6.QtGui import QFont, QAction, QKeySequence

from views.forms.SyncForm import SyncForm
from views.forms.MachineForm import MachineForm
from views.forms.AuthForm import AuthForm

class AppView(QObject):
    _instance = None

    uiChanged = Signal(int)
    logoutRequired = Signal()

    UI_LOADING = 1 << 1
    UI_AUTH = 1 << 2
    UI_SYNC = 1 << 3
    UI_MACHINE = 1 << 4
    UI_MAIN = UI_SYNC | UI_MACHINE
    _UIS = (UI_AUTH, UI_LOADING, UI_MACHINE, UI_MAIN, UI_SYNC)

    def __init__(self):
        super().__init__()
        self._syncForm = None
        self._machineForm = None
        self._authForm = None
        self._currentUi = None

        self._win = QMainWindow()
        self._win.setFont(QFont('Segoe UI', 11))
        self._win.setMinimumSize(900, 600)
        self._win.setWindowTitle('Machine Inventory')

        menubar = self._win.menuBar()
        menuFile = menubar.addMenu('&Arquivo')
        
        actionLogout = QAction('Sair', self._win)
        actionLogout.setStatusTip('Retorna para a tela de autenticação')
        actionLogout.triggered.connect(self.logoutRequired)

        actionQuit = QAction('Fechar Janela', self._win)
        actionQuit.setShortcut(QKeySequence.Quit)
        actionQuit.triggered.connect(self._win.close)
        
        menuFile.addAction(actionQuit)
        menuFile.addSeparator()
        menuFile.addAction(actionLogout)

        self.setUi(self.UI_LOADING)

    #-----------------------------------------------------------------------
    # Métodos Públicos
    def initialize(self):
        self._win.show()
    
    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance
    
    def syncForm(self): return self._syncForm if self._currentUi & self.UI_SYNC else None
    def machineForm(self): return self._machineForm if self._currentUi & self.UI_MACHINE else None
    def authForm(self): return self._authForm if self._currentUi & self.UI_AUTH else None
    
    def setUi(self, ui:int, *args, **kwargs):
        if ui not in self._UIS:
            raise RuntimeError(f'invalid UI {ui}')

        if self._currentUi == ui:
            return
        
        if ui & self.UI_SYNC:
            self._wid = self._syncForm = SyncForm(*args, **kwargs)
        
        if ui & self.UI_MACHINE:
            self._wid = self._machineForm = MachineForm()
        
        match ui:
            case self.UI_MAIN:
                self._wid = QWidget()
                self._layout = QVBoxLayout(self._wid)
                
                self._tabWid = QTabWidget(self._wid)
                self._layout.addWidget(self._tabWid)

                self._syncForm.setParent(self._tabWid)
                self._tabWid.addTab(self._syncForm, 'Sincronização')

                self._machineForm.setParent(self._tabWid)
                self._tabWid.addTab(self._machineForm, 'Máquina')

            case self.UI_LOADING:
                self._wid = self._loadingLabel = QLabel('inicializando componentes...', alignment=Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignHCenter)
                self._loadingLabel.destroyed.connect(self.on_loadingLabel_destroyed)

            case self.UI_AUTH:
                self._wid = self._authForm = AuthForm()

        self._win.menuBar().setVisible(ui not in (self.UI_LOADING, self.UI_AUTH))
        self._currentUi = ui
        self._win.setCentralWidget(self._wid)
        self.uiChanged.emit(ui)
    
    def setLoadingMessage(self, msg:str):
        if self._loadingLabel:
            self._loadingLabel.setText(msg)

    #-----------------------------------------------------------------------
    # Eventos
    def on_loadingLabel_destroyed(self):
        self._loadingLabel = None

    #-----------------------------------------------------------------------