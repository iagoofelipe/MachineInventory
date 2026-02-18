from PySide6.QtWidgets import QMainWindow, QApplication, QSplitter, QTableWidget, QTreeWidget, QTreeWidgetItem, QHeaderView, QVBoxLayout, QTableWidgetItem
from PySide6.QtCore import Qt
from enum import Enum, auto
import json

from autofiles.Ui_WinAuthUser import Ui_WinAuthUser
from ServerConnection import ServerConnection

with open("machine.json") as f:
    SYSTEM_DATA = json.load(f)

class WinAuthUser(QMainWindow):
    class State(Enum):
        AUTH = auto()
        NEW_USER = auto()
        MACHINE_OWNER = auto()
        READY_TO_SYNC = auto()

    def __init__(self):
        super().__init__()
        self.owner = None
        self.ui = Ui_WinAuthUser()
        self.ui.setupUi(self)
        self.setupMachineUi()
        self.resize(800, 500)

        self.ui.checkBox.checkStateChanged.connect(self.on_checkBox_checkStateChanged)
        self.ui.btnContinue.clicked.connect(self.on_btnContinue_clicked)
        self.ui.btnSecondary.clicked.connect(self.on_btnSecondary_clicked)

        # verificar se há token presente e válido
        self.server = ServerConnection()
        if not self.server.hasToken():
            self.setState(self.State.AUTH)
            return
        
        self.owner = self.server.getUser()
        self.ui.leCpf.setText(self.owner['cpf'])
        self.ui.leName.setText(self.owner['name'])
        self.setState(self.State.MACHINE_OWNER)

    
    def setupMachineUi(self):
        self.layoutTabMachine = QVBoxLayout(self.ui.tabMachine)

        # O Splitter permite redimensionar os painéis lateral e principal
        splitter = QSplitter(Qt.Horizontal)

        # 1. Painel da Esquerda: Árvore (Hierarquia)
        self.tree = QTreeWidget()
        self.tree.setHeaderLabel("Categorias")
        
        # Criando a estrutura de pastas
        root = QTreeWidgetItem(self.tree, ["Resumo do Sistema"])
        components = QTreeWidgetItem(root, ["Componentes"])
        programs = QTreeWidgetItem(root, ["Programas"])
        QTreeWidgetItem(components, ["Rede"])
        QTreeWidgetItem(components, ["Armazenamento"])
        QTreeWidgetItem(components, ["Memória RAM"])
        
        self.tree.expandAll()
        splitter.addWidget(self.tree)

        # 2. Painel da Direita: Tabela (Dados)
        self.table = QTableWidget(0, 2)
        self.table.setHorizontalHeaderLabels(["Item", "Valor"])
        self.table.verticalHeader().hide()
        self.table.horizontalHeader().setStretchLastSection(True)
        
        # Bloqueia a edição direta na tabela
        self.table.setEditTriggers(QTableWidget.NoEditTriggers)
        
        splitter.addWidget(self.table)
        self.layoutTabMachine.addWidget(splitter)

        # Conectar evento de clique na árvore
        self.tree.itemClicked.connect(self.update_table)

        self.tree.setCurrentItem(root)
        self.update_table(root, None)

    def update_table(self, item, column):
        # Limpa a tabela atual
        self.table.setRowCount(0)
        category = item.text(0)

        if category == "Resumo do Sistema":
            for i in range(10):
                self.table.insertRow(i)

            self.table.setItem(0, 0, QTableWidgetItem("Nome do Sistema Operacional"))
            self.table.setItem(0, 1, QTableWidgetItem(SYSTEM_DATA["os"]))
            self.table.setItem(1, 0, QTableWidgetItem("Versão"))
            self.table.setItem(1, 1, QTableWidgetItem(SYSTEM_DATA["osVersion"]))
            self.table.setItem(2, 0, QTableWidgetItem("Arquitetura"))
            self.table.setItem(2, 1, QTableWidgetItem(SYSTEM_DATA["osArchitecture"]))
            self.table.setItem(3, 0, QTableWidgetItem("Data de Instalação"))
            self.table.setItem(3, 1, QTableWidgetItem(SYSTEM_DATA["osInstallDate"]))
            self.table.setItem(4, 0, QTableWidgetItem("ID do Sistema Operacional"))
            self.table.setItem(4, 1, QTableWidgetItem(SYSTEM_DATA["osSerialNumber"]))
            self.table.setItem(5, 0, QTableWidgetItem("Organização"))
            self.table.setItem(5, 1, QTableWidgetItem(SYSTEM_DATA["osOrganization"]))
            self.table.setItem(6, 0, QTableWidgetItem("Placa Mãe"))
            self.table.setItem(6, 1, QTableWidgetItem(SYSTEM_DATA["motherboard"]))
            self.table.setItem(7, 0, QTableWidgetItem("Provedor da Placa Mãe"))
            self.table.setItem(7, 1, QTableWidgetItem(SYSTEM_DATA["motherboardManufacturer"]))
            self.table.setItem(8, 0, QTableWidgetItem("Processador"))
            self.table.setItem(8, 1, QTableWidgetItem(SYSTEM_DATA["processor"]))
            self.table.setItem(9, 0, QTableWidgetItem("Velocidade de Clock do Processador"))
            self.table.setItem(9, 1, QTableWidgetItem(f"{SYSTEM_DATA["processorClockSpeed"] / 1000:.2f} GHz"))
        
        elif category == "Armazenamento":
            data = SYSTEM_DATA["disks"]
            last_index = len(data) - 1
            row = -1

            for i, disk in enumerate(data):
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Nome"))
                self.table.setItem(row, 1, QTableWidgetItem(disk['name']))
                
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Número de Série"))
                self.table.setItem(row, 1, QTableWidgetItem(disk['seriaNumber']))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Espaço"))
                self.table.setItem(row, 1, QTableWidgetItem(f"{int(disk['size']) // pow(10, 9)} GB"))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Modelo"))
                self.table.setItem(row, 1, QTableWidgetItem(disk['model']))

                if i != last_index:
                    row+=1
                    self.table.insertRow(row)

        elif category == "Rede":
            data = SYSTEM_DATA["networkAdapters"]
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Nome"))
                self.table.setItem(row, 1, QTableWidgetItem(d['name']))
                
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Mac"))
                self.table.setItem(row, 1, QTableWidgetItem(d['mac']))

                if i != last_index:
                    row+=1
                    self.table.insertRow(row)

        elif category == "Memória RAM":
            data = SYSTEM_DATA["physicalMemories"]
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Nome"))
                self.table.setItem(row, 1, QTableWidgetItem(d['name']))
                
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Capacidade"))
                self.table.setItem(row, 1, QTableWidgetItem(f"{int(d['capacity']) // pow(10, 9)} GB"))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Velocidade"))
                self.table.setItem(row, 1, QTableWidgetItem(f"{d['speed']} MT/s"))

                if i != last_index:
                    row+=1
                    self.table.insertRow(row)

        elif category == "Programas":
            data = SYSTEM_DATA["programs"]
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Nome"))
                self.table.setItem(row, 1, QTableWidgetItem(d['name']))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Versão"))
                self.table.setItem(row, 1, QTableWidgetItem(d['version']))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Provedor"))
                self.table.setItem(row, 1, QTableWidgetItem(d['publisher']))
                
                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Espaço"))
                self.table.setItem(row, 1, QTableWidgetItem(f"{int(d['estimatedSize']) // pow(10, 3)} MB"))

                row+=1
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem("Apenas no Usuário Atual"))
                self.table.setItem(row, 1, QTableWidgetItem(str(d['currentUserOnly'])))

                if i != last_index:
                    row+=1
                    self.table.insertRow(row)


    def setState(self, state:State):
        self.state = state

        match state:
            case self.State.AUTH:
                self.ui.checkBox.hide()
                self.ui.lbCpf.show()
                self.ui.leCpf.show()
                self.ui.lbPass.show()
                self.ui.lePass.show()
                self.ui.lbPassConfirm.hide()
                self.ui.lePassConfirm.hide()
                self.ui.lbName.hide()
                self.ui.leName.hide()
                self.ui.widBtns.show()
                self.ui.btnSecondary.show()
                self.ui.btnContinue.show()

                self.ui.lbMessage.setText('')
                self.ui.lbTitle.setText('Autenticação do Usuário')
                self.ui.btnSecondary.setText('novo usuário')
                self.secondaryState = self.State.NEW_USER

            case self.State.NEW_USER:
                self.ui.checkBox.hide()
                self.ui.lbCpf.show()
                self.ui.leCpf.show()
                self.ui.lbPass.show()
                self.ui.lePass.show()
                self.ui.lbPassConfirm.show()
                self.ui.lePassConfirm.show()
                self.ui.lbName.show()
                self.ui.leName.show()
                self.ui.widBtns.show()
                self.ui.btnSecondary.show()
                self.ui.btnContinue.show()

                self.ui.lbMessage.setText('')
                self.ui.lbTitle.setText('Cadastro de Usuário')
                self.ui.btnSecondary.setText('tela de login')
                self.secondaryState = self.State.AUTH

            case self.State.MACHINE_OWNER:
                self.ui.checkBox.show()
                self.ui.lbCpf.show()
                self.ui.leCpf.show()
                self.ui.lbPass.hide()
                self.ui.lePass.hide()
                self.ui.lbPassConfirm.hide()
                self.ui.lePassConfirm.hide()
                self.ui.lbName.show()
                self.ui.leName.show()
                self.ui.widBtns.show()
                self.ui.btnSecondary.show()
                self.ui.btnContinue.show()

                self.ui.lbMessage.setText('')
                self.ui.lbTitle.setText('Dados do Proprietário')
                self.ui.btnSecondary.setText('tela de login')
                self.ui.leCpf.setEnabled(False)
                self.ui.leName.setEnabled(False)
                self.ui.checkBox.setChecked(False)
                self.secondaryState = self.State.AUTH

            case self.State.READY_TO_SYNC:
                self.ui.checkBox.hide()
                self.ui.lbCpf.show()
                self.ui.leCpf.show()
                self.ui.lbPass.hide()
                self.ui.lePass.hide()
                self.ui.lbPassConfirm.hide()
                self.ui.lePassConfirm.hide()
                self.ui.lbName.show()
                self.ui.leName.show()
                self.ui.widBtns.show()
                self.ui.btnSecondary.show()
                self.ui.btnContinue.show()

                self.ui.lbMessage.setText('')
                self.ui.lbTitle.setText('Dados para Sincronização')
                self.ui.leCpf.setEnabled(False)
                self.ui.leName.setEnabled(False)
                self.ui.btnSecondary.setText('voltar')
                self.secondaryState = self.State.MACHINE_OWNER

    def on_btnSecondary_clicked(self):
        self.setState(self.secondaryState)

    def on_checkBox_checkStateChanged(self, state:Qt.CheckState):
        checked = state == Qt.CheckState.Checked

        match self.state:
            case self.State.MACHINE_OWNER:
                self.ui.leCpf.setEnabled(checked)
                self.ui.lbName.setHidden(checked)
                self.ui.leName.setHidden(checked)

    def on_btnContinue_clicked(self):
        self.ui.lbMessage.setText('')
        self.blockButtons(True)

        # gerar token com dados de login
        match self.state:
            case self.State.AUTH:
                # coletar usuário e senha para gerar o token
                if self.server.generateToken(self.ui.leCpf.text(), self.ui.lePass.text()):
                    self.owner = self.server.getUser()
                    self.ui.leName.setText(self.owner['name'])
                    self.setState(self.State.MACHINE_OWNER)

                else:
                    self.ui.lbMessage.setText(self.server.getLastError())
                

            # verificar se é para outro usuário e requisitar os dados do usuário desejado
            case self.State.MACHINE_OWNER:                
                if self.ui.checkBox.isChecked():
                    self.owner = self.server.getUser(self.ui.leCpf.text())

                    if self.owner:
                        self.ui.leCpf.setText(self.owner['cpf'])
                        self.ui.leName.setText(self.owner['name'])
                        self.setState(self.State.READY_TO_SYNC)

                    else:
                        self.ui.lbMessage.setText(self.server.getLastError())

                else:
                    self.setState(self.State.READY_TO_SYNC)

            # enviar dados da máquina
            case self.State.READY_TO_SYNC:
                self.ui.widBtns.hide()
                self.ui.lbMessage.setText('máquina sincronizada com êxito!')

        self.blockButtons(False)
        
    def blockButtons(self, arg:bool):
        self.ui.btnContinue.setDisabled(arg)
        self.ui.btnSecondary.setDisabled(arg)


if __name__ == '__main__':
    app = QApplication()
    win = WinAuthUser()
    
    win.show()
    app.exec()