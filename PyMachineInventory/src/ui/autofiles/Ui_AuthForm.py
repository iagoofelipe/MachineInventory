# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'AuthForm.ui'
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
from PySide6.QtWidgets import (QApplication, QGridLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QWidget)

class Ui_AuthForm(object):
    def setupUi(self, AuthForm):
        if not AuthForm.objectName():
            AuthForm.setObjectName(u"AuthForm")
        AuthForm.resize(623, 496)
        self.gridLayout = QGridLayout(AuthForm)
        self.gridLayout.setObjectName(u"gridLayout")
        self.leCpf = QLineEdit(AuthForm)
        self.leCpf.setObjectName(u"leCpf")

        self.gridLayout.addWidget(self.leCpf, 3, 1, 1, 1)

        self.label = QLabel(AuthForm)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 2, 1, 1, 1)

        self.label_2 = QLabel(AuthForm)
        self.label_2.setObjectName(u"label_2")

        self.gridLayout.addWidget(self.label_2, 6, 1, 1, 1)

        self.label_3 = QLabel(AuthForm)
        self.label_3.setObjectName(u"label_3")
        font = QFont()
        font.setPointSize(15)
        font.setBold(False)
        self.label_3.setFont(font)
        self.label_3.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_3, 1, 1, 1, 1)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer_2, 0, 1, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 0, 2, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 16, 1, 1, 1)

        self.lePass = QLineEdit(AuthForm)
        self.lePass.setObjectName(u"lePass")
        self.lePass.setEnabled(True)
        self.lePass.setEchoMode(QLineEdit.Password)
        self.lePass.setReadOnly(False)

        self.gridLayout.addWidget(self.lePass, 7, 1, 1, 1)

        self.lbMessage = QLabel(AuthForm)
        self.lbMessage.setObjectName(u"lbMessage")
        self.lbMessage.setStyleSheet(u"color: red;")
        self.lbMessage.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lbMessage, 10, 1, 1, 1)

        self.lbName = QLabel(AuthForm)
        self.lbName.setObjectName(u"lbName")

        self.gridLayout.addWidget(self.lbName, 4, 1, 1, 1)

        self.btnCreateAccount = QPushButton(AuthForm)
        self.btnCreateAccount.setObjectName(u"btnCreateAccount")

        self.gridLayout.addWidget(self.btnCreateAccount, 12, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 0, 1, 1)

        self.leName = QLineEdit(AuthForm)
        self.leName.setObjectName(u"leName")

        self.gridLayout.addWidget(self.leName, 5, 1, 1, 1)

        self.lbPassConfirm = QLabel(AuthForm)
        self.lbPassConfirm.setObjectName(u"lbPassConfirm")

        self.gridLayout.addWidget(self.lbPassConfirm, 8, 1, 1, 1)

        self.lePassConfirm = QLineEdit(AuthForm)
        self.lePassConfirm.setObjectName(u"lePassConfirm")
        self.lePassConfirm.setEchoMode(QLineEdit.Password)

        self.gridLayout.addWidget(self.lePassConfirm, 9, 1, 1, 1)

        self.btnMachineData = QPushButton(AuthForm)
        self.btnMachineData.setObjectName(u"btnMachineData")

        self.gridLayout.addWidget(self.btnMachineData, 14, 1, 1, 1)

        self.btnAccess = QPushButton(AuthForm)
        self.btnAccess.setObjectName(u"btnAccess")
        self.btnAccess.setEnabled(True)

        self.gridLayout.addWidget(self.btnAccess, 11, 1, 1, 1)

        self.btnBack = QPushButton(AuthForm)
        self.btnBack.setObjectName(u"btnBack")

        self.gridLayout.addWidget(self.btnBack, 13, 1, 1, 1)

        self.gridLayout.setColumnStretch(0, 1)
        self.gridLayout.setColumnStretch(1, 2)
        self.gridLayout.setColumnStretch(2, 1)
        QWidget.setTabOrder(self.leCpf, self.leName)
        QWidget.setTabOrder(self.leName, self.lePass)
        QWidget.setTabOrder(self.lePass, self.lePassConfirm)
        QWidget.setTabOrder(self.lePassConfirm, self.btnAccess)
        QWidget.setTabOrder(self.btnAccess, self.btnCreateAccount)
        QWidget.setTabOrder(self.btnCreateAccount, self.btnBack)
        QWidget.setTabOrder(self.btnBack, self.btnMachineData)

        self.retranslateUi(AuthForm)

        QMetaObject.connectSlotsByName(AuthForm)
    # setupUi

    def retranslateUi(self, AuthForm):
        AuthForm.setWindowTitle(QCoreApplication.translate("AuthForm", u"Form", None))
        self.label.setText(QCoreApplication.translate("AuthForm", u"CPF", None))
        self.label_2.setText(QCoreApplication.translate("AuthForm", u"Senha", None))
        self.label_3.setText(QCoreApplication.translate("AuthForm", u"Invent\u00e1rio de M\u00e1quinas", None))
        self.lbMessage.setText(QCoreApplication.translate("AuthForm", u"-MESSAGE-", None))
        self.lbName.setText(QCoreApplication.translate("AuthForm", u"Nome", None))
        self.btnCreateAccount.setText(QCoreApplication.translate("AuthForm", u"criar conta", None))
        self.lbPassConfirm.setText(QCoreApplication.translate("AuthForm", u"Confirmar Senha", None))
        self.btnMachineData.setText(QCoreApplication.translate("AuthForm", u"dados da m\u00e1quina (leitura)", None))
        self.btnAccess.setText(QCoreApplication.translate("AuthForm", u"acessar", None))
        self.btnBack.setText(QCoreApplication.translate("AuthForm", u"voltar", None))
    # retranslateUi

