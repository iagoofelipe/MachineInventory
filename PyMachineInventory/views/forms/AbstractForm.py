from PySide6.QtWidgets import QWidget, QMessageBox, QLabel
from PySide6.QtCore import QTimer
from typing import Iterable

from tools.utils import set_unenabled

class AbstractForm(QWidget):
    WIDGETS_TO_BLOCK:Iterable[str] = []
    WIDGETS_TO_CLEAR:Iterable[str] = []
    LABEL_MESSAGE:QLabel = None

    def __init__(self, parent:QWidget=None):
        super().__init__(parent)
        self._unenabled = [] # widgets definidos com unenabled
        self._lbMessage = None

    def clear(self):
        ui = self._ui if hasattr(self, '_ui') else self
        for wid in map(lambda x: getattr(ui, x), self.WIDGETS_TO_CLEAR):
            wid.clear()

    def blockChanges(self, block=True):
        ui = self._ui if hasattr(self, '_ui') else self
        if block:
            self._unenabled = set_unenabled(map(lambda x: getattr(ui, x), self.WIDGETS_TO_BLOCK))
        else:
            self._unenabled = set_unenabled(self._unenabled, False)

    def showMessage(self, text:str, timeout:int=None):
        if self.LABEL_MESSAGE:
            self.LABEL_MESSAGE.setText(text)

            if timeout is not None and timeout > 0:
                QTimer.singleShot(timeout, self.LABEL_MESSAGE, self.LABEL_MESSAGE.clear)
        
        elif text:
            QMessageBox(QMessageBox.Icon.Information, 'Mensagem do sistema', text, parent=self).exec()