/********************************************************************************
** Form generated from reading UI file 'frmvideowidgetpreview.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMVIDEOWIDGETPREVIEW_H
#define UI_FRMVIDEOWIDGETPREVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmVideoWidgetPreview
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnMenu_Close;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_3;

    void setupUi(QDialog *frmVideoWidgetPreview)
    {
        if (frmVideoWidgetPreview->objectName().isEmpty())
            frmVideoWidgetPreview->setObjectName("frmVideoWidgetPreview");
        frmVideoWidgetPreview->resize(800, 600);
        verticalLayout = new QVBoxLayout(frmVideoWidgetPreview);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widgetTitle = new QWidget(frmVideoWidgetPreview);
        widgetTitle->setObjectName("widgetTitle");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
        widgetTitle->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widgetTitle);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labIco = new QLabel(widgetTitle);
        labIco->setObjectName("labIco");
        labIco->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(labIco);

        labTitle = new QLabel(widgetTitle);
        labTitle->setObjectName("labTitle");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labTitle->sizePolicy().hasHeightForWidth());
        labTitle->setSizePolicy(sizePolicy1);
        labTitle->setStyleSheet(QString::fromUtf8(""));
        labTitle->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labTitle);

        widgetMenu = new QWidget(widgetTitle);
        widgetMenu->setObjectName("widgetMenu");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
        widgetMenu->setSizePolicy(sizePolicy2);
        horizontalLayout = new QHBoxLayout(widgetMenu);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(10, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnMenu_Close = new QPushButton(widgetMenu);
        btnMenu_Close->setObjectName("btnMenu_Close");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy3);
        btnMenu_Close->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);

        horizontalLayout->addWidget(btnMenu_Close);


        horizontalLayout_2->addWidget(widgetMenu);


        verticalLayout->addWidget(widgetTitle);

        widget = new QWidget(frmVideoWidgetPreview);
        widget->setObjectName("widget");
        horizontalLayout_3 = new QHBoxLayout(widget);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(widget);


        retranslateUi(frmVideoWidgetPreview);

        QMetaObject::connectSlotsByName(frmVideoWidgetPreview);
    } // setupUi

    void retranslateUi(QDialog *frmVideoWidgetPreview)
    {
        frmVideoWidgetPreview->setWindowTitle(QCoreApplication::translate("frmVideoWidgetPreview", "Form", nullptr));
        labIco->setText(QString());
        labTitle->setText(QCoreApplication::translate("frmVideoWidgetPreview", "\350\247\206\351\242\221\351\242\204\350\247\210", nullptr));
        btnMenu_Close->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class frmVideoWidgetPreview: public Ui_frmVideoWidgetPreview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMVIDEOWIDGETPREVIEW_H
