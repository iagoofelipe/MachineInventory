# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'WinAuthUser.ui'
##
## Created by: Qt User Interface Compiler version 6.10.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QHBoxLayout, QLabel,
    QLineEdit, QMainWindow, QPushButton, QSizePolicy,
    QSpacerItem, QTabWidget, QVBoxLayout, QWidget)

class Ui_WinAuthUser(object):
    def setupUi(self, WinAuthUser):
        if not WinAuthUser.objectName():
            WinAuthUser.setObjectName(u"WinAuthUser")
        WinAuthUser.resize(415, 496)
        font = QFont()
        font.setFamilies([u"Segoe UI"])
        font.setPointSize(11)
        WinAuthUser.setFont(font)
        self.centralwidget = QWidget(WinAuthUser)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(6, 6, 6, 6)
        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tabSync = QWidget()
        self.tabSync.setObjectName(u"tabSync")
        self.verticalLayout_2 = QVBoxLayout(self.tabSync)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.lbTitle = QLabel(self.tabSync)
        self.lbTitle.setObjectName(u"lbTitle")

        self.verticalLayout_2.addWidget(self.lbTitle)

        self.checkBox = QCheckBox(self.tabSync)
        self.checkBox.setObjectName(u"checkBox")

        self.verticalLayout_2.addWidget(self.checkBox)

        self.lbCpf = QLabel(self.tabSync)
        self.lbCpf.setObjectName(u"lbCpf")

        self.verticalLayout_2.addWidget(self.lbCpf)

        self.leCpf = QLineEdit(self.tabSync)
        self.leCpf.setObjectName(u"leCpf")

        self.verticalLayout_2.addWidget(self.leCpf)

        self.lbPass = QLabel(self.tabSync)
        self.lbPass.setObjectName(u"lbPass")

        self.verticalLayout_2.addWidget(self.lbPass)

        self.lePass = QLineEdit(self.tabSync)
        self.lePass.setObjectName(u"lePass")
        self.lePass.setEchoMode(QLineEdit.Password)

        self.verticalLayout_2.addWidget(self.lePass)

        self.lbPassConfirm = QLabel(self.tabSync)
        self.lbPassConfirm.setObjectName(u"lbPassConfirm")

        self.verticalLayout_2.addWidget(self.lbPassConfirm)

        self.lePassConfirm = QLineEdit(self.tabSync)
        self.lePassConfirm.setObjectName(u"lePassConfirm")

        self.verticalLayout_2.addWidget(self.lePassConfirm)

        self.lbName = QLabel(self.tabSync)
        self.lbName.setObjectName(u"lbName")

        self.verticalLayout_2.addWidget(self.lbName)

        self.leName = QLineEdit(self.tabSync)
        self.leName.setObjectName(u"leName")
        self.leName.setEnabled(True)

        self.verticalLayout_2.addWidget(self.leName)

        self.verticalSpacer = QSpacerItem(20, 95, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_2.addItem(self.verticalSpacer)

        self.lbMessage = QLabel(self.tabSync)
        self.lbMessage.setObjectName(u"lbMessage")
        font1 = QFont()
        font1.setPointSize(9)
        self.lbMessage.setFont(font1)

        self.verticalLayout_2.addWidget(self.lbMessage)

        self.widBtns = QWidget(self.tabSync)
        self.widBtns.setObjectName(u"widBtns")
        self.horizontalLayout = QHBoxLayout(self.widBtns)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer = QSpacerItem(313, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.btnSecondary = QPushButton(self.widBtns)
        self.btnSecondary.setObjectName(u"btnSecondary")

        self.horizontalLayout.addWidget(self.btnSecondary)

        self.btnContinue = QPushButton(self.widBtns)
        self.btnContinue.setObjectName(u"btnContinue")

        self.horizontalLayout.addWidget(self.btnContinue)


        self.verticalLayout_2.addWidget(self.widBtns)

        self.tabWidget.addTab(self.tabSync, "")
        self.tabMachine = QWidget()
        self.tabMachine.setObjectName(u"tabMachine")
        self.tabWidget.addTab(self.tabMachine, "")

        self.verticalLayout.addWidget(self.tabWidget)

        WinAuthUser.setCentralWidget(self.centralwidget)

        self.retranslateUi(WinAuthUser)

        self.tabWidget.setCurrentIndex(1)


        QMetaObject.connectSlotsByName(WinAuthUser)
    # setupUi

    def retranslateUi(self, WinAuthUser):
        WinAuthUser.setWindowTitle(QCoreApplication.translate("WinAuthUser", u"MainWindow", None))
        self.lbTitle.setText(QCoreApplication.translate("WinAuthUser", u"TITLE", None))
        self.checkBox.setText(QCoreApplication.translate("WinAuthUser", u"Cadastrar m\u00e1quina de outro usu\u00e1rio", None))
        self.lbCpf.setText(QCoreApplication.translate("WinAuthUser", u"CPF", None))
        self.lbPass.setText(QCoreApplication.translate("WinAuthUser", u"Senha", None))
        self.lbPassConfirm.setText(QCoreApplication.translate("WinAuthUser", u"Confirmar Senha", None))
        self.lbName.setText(QCoreApplication.translate("WinAuthUser", u"Nome", None))
        self.lbMessage.setText(QCoreApplication.translate("WinAuthUser", u"MESSAGE", None))
        self.btnSecondary.setText(QCoreApplication.translate("WinAuthUser", u"secondary", None))
        self.btnContinue.setText(QCoreApplication.translate("WinAuthUser", u"continuar", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabSync), QCoreApplication.translate("WinAuthUser", u"Sincronizar", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabMachine), QCoreApplication.translate("WinAuthUser", u"M\u00e1quina", None))
    # retranslateUi

