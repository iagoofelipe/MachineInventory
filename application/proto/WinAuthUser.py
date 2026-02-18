from PySide6.QtWidgets import QMainWindow, QApplication
from PySide6.QtCore import Qt

from autofiles.Ui_WinAuthUser import Ui_WinAuthUser

class WinAuthUser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_WinAuthUser()
        self.state = 0

        self.ui.setupUi(self)

        self.ui.lbName.hide()
        self.ui.leName.hide()
        self.ui.lbPassConfirm.hide()
        self.ui.lePassConfirm.hide()
        self.ui.leName.setText('Usuário 1')

        self.ui.checkBox.checkStateChanged.connect(self.on_checkBox_checkStateChanged)
        self.ui.leCpf.textChanged.connect(self.on_leCpf_textChanged)
        self.ui.btnContinue.clicked.connect(self.on_btnContinue_clicked)
        self.ui.btnBack.clicked.connect(self.on_btnBack_clicked)

    def on_checkBox_checkStateChanged(self, state:Qt.CheckState):
        if state == Qt.CheckState.Checked:
            self.ui.leCpf.textChanged.disconnect(self.on_leCpf_textChanged)

        if state == Qt.CheckState.Unchecked:
            self.ui.leCpfOwner.setText(self.ui.leCpf.text())
            self.ui.leCpf.textChanged.connect(self.on_leCpf_textChanged)

    def on_leCpf_textChanged(self, text:str):
        self.ui.leCpfOwner.setText(text)

    def on_btnContinue_clicked(self):
        # gerar token com dados de login
        if self.state == 0:
            self.ui.lbMessage.setText('requisição feita, token gerado')
            self.state = 1
            self.ui.lbPass.hide()
            self.ui.lePass.hide()
            self.ui.lbCpf.hide()
            self.ui.leCpf.hide()
            self.ui.checkBox.hide()
            self.ui.lbName.show()
            self.ui.leName.show()
            self.ui.btnBack.show()
            if self.ui.leCpfOwner.isEnabled(): self.ui.leCpfOwner.setEnabled(False)

        elif self.state == 1:
            self.state = 2
            self.ui.lbMessage.setText('máquina sincronizada com êxito!')
            self.ui.widBtns.hide()

    def on_btnBack_clicked(self):
        # gerar token com dados de login
        if self.state == 1:
            self.state = 0
            self.ui.lbPass.show()
            self.ui.lePass.show()
            self.ui.lbCpf.show()
            self.ui.leCpf.show()
            self.ui.checkBox.show()
            self.ui.lbName.hide()
            self.ui.leName.hide()
            self.ui.btnBack.hide()
            self.ui.lbMessage.setText('')
            if self.ui.checkBox.isChecked(): self.ui.leCpfOwner.setEnabled(True)

        

if __name__ == '__main__':
    app = QApplication()
    win = WinAuthUser()
    
    win.show()
    app.exec()