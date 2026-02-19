from PySide6.QtWidgets import QApplication
import sys

from models.AppModel import AppModel
from views.AppView import AppView
from controllers.AppController import AppController

class MachineInventoryApp(QApplication):
    def __init__(self):
        super().__init__()
        
        # instanciando classes singleton
        AppModel._instance = AppModel()
        AppView._instance = AppView()
        AppController._instance = AppController()

    def exec(self):
        AppController.instance().initialize()
        return super().exec()
    
if __name__ == '__main__':
    app = MachineInventoryApp()
    sys.exit(app.exec())