/********************************************************************************
** Form generated from reading UI file 'frmconfigdb.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMCONFIGDB_H
#define UI_FRMCONFIGDB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmConfigDb
{
public:
    QGridLayout *gridLayout;
    QComboBox *cboxConnMode;
    QLineEdit *txtHostInfo;
    QComboBox *cboxDbType;
    QLabel *labDbType;
    QLabel *labHostInfo;
    QLineEdit *txtUserPwd;
    QLabel *labDbName;
    QPushButton *btnConnect;
    QLabel *labUserPwd;
    QLineEdit *txtUserName;
    QPushButton *btnInit;
    QLabel *labConnMode;
    QLineEdit *txtDbName;
    QLabel *labUserName;

    void setupUi(QWidget *frmConfigDb)
    {
        if (frmConfigDb->objectName().isEmpty())
            frmConfigDb->setObjectName("frmConfigDb");
        frmConfigDb->resize(328, 141);
        gridLayout = new QGridLayout(frmConfigDb);
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(6, 6, 6, 6);
        cboxConnMode = new QComboBox(frmConfigDb);
        cboxConnMode->setObjectName("cboxConnMode");

        gridLayout->addWidget(cboxConnMode, 0, 1, 1, 1);

        txtHostInfo = new QLineEdit(frmConfigDb);
        txtHostInfo->setObjectName("txtHostInfo");

        gridLayout->addWidget(txtHostInfo, 1, 3, 1, 1);

        cboxDbType = new QComboBox(frmConfigDb);
        cboxDbType->setObjectName("cboxDbType");

        gridLayout->addWidget(cboxDbType, 0, 3, 1, 1);

        labDbType = new QLabel(frmConfigDb);
        labDbType->setObjectName("labDbType");

        gridLayout->addWidget(labDbType, 0, 2, 1, 1);

        labHostInfo = new QLabel(frmConfigDb);
        labHostInfo->setObjectName("labHostInfo");

        gridLayout->addWidget(labHostInfo, 1, 2, 1, 1);

        txtUserPwd = new QLineEdit(frmConfigDb);
        txtUserPwd->setObjectName("txtUserPwd");
        txtUserPwd->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(txtUserPwd, 2, 3, 1, 1);

        labDbName = new QLabel(frmConfigDb);
        labDbName->setObjectName("labDbName");

        gridLayout->addWidget(labDbName, 1, 0, 1, 1);

        btnConnect = new QPushButton(frmConfigDb);
        btnConnect->setObjectName("btnConnect");

        gridLayout->addWidget(btnConnect, 3, 0, 1, 2);

        labUserPwd = new QLabel(frmConfigDb);
        labUserPwd->setObjectName("labUserPwd");

        gridLayout->addWidget(labUserPwd, 2, 2, 1, 1);

        txtUserName = new QLineEdit(frmConfigDb);
        txtUserName->setObjectName("txtUserName");

        gridLayout->addWidget(txtUserName, 2, 1, 1, 1);

        btnInit = new QPushButton(frmConfigDb);
        btnInit->setObjectName("btnInit");

        gridLayout->addWidget(btnInit, 3, 2, 1, 2);

        labConnMode = new QLabel(frmConfigDb);
        labConnMode->setObjectName("labConnMode");

        gridLayout->addWidget(labConnMode, 0, 0, 1, 1);

        txtDbName = new QLineEdit(frmConfigDb);
        txtDbName->setObjectName("txtDbName");

        gridLayout->addWidget(txtDbName, 1, 1, 1, 1);

        labUserName = new QLabel(frmConfigDb);
        labUserName->setObjectName("labUserName");

        gridLayout->addWidget(labUserName, 2, 0, 1, 1);

        QWidget::setTabOrder(cboxConnMode, cboxDbType);
        QWidget::setTabOrder(cboxDbType, txtDbName);
        QWidget::setTabOrder(txtDbName, txtHostInfo);
        QWidget::setTabOrder(txtHostInfo, txtUserName);
        QWidget::setTabOrder(txtUserName, txtUserPwd);
        QWidget::setTabOrder(txtUserPwd, btnConnect);
        QWidget::setTabOrder(btnConnect, btnInit);

        retranslateUi(frmConfigDb);

        QMetaObject::connectSlotsByName(frmConfigDb);
    } // setupUi

    void retranslateUi(QWidget *frmConfigDb)
    {
        frmConfigDb->setWindowTitle(QCoreApplication::translate("frmConfigDb", "Form", nullptr));
        labDbType->setText(QCoreApplication::translate("frmConfigDb", "\344\270\273\346\234\272\347\261\273\345\236\213", nullptr));
        labHostInfo->setText(QCoreApplication::translate("frmConfigDb", "\344\270\273\346\234\272\344\277\241\346\201\257", nullptr));
        labDbName->setText(QCoreApplication::translate("frmConfigDb", "\346\225\260\346\215\256\345\272\223\345\220\215", nullptr));
        btnConnect->setText(QCoreApplication::translate("frmConfigDb", "\350\277\236\346\216\245\346\265\213\350\257\225", nullptr));
        labUserPwd->setText(QCoreApplication::translate("frmConfigDb", "\347\224\250\346\210\267\345\257\206\347\240\201", nullptr));
        btnInit->setText(QCoreApplication::translate("frmConfigDb", "\345\210\235\345\247\213\345\214\226\346\225\260\346\215\256", nullptr));
        labConnMode->setText(QCoreApplication::translate("frmConfigDb", "\350\277\236\346\216\245\346\226\271\345\274\217", nullptr));
        labUserName->setText(QCoreApplication::translate("frmConfigDb", "\347\224\250\346\210\267\345\220\215\347\247\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class frmConfigDb: public Ui_frmConfigDb {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMCONFIGDB_H
