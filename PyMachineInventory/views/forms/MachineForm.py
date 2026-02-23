from PySide6.QtWidgets import QWidget, QSplitter, QTableWidget, QTreeWidget, QTreeWidgetItem, QVBoxLayout, QTableWidgetItem
from PySide6.QtCore import Qt

from models.AppModel import AppModel

class MachineForm(QWidget):
    def __init__(self, parent:QWidget=None):
        super().__init__(parent)
        self._model = AppModel.instance()
        self._setupUi()
    
    def _setupUi(self):
        layout = QVBoxLayout(self)
        splitter = QSplitter(Qt.Horizontal)

        # árvore esquerda
        tree = QTreeWidget()
        tree.setHeaderLabel("Categorias")
        splitter.addWidget(tree)
        
        root = QTreeWidgetItem(tree, ["Resumo do Sistema"])
        components = QTreeWidgetItem(root, ["Componentes"])
        QTreeWidgetItem(root, ["Programas"])
        QTreeWidgetItem(components, ["Rede"])
        QTreeWidgetItem(components, ["Armazenamento"])
        QTreeWidgetItem(components, ["Memória RAM"])
        
        # tabela direita
        self._table = QTableWidget(0, 2)
        self._table.setHorizontalHeaderLabels(["Item", "Valor"])
        self._table.horizontalHeader().setStretchLastSection(True)
        self._table.verticalHeader().hide()
        self._table.setEditTriggers(QTableWidget.NoEditTriggers)
        
        splitter.addWidget(self._table)
        layout.addWidget(splitter)

        # eventos
        tree.itemClicked.connect(self.on_tree_itemClicked)

        tree.expandAll()
        tree.setCurrentItem(root)
        self.on_tree_itemClicked(root, 0)

        splitter.setSizes([300, 600])

    def on_tree_itemClicked(self, item:QTreeWidgetItem, column:int):
        # Limpa a tabela atual
        self._table.setRowCount(0)
        category = item.text(0)
        machine_data = self._model.machine()

        if not machine_data:
            return

        if category == "Resumo do Sistema":
            for i in range(10):
                self._table.insertRow(i)

            self._table.setItem(0, 0, QTableWidgetItem("Nome do Sistema Operacional"))
            self._table.setItem(0, 1, QTableWidgetItem(machine_data.os))
            self._table.setItem(1, 0, QTableWidgetItem("Versão"))
            self._table.setItem(1, 1, QTableWidgetItem(machine_data.osVersion))
            self._table.setItem(2, 0, QTableWidgetItem("Arquitetura"))
            self._table.setItem(2, 1, QTableWidgetItem(machine_data.osArchitecture))
            self._table.setItem(3, 0, QTableWidgetItem("Data de Instalação"))
            self._table.setItem(3, 1, QTableWidgetItem(str(machine_data.osInstallDate)))
            self._table.setItem(4, 0, QTableWidgetItem("ID do Sistema Operacional"))
            self._table.setItem(4, 1, QTableWidgetItem(machine_data.osSerialNumber))
            self._table.setItem(5, 0, QTableWidgetItem("Organização"))
            self._table.setItem(5, 1, QTableWidgetItem(machine_data.osOrganization))
            self._table.setItem(6, 0, QTableWidgetItem("Placa Mãe"))
            self._table.setItem(6, 1, QTableWidgetItem(machine_data.motherboard))
            self._table.setItem(7, 0, QTableWidgetItem("Provedor da Placa Mãe"))
            self._table.setItem(7, 1, QTableWidgetItem(machine_data.motherboardManufacturer))
            self._table.setItem(8, 0, QTableWidgetItem("Processador"))
            self._table.setItem(8, 1, QTableWidgetItem(machine_data.processor))
            self._table.setItem(9, 0, QTableWidgetItem("Velocidade de Clock do Processador"))
            self._table.setItem(9, 1, QTableWidgetItem(f"{machine_data.processorClockSpeed / 1000:.2f} GHz"))
        
        elif category == "Armazenamento":
            data = machine_data.disks
            last_index = len(data) - 1
            row = -1

            for i, disk in enumerate(data):
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Nome"))
                self._table.setItem(row, 1, QTableWidgetItem(disk.name))
                
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Número de Série"))
                self._table.setItem(row, 1, QTableWidgetItem(disk.seriaNumber))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Espaço"))
                self._table.setItem(row, 1, QTableWidgetItem(f"{int(disk.size) // pow(10, 9)} GB"))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Modelo"))
                self._table.setItem(row, 1, QTableWidgetItem(disk.model))

                if i != last_index:
                    row+=1
                    self._table.insertRow(row)

        elif category == "Rede":
            data = machine_data.networkAdapters
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Nome"))
                self._table.setItem(row, 1, QTableWidgetItem(d.name))
                
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Mac"))
                self._table.setItem(row, 1, QTableWidgetItem(d.mac))

                if i != last_index:
                    row+=1
                    self._table.insertRow(row)

        elif category == "Memória RAM":
            data = machine_data.physicalMemories
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Nome"))
                self._table.setItem(row, 1, QTableWidgetItem(d.name))
                
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Capacidade"))
                self._table.setItem(row, 1, QTableWidgetItem(f"{int(d.capacity) // pow(10, 9)} GB"))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Velocidade"))
                self._table.setItem(row, 1, QTableWidgetItem(f"{d.speed} MT/s"))

                if i != last_index:
                    row+=1
                    self._table.insertRow(row)

        elif category == "Programas":
            data = machine_data.programs
            last_index = len(data) - 1
            row = -1

            for i, d in enumerate(data):
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Nome"))
                self._table.setItem(row, 1, QTableWidgetItem(d.name))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Versão"))
                self._table.setItem(row, 1, QTableWidgetItem(d.version))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Provedor"))
                self._table.setItem(row, 1, QTableWidgetItem(d.publisher))
                
                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Espaço"))
                self._table.setItem(row, 1, QTableWidgetItem(f"{int(d.estimatedSize) // pow(10, 3)} MB"))

                row+=1
                self._table.insertRow(row)
                self._table.setItem(row, 0, QTableWidgetItem("Apenas no Usuário Atual"))
                self._table.setItem(row, 1, QTableWidgetItem(str(d.currentUserOnly)))

                if i != last_index:
                    row+=1
                    self._table.insertRow(row)