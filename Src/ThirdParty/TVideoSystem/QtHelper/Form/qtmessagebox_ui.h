/********************************************************************************
** Form generated from reading UI file 'qtmessagebox.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTMESSAGEBOX_H
#define UI_QTMESSAGEBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtMessageBox {
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
    QGridLayout *gridLayout;
    QLabel *QtLabIconMain;
    QLabel *QtLabInfo;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *QtMessageBox) {
        if (QtMessageBox->objectName().isEmpty())
            QtMessageBox->setObjectName("QtMessageBox");
        QtMessageBox->resize(400, 300);
        verticalLayout = new QVBoxLayout(QtMessageBox);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        QtFormTitle = new QWidget(QtMessageBox);
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

        QtFormMain = new QWidget(QtMessageBox);
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
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(6, 6, 6, 6);
        QtLabIconMain = new QLabel(frame);
        QtLabIconMain->setObjectName("QtLabIconMain");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(QtLabIconMain->sizePolicy().hasHeightForWidth());
        QtLabIconMain->setSizePolicy(sizePolicy3);
        QtLabIconMain->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(QtLabIconMain, 0, 0, 1, 1);

        QtLabInfo = new QLabel(frame);
        QtLabInfo->setObjectName("QtLabInfo");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(QtLabInfo->sizePolicy().hasHeightForWidth());
        QtLabInfo->setSizePolicy(sizePolicy4);
        QtLabInfo->setWordWrap(true);

        gridLayout->addWidget(QtLabInfo, 0, 1, 1, 1);

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


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 2);


        verticalLayout_2->addWidget(frame);


        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtMessageBox);

        QMetaObject::connectSlotsByName(QtMessageBox);
    } // setupUi

    void retranslateUi(QDialog *QtMessageBox) {
        QtMessageBox->setWindowTitle(QCoreApplication::translate("QtMessageBox", "Dialog", nullptr));
        QtLabIco->setText(QString());
        QtLabTitle->setText(
                QCoreApplication::translate("QtMessageBox", "\346\266\210\346\201\257\346\241\206", nullptr));
        QtLabIconMain->setText(QString());
        QtLabInfo->setText(QString());
        btnOk->setText(QCoreApplication::translate("QtMessageBox", "\347\241\256 \345\256\232", nullptr));
#if QT_CONFIG(tooltip)
        btnCancel->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        btnCancel->setText(QCoreApplication::translate("QtMessageBox", "\345\217\226 \346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtMessageBox : public Ui_QtMessageBox {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTMESSAGEBOX_H
