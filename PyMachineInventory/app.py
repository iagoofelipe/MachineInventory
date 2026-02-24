from PySide6.QtWidgets import QApplication
import sys

from controllers.AppController import AppController

class MachineInventoryApp(QApplication):
    def __init__(self):
        super().__init__()

    def exec(self):
        controller = AppController.instance()
        controller.initialize()
        return super().exec()
    
if __name__ == '__main__':
    app = MachineInventoryApp()
    sys.exit(app.exec())