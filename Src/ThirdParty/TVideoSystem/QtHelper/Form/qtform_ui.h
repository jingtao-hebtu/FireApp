/********************************************************************************
** Form generated from reading UI file 'qtform.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTFORM_H
#define UI_QTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtForm {
public:
    QVBoxLayout *verticalLayout;
    QWidget *QtFormTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *QtLabIco;
    QLabel *QtLabTitle;
    QToolButton *btnMenu;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Max;
    QPushButton *btnMenu_Close;
    QWidget *QtFormMain;
    QVBoxLayout *verticalLayout_2;

    void setupUi(QDialog *QtForm) {
        if (QtForm->objectName().isEmpty())
            QtForm->setObjectName("QtForm");
        QtForm->resize(400, 300);
        verticalLayout = new QVBoxLayout(QtForm);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        QtFormTitle = new QWidget(QtForm);
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
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(QtLabTitle->sizePolicy().hasHeightForWidth());
        QtLabTitle->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(QtLabTitle);

        btnMenu = new QToolButton(QtFormTitle);
        btnMenu->setObjectName("btnMenu");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btnMenu->sizePolicy().hasHeightForWidth());
        btnMenu->setSizePolicy(sizePolicy2);
        btnMenu->setPopupMode(QToolButton::InstantPopup);

        horizontalLayout_2->addWidget(btnMenu);

        btnMenu_Min = new QPushButton(QtFormTitle);
        btnMenu_Min->setObjectName("btnMenu_Min");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnMenu_Min->sizePolicy().hasHeightForWidth());
        btnMenu_Min->setSizePolicy(sizePolicy3);
        btnMenu_Min->setFlat(true);

        horizontalLayout_2->addWidget(btnMenu_Min);

        btnMenu_Max = new QPushButton(QtFormTitle);
        btnMenu_Max->setObjectName("btnMenu_Max");
        sizePolicy3.setHeightForWidth(btnMenu_Max->sizePolicy().hasHeightForWidth());
        btnMenu_Max->setSizePolicy(sizePolicy3);
        btnMenu_Max->setFocusPolicy(Qt::NoFocus);
        btnMenu_Max->setFlat(true);

        horizontalLayout_2->addWidget(btnMenu_Max);

        btnMenu_Close = new QPushButton(QtFormTitle);
        btnMenu_Close->setObjectName("btnMenu_Close");
        sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy3);
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);

        horizontalLayout_2->addWidget(btnMenu_Close);


        verticalLayout->addWidget(QtFormTitle);

        QtFormMain = new QWidget(QtForm);
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
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtForm);

        QMetaObject::connectSlotsByName(QtForm);
    } // setupUi

    void retranslateUi(QDialog *QtForm) {
        QtForm->setWindowTitle(QCoreApplication::translate("QtForm", "Dialog", nullptr));
        QtLabIco->setText(QString());
        QtLabTitle->setText(
                QCoreApplication::translate("QtForm", "\350\207\252\345\256\232\344\271\211\347\252\227\344\275\223",
                                            nullptr));
        btnMenu->setText(QString());
        btnMenu_Min->setText(QString());
        btnMenu_Max->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QtForm : public Ui_QtForm {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTFORM_H
