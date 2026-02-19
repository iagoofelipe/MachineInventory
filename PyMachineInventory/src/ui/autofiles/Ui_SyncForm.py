# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'SyncForm.ui'
##
## Created by: Qt User Interface Compiler version 6.10.1
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
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

class Ui_SyncForm(object):
    def setupUi(self, SyncForm):
        if not SyncForm.objectName():
            SyncForm.setObjectName(u"SyncForm")
        SyncForm.resize(390, 404)
        self.verticalLayout = QVBoxLayout(SyncForm)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.lbTitle = QLabel(SyncForm)
        self.lbTitle.setObjectName(u"lbTitle")
        font = QFont()
        font.setPointSize(15)
        self.lbTitle.setFont(font)

        self.verticalLayout.addWidget(self.lbTitle)

        self.verticalSpacer_2 = QSpacerItem(20, 18, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.widget = QWidget(SyncForm)
        self.widget.setObjectName(u"widget")
        self.horizontalLayout_2 = QHBoxLayout(self.widget)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_2)

        self.widget_2 = QWidget(self.widget)
        self.widget_2.setObjectName(u"widget_2")
        self.verticalLayout_3 = QVBoxLayout(self.widget_2)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(0, 0, 0, 0)
        self.checkBox = QCheckBox(self.widget_2)
        self.checkBox.setObjectName(u"checkBox")

        self.verticalLayout_3.addWidget(self.checkBox)

        self.lbCpf = QLabel(self.widget_2)
        self.lbCpf.setObjectName(u"lbCpf")

        self.verticalLayout_3.addWidget(self.lbCpf)

        self.leCpf = QLineEdit(self.widget_2)
        self.leCpf.setObjectName(u"leCpf")

        self.verticalLayout_3.addWidget(self.leCpf)

        self.lbPass = QLabel(self.widget_2)
        self.lbPass.setObjectName(u"lbPass")

        self.verticalLayout_3.addWidget(self.lbPass)

        self.lePass = QLineEdit(self.widget_2)
        self.lePass.setObjectName(u"lePass")
        self.lePass.setEchoMode(QLineEdit.Password)

        self.verticalLayout_3.addWidget(self.lePass)

        self.lbPassConfirm = QLabel(self.widget_2)
        self.lbPassConfirm.setObjectName(u"lbPassConfirm")

        self.verticalLayout_3.addWidget(self.lbPassConfirm)

        self.lePassConfirm = QLineEdit(self.widget_2)
        self.lePassConfirm.setObjectName(u"lePassConfirm")
        self.lePassConfirm.setEchoMode(QLineEdit.Password)

        self.verticalLayout_3.addWidget(self.lePassConfirm)

        self.lbName = QLabel(self.widget_2)
        self.lbName.setObjectName(u"lbName")

        self.verticalLayout_3.addWidget(self.lbName)

        self.leName = QLineEdit(self.widget_2)
        self.leName.setObjectName(u"leName")
        self.leName.setEnabled(True)

        self.verticalLayout_3.addWidget(self.leName)


        self.horizontalLayout_2.addWidget(self.widget_2)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_3)

        self.horizontalLayout_2.setStretch(0, 1)
        self.horizontalLayout_2.setStretch(1, 2)
        self.horizontalLayout_2.setStretch(2, 1)

        self.verticalLayout.addWidget(self.widget)

        self.lbMessage = QLabel(SyncForm)
        self.lbMessage.setObjectName(u"lbMessage")
        font1 = QFont()
        font1.setPointSize(9)
        font1.setBold(False)
        self.lbMessage.setFont(font1)
        self.lbMessage.setStyleSheet(u"color: rgb(173, 0, 0);")
        self.lbMessage.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.lbMessage)

        self.verticalSpacer = QSpacerItem(20, 74, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.widBtns = QWidget(SyncForm)
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


        self.verticalLayout.addWidget(self.widBtns)


        self.retranslateUi(SyncForm)

        QMetaObject.connectSlotsByName(SyncForm)
    # setupUi

    def retranslateUi(self, SyncForm):
        SyncForm.setWindowTitle(QCoreApplication.translate("SyncForm", u"Form", None))
        self.lbTitle.setText(QCoreApplication.translate("SyncForm", u"TITLE", None))
        self.checkBox.setText(QCoreApplication.translate("SyncForm", u"Cadastrar m\u00e1quina de outro usu\u00e1rio", None))
        self.lbCpf.setText(QCoreApplication.translate("SyncForm", u"CPF", None))
        self.lbPass.setText(QCoreApplication.translate("SyncForm", u"Senha", None))
        self.lbPassConfirm.setText(QCoreApplication.translate("SyncForm", u"Confirmar Senha", None))
        self.lbName.setText(QCoreApplication.translate("SyncForm", u"Nome", None))
        self.lbMessage.setText(QCoreApplication.translate("SyncForm", u"MESSAGE", None))
        self.btnSecondary.setText(QCoreApplication.translate("SyncForm", u"secondary", None))
        self.btnContinue.setText(QCoreApplication.translate("SyncForm", u"continuar", None))
    # retranslateUi

