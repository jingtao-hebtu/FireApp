/********************************************************************************
** Form generated from reading UI file 'qtabout.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTABOUT_H
#define UI_QTABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtAbout {
public:
    QVBoxLayout *verticalLayout;
    QWidget *QtFormMain;
    QLabel *QtLabTitle;
    QLabel *QtLabName;
    QLabel *QtLabIco;
    QPushButton *btnMenu_Close;
    QLabel *QtLabImage;
    QLabel *QtLabInfo;

    void setupUi(QDialog *QtAbout) {
        if (QtAbout->objectName().isEmpty())
            QtAbout->setObjectName("QtAbout");
        QtAbout->resize(720, 300);
        QtAbout->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(QtAbout);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        QtFormMain = new QWidget(QtAbout);
        QtFormMain->setObjectName("QtFormMain");
        QtLabTitle = new QLabel(QtFormMain);
        QtLabTitle->setObjectName("QtLabTitle");
        QtLabTitle->setGeometry(QRect(50, 9, 401, 31));
        QtLabName = new QLabel(QtFormMain);
        QtLabName->setObjectName("QtLabName");
        QtLabName->setGeometry(QRect(50, 50, 401, 51));
        QtLabName->setStyleSheet(QString::fromUtf8("font: 20pt;"));
        QtLabIco = new QLabel(QtFormMain);
        QtLabIco->setObjectName("QtLabIco");
        QtLabIco->setGeometry(QRect(12, 9, 31, 30));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QtLabIco->sizePolicy().hasHeightForWidth());
        QtLabIco->setSizePolicy(sizePolicy);
        QtLabIco->setMinimumSize(QSize(31, 0));
        QtLabIco->setAlignment(Qt::AlignCenter);
        btnMenu_Close = new QPushButton(QtFormMain);
        btnMenu_Close->setObjectName("btnMenu_Close");
        btnMenu_Close->setGeometry(QRect(670, 6, 33, 30));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy1);
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);
        QtLabImage = new QLabel(QtFormMain);
        QtLabImage->setObjectName("QtLabImage");
        QtLabImage->setGeometry(QRect(450, 30, 241, 241));
        QtLabImage->setAlignment(Qt::AlignCenter);
        QtLabInfo = new QLabel(QtFormMain);
        QtLabInfo->setObjectName("QtLabInfo");
        QtLabInfo->setGeometry(QRect(50, 110, 401, 151));

        verticalLayout->addWidget(QtFormMain);


        retranslateUi(QtAbout);

        QMetaObject::connectSlotsByName(QtAbout);
    } // setupUi

    void retranslateUi(QDialog *QtAbout) {
        QtLabTitle->setText(QCoreApplication::translate("QtAbout", "\345\205\263\344\272\216", nullptr));
        QtLabName->setText(QString());
        QtLabIco->setText(QString());
        QtLabImage->setText(QString());
        (void) QtAbout;
    } // retranslateUi

};

namespace Ui {
    class QtAbout : public Ui_QtAbout {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTABOUT_H
