/********************************************************************************
** Form generated from reading UI file 'qtdateselect.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDATESELECT_H
#define UI_QTDATESELECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
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

class Ui_QtDateSelect {
public:
    QVBoxLayout *verticalLayout;
    QWidget *QtFormTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *QtLabIco;
    QLabel *QtLabTitle;
    QPushButton *btnMenu_Close;
    QWidget *QtFormMain;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *labStart;
    QDateTimeEdit *dateTimeStart;
    QLabel *labEnd;
    QDateTimeEdit *dateTimeEnd;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *QtDateSelect) {
        if (QtDateSelect->objectName().isEmpty())
            QtDateSelect->setObjectName("QtDateSelect");
        QtDateSelect->resize(300, 156);
        verticalLayout = new QVBoxLayout(QtDateSelect);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        QtFormTitle = new QWidget(QtDateSelect);
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
        QtLabTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

        horizontalLayout_2->addWidget(QtLabTitle);

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

        QtFormMain = new QWidget(QtDateSelect);
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
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(6, 6, 6, 6);
        labStart = new QLabel(frame);
        labStart->setObjectName("labStart");

        gridLayout->addWidget(labStart, 0, 0, 1, 1);

        dateTimeStart = new QDateTimeEdit(frame);
        dateTimeStart->setObjectName("dateTimeStart");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(dateTimeStart->sizePolicy().hasHeightForWidth());
        dateTimeStart->setSizePolicy(sizePolicy3);
        dateTimeStart->setCalendarPopup(true);

        gridLayout->addWidget(dateTimeStart, 0, 1, 1, 1);

        labEnd = new QLabel(frame);
        labEnd->setObjectName("labEnd");

        gridLayout->addWidget(labEnd, 1, 0, 1, 1);

        dateTimeEnd = new QDateTimeEdit(frame);
        dateTimeEnd->setObjectName("dateTimeEnd");
        sizePolicy3.setHeightForWidth(dateTimeEnd->sizePolicy().hasHeightForWidth());
        dateTimeEnd->setSizePolicy(sizePolicy3);
        dateTimeEnd->setCalendarPopup(true);

        gridLayout->addWidget(dateTimeEnd, 1, 1, 1, 1);

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


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 2);


        verticalLayout_2->addWidget(frame);


        verticalLayout->addWidget(QtFormMain);

        QWidget::setTabOrder(btnOk, btnCancel);
        QWidget::setTabOrder(btnCancel, dateTimeStart);
        QWidget::setTabOrder(dateTimeStart, dateTimeEnd);

        retranslateUi(QtDateSelect);

        QMetaObject::connectSlotsByName(QtDateSelect);
    } // setupUi

    void retranslateUi(QDialog *QtDateSelect) {
        QtDateSelect->setWindowTitle(QCoreApplication::translate("QtDateSelect", "Dialog", nullptr));
        QtLabIco->setText(QString());
        QtLabTitle->setText(QCoreApplication::translate("QtDateSelect",
                                                        "\346\227\245\346\234\237\346\227\266\351\227\264\351\200\211\346\213\251",
                                                        nullptr));
        labStart->setText(
                QCoreApplication::translate("QtDateSelect", "\345\274\200\345\247\213\346\227\266\351\227\264",
                                            nullptr));
        labEnd->setText(QCoreApplication::translate("QtDateSelect", "\347\273\223\346\235\237\346\227\266\351\227\264",
                                                    nullptr));
        btnOk->setText(QCoreApplication::translate("QtDateSelect", "\347\241\256 \345\256\232", nullptr));
#if QT_CONFIG(tooltip)
        btnCancel->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        btnCancel->setText(QCoreApplication::translate("QtDateSelect", "\345\217\226 \346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtDateSelect : public Ui_QtDateSelect {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDATESELECT_H
