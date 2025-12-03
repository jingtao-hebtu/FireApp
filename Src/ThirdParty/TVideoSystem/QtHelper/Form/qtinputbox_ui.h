/********************************************************************************
** Form generated from reading UI file 'qtinputbox.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTINPUTBOX_H
#define UI_QTINPUTBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtInputBox {
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
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QLabel *QtLabInfo;
    QLineEdit *QtTxtValue;
    QComboBox *QtCboxValue;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *QtInputBox) {
        if (QtInputBox->objectName().isEmpty())
            QtInputBox->setObjectName("QtInputBox");
        QtInputBox->resize(320, 186);
        verticalLayout = new QVBoxLayout(QtInputBox);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        QtFormTitle = new QWidget(QtInputBox);
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

        QtFormMain = new QWidget(QtInputBox);
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
        frame = new QFrame(QtFormMain);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(6, 6, 6, 6);
        QtLabInfo = new QLabel(frame);
        QtLabInfo->setObjectName("QtLabInfo");

        verticalLayout_3->addWidget(QtLabInfo);

        QtTxtValue = new QLineEdit(frame);
        QtTxtValue->setObjectName("QtTxtValue");

        verticalLayout_3->addWidget(QtTxtValue);

        QtCboxValue = new QComboBox(frame);
        QtCboxValue->setObjectName("QtCboxValue");

        verticalLayout_3->addWidget(QtCboxValue);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOk = new QPushButton(frame);
        btnOk->setObjectName("btnOk");
        btnOk->setMinimumSize(QSize(85, 0));

        horizontalLayout->addWidget(btnOk);

        btnCancel = new QPushButton(frame);
        btnCancel->setObjectName("btnCancel");
        btnCancel->setMinimumSize(QSize(85, 0));

        horizontalLayout->addWidget(btnCancel);


        verticalLayout_3->addLayout(horizontalLayout);


        verticalLayout_2->addWidget(frame);


        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtInputBox);

        QMetaObject::connectSlotsByName(QtInputBox);
    } // setupUi

    void retranslateUi(QDialog *QtInputBox) {
        QtInputBox->setWindowTitle(QCoreApplication::translate("QtInputBox", "Dialog", nullptr));
        QtLabIco->setText(QString());
        QtLabTitle->setText(QCoreApplication::translate("QtInputBox", "\350\276\223\345\205\245\346\241\206", nullptr));
        QtLabInfo->setText(QCoreApplication::translate("QtInputBox", "\350\257\267\350\276\223\345\205\245:", nullptr));
        btnOk->setText(QCoreApplication::translate("QtInputBox", "\347\241\256 \345\256\232", nullptr));
#if QT_CONFIG(tooltip)
        btnCancel->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        btnCancel->setText(QCoreApplication::translate("QtInputBox", "\345\217\226 \346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtInputBox : public Ui_QtInputBox {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTINPUTBOX_H
