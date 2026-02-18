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
    QSpacerItem, QVBoxLayout, QWidget)

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
        self.checkBox = QCheckBox(self.centralwidget)
        self.checkBox.setObjectName(u"checkBox")

        self.verticalLayout.addWidget(self.checkBox)

        self.lbCpf = QLabel(self.centralwidget)
        self.lbCpf.setObjectName(u"lbCpf")

        self.verticalLayout.addWidget(self.lbCpf)

        self.leCpf = QLineEdit(self.centralwidget)
        self.leCpf.setObjectName(u"leCpf")

        self.verticalLayout.addWidget(self.leCpf)

        self.lbPass = QLabel(self.centralwidget)
        self.lbPass.setObjectName(u"lbPass")

        self.verticalLayout.addWidget(self.lbPass)

        self.lePass = QLineEdit(self.centralwidget)
        self.lePass.setObjectName(u"lePass")
        self.lePass.setEchoMode(QLineEdit.Password)

        self.verticalLayout.addWidget(self.lePass)

        self.lbPassConfirm = QLabel(self.centralwidget)
        self.lbPassConfirm.setObjectName(u"lbPassConfirm")

        self.verticalLayout.addWidget(self.lbPassConfirm)

        self.lePassConfirm = QLineEdit(self.centralwidget)
        self.lePassConfirm.setObjectName(u"lePassConfirm")

        self.verticalLayout.addWidget(self.lePassConfirm)

        self.lbCpfOwner = QLabel(self.centralwidget)
        self.lbCpfOwner.setObjectName(u"lbCpfOwner")

        self.verticalLayout.addWidget(self.lbCpfOwner)

        self.leCpfOwner = QLineEdit(self.centralwidget)
        self.leCpfOwner.setObjectName(u"leCpfOwner")
        self.leCpfOwner.setEnabled(False)

        self.verticalLayout.addWidget(self.leCpfOwner)

        self.lbName = QLabel(self.centralwidget)
        self.lbName.setObjectName(u"lbName")

        self.verticalLayout.addWidget(self.lbName)

        self.leName = QLineEdit(self.centralwidget)
        self.leName.setObjectName(u"leName")
        self.leName.setEnabled(False)

        self.verticalLayout.addWidget(self.leName)

        self.verticalSpacer = QSpacerItem(20, 95, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.lbMessage = QLabel(self.centralwidget)
        self.lbMessage.setObjectName(u"lbMessage")

        self.verticalLayout.addWidget(self.lbMessage)

        self.widBtns = QWidget(self.centralwidget)
        self.widBtns.setObjectName(u"widBtns")
        self.horizontalLayout = QHBoxLayout(self.widBtns)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer = QSpacerItem(313, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.btnBack = QPushButton(self.widBtns)
        self.btnBack.setObjectName(u"btnBack")

        self.horizontalLayout.addWidget(self.btnBack)

        self.btnContinue = QPushButton(self.widBtns)
        self.btnContinue.setObjectName(u"btnContinue")

        self.horizontalLayout.addWidget(self.btnContinue)


        self.verticalLayout.addWidget(self.widBtns)

        WinAuthUser.setCentralWidget(self.centralwidget)

        self.retranslateUi(WinAuthUser)
        self.checkBox.clicked["bool"].connect(self.leCpfOwner.setEnabled)

        QMetaObject.connectSlotsByName(WinAuthUser)
    # setupUi

    def retranslateUi(self, WinAuthUser):
        WinAuthUser.setWindowTitle(QCoreApplication.translate("WinAuthUser", u"MainWindow", None))
        self.checkBox.setText(QCoreApplication.translate("WinAuthUser", u"Cadastrar m\u00e1quina de outro usu\u00e1rio", None))
        self.lbCpf.setText(QCoreApplication.translate("WinAuthUser", u"CPF", None))
        self.lbPass.setText(QCoreApplication.translate("WinAuthUser", u"Senha", None))
        self.lbPassConfirm.setText(QCoreApplication.translate("WinAuthUser", u"Confirmar Senha", None))
        self.lbCpfOwner.setText(QCoreApplication.translate("WinAuthUser", u"CPF Propriet\u00e1rio", None))
        self.lbName.setText(QCoreApplication.translate("WinAuthUser", u"Nome", None))
        self.lbMessage.setText("")
        self.btnBack.setText(QCoreApplication.translate("WinAuthUser", u"voltar", None))
        self.btnContinue.setText(QCoreApplication.translate("WinAuthUser", u"continuar", None))
    # retranslateUi

