/********************************************************************************
** Form generated from reading UI file 'qttipbox.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTIPBOX_H
#define UI_QTTIPBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtTipBox {
public:
    QVBoxLayout *verticalLayout;
    QWidget *QtFormTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *QtLabIco;
    QLabel *QtLabTitle;
    QLabel *QtLabCountDown;
    QPushButton *btnMenu_Close;
    QWidget *QtFormMain;
    QVBoxLayout *verticalLayout_2;
    QLabel *QtLabInfo;

    void setupUi(QDialog *QtTipBox) {
        if (QtTipBox->objectName().isEmpty())
            QtTipBox->setObjectName("QtTipBox");
        QtTipBox->resize(400, 300);
        verticalLayout = new QVBoxLayout(QtTipBox);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        QtFormTitle = new QWidget(QtTipBox);
        QtFormTitle->setObjectName("QtFormTitle");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QtFormTitle->sizePolicy().hasHeightForWidth());
        QtFormTitle->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(QtFormTitle);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        QtLabIco = new QLabel(QtFormTitle);
        QtLabIco->setObjectName("QtLabIco");
        QtLabIco->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(QtLabIco);

        QtLabTitle = new QLabel(QtFormTitle);
        QtLabTitle->setObjectName("QtLabTitle");
        QtLabTitle->setStyleSheet(QString::fromUtf8(""));
        QtLabTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

        horizontalLayout_2->addWidget(QtLabTitle);

        QtLabCountDown = new QLabel(QtFormTitle);
        QtLabCountDown->setObjectName("QtLabCountDown");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(QtLabCountDown->sizePolicy().hasHeightForWidth());
        QtLabCountDown->setSizePolicy(sizePolicy1);
        QtLabCountDown->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(QtLabCountDown);

        btnMenu_Close = new QPushButton(QtFormTitle);
        btnMenu_Close->setObjectName("btnMenu_Close");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy2);
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);

        horizontalLayout_2->addWidget(btnMenu_Close);


        verticalLayout->addWidget(QtFormTitle);

        QtFormMain = new QWidget(QtTipBox);
        QtFormMain->setObjectName("QtFormMain");
        QtFormMain->setStyleSheet(QString::fromUtf8("#gboxSoftKey .QPushButton{\n"
                                                    "border-width:0px;\n"
                                                    "border-radius:0px;\n"
                                                    "padding:2px;\n"
                                                    "}\n"
                                                    "\n"
                                                    "#gboxSoftKey .QPushButton:hover{\n"
                                                    "border:1px solid #FF0000;\n"
                                                    "}\n"
                                                    "\n"
                                                    "#gboxSoftKey{\n"
                                                    "border-width:0px;\n"
                                                    "border-radius:0px;\n"
                                                    "}"));
        verticalLayout_2 = new QVBoxLayout(QtFormMain);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(6, 6, 6, 6);
        QtLabInfo = new QLabel(QtFormMain);
        QtLabInfo->setObjectName("QtLabInfo");

        verticalLayout_2->addWidget(QtLabInfo);


        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtTipBox);

        QMetaObject::connectSlotsByName(QtTipBox);
    } // setupUi

    void retranslateUi(QDialog *QtTipBox) {
        QtTipBox->setWindowTitle(QCoreApplication::translate("QtTipBox", "Dialog", nullptr));
        QtLabIco->setText(QString());
        QtLabTitle->setText(
                QCoreApplication::translate("QtTipBox", "\346\266\210\346\201\257\346\217\220\347\244\272", nullptr));
        QtLabInfo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QtTipBox : public Ui_QtTipBox {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTIPBOX_H
