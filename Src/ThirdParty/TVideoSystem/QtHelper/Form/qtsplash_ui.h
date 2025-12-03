/********************************************************************************
** Form generated from reading UI file 'qtsplash.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSPLASH_H
#define UI_QTSPLASH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSplash {
public:
    QVBoxLayout *verticalLayout;
    QWidget *QtFormMain;
    QVBoxLayout *verticalLayout_2;
    QLabel *QtLabInfo;
    QLabel *QtLabCountDown;

    void setupUi(QDialog *QtSplash) {
        if (QtSplash->objectName().isEmpty())
            QtSplash->setObjectName("QtSplash");
        QtSplash->resize(520, 120);
        verticalLayout = new QVBoxLayout(QtSplash);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        QtFormMain = new QWidget(QtSplash);
        QtFormMain->setObjectName("QtFormMain");
        verticalLayout_2 = new QVBoxLayout(QtFormMain);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        QtLabInfo = new QLabel(QtFormMain);
        QtLabInfo->setObjectName("QtLabInfo");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QtLabInfo->sizePolicy().hasHeightForWidth());
        QtLabInfo->setSizePolicy(sizePolicy);
        QtLabInfo->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(QtLabInfo);

        QtLabCountDown = new QLabel(QtFormMain);
        QtLabCountDown->setObjectName("QtLabCountDown");
        QtLabCountDown->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

        verticalLayout_2->addWidget(QtLabCountDown);


        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtSplash);

        QMetaObject::connectSlotsByName(QtSplash);
    } // setupUi

    void retranslateUi(QDialog *QtSplash) {
        QtSplash->setWindowTitle(QCoreApplication::translate("QtSplash", "Form", nullptr));
        QtLabCountDown->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QtSplash : public Ui_QtSplash {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSPLASH_H
