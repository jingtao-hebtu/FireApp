/********************************************************************************
** Form generated from reading UI file 'frmvideowidgetslider.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMVIDEOWIDGETSLIDER_H
#define UI_FRMVIDEOWIDGETSLIDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "xslider.h"

QT_BEGIN_NAMESPACE

class Ui_frmVideoWidgetSlider
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frameSlider;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labPosition;
    QLabel *labSlash;
    QLabel *labDuration;
    XSlider *sliderPosition;
    QPushButton *btnPlay;
    QPushButton *btnStop;
    QPushButton *btnMute;

    void setupUi(QWidget *frmVideoWidgetSlider)
    {
        if (frmVideoWidgetSlider->objectName().isEmpty())
            frmVideoWidgetSlider->setObjectName("frmVideoWidgetSlider");
        frmVideoWidgetSlider->resize(400, 300);
        verticalLayout = new QVBoxLayout(frmVideoWidgetSlider);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frameSlider = new QFrame(frmVideoWidgetSlider);
        frameSlider->setObjectName("frameSlider");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frameSlider->sizePolicy().hasHeightForWidth());
        frameSlider->setSizePolicy(sizePolicy);
        frameSlider->setFrameShape(QFrame::Box);
        frameSlider->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(frameSlider);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(6, 6, 6, 6);
        labPosition = new QLabel(frameSlider);
        labPosition->setObjectName("labPosition");
        labPosition->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labPosition);

        labSlash = new QLabel(frameSlider);
        labSlash->setObjectName("labSlash");

        horizontalLayout_2->addWidget(labSlash);

        labDuration = new QLabel(frameSlider);
        labDuration->setObjectName("labDuration");
        labDuration->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labDuration);

        sliderPosition = new XSlider(frameSlider);
        sliderPosition->setObjectName("sliderPosition");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(sliderPosition->sizePolicy().hasHeightForWidth());
        sliderPosition->setSizePolicy(sizePolicy1);
        sliderPosition->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(sliderPosition);

        btnPlay = new QPushButton(frameSlider);
        btnPlay->setObjectName("btnPlay");

        horizontalLayout_2->addWidget(btnPlay);

        btnStop = new QPushButton(frameSlider);
        btnStop->setObjectName("btnStop");

        horizontalLayout_2->addWidget(btnStop);

        btnMute = new QPushButton(frameSlider);
        btnMute->setObjectName("btnMute");

        horizontalLayout_2->addWidget(btnMute);


        verticalLayout->addWidget(frameSlider);


        retranslateUi(frmVideoWidgetSlider);

        QMetaObject::connectSlotsByName(frmVideoWidgetSlider);
    } // setupUi

    void retranslateUi(QWidget *frmVideoWidgetSlider)
    {
        frmVideoWidgetSlider->setWindowTitle(QCoreApplication::translate("frmVideoWidgetSlider", "Form", nullptr));
        labPosition->setText(QCoreApplication::translate("frmVideoWidgetSlider", "00:00", nullptr));
        labSlash->setText(QCoreApplication::translate("frmVideoWidgetSlider", "/", nullptr));
        labDuration->setText(QCoreApplication::translate("frmVideoWidgetSlider", "00:00", nullptr));
        btnPlay->setText(QString());
        btnStop->setText(QString());
        btnMute->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class frmVideoWidgetSlider: public Ui_frmVideoWidgetSlider {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMVIDEOWIDGETSLIDER_H
