/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : CamConfigWid.cpp
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#include "CamConfigWid.h"
#include "FuVideoButtons.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPoint>
#include <QRect>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QSizePolicy>


TF::CamConfigWid::CamConfigWid(QWidget *parent)
    : QWidget(parent) {
    setObjectName("CamConfigWid");
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
        QWidget#CamConfigWid {
            background-color: #1f2430;
            border: 1px solid #2f3545;
            border-radius: 12px;
        }
        QWidget#CamConfigWid QLabel {
            color: #e8ecf5;
            font-size: 14px;
        }
        QWidget#CamConfigWid QLineEdit {
            background: #11141c;
            color: #e8ecf5;
            border: 1px solid #2f3545;
            border-radius: 8px;
            padding: 6px 10px;
            font-weight: 600;
        }
        QWidget#CamConfigWid QPushButton {
            background: #2c3344;
            color: #e8ecf5;
            border: 1px solid #3b4357;
            border-radius: 10px;
            padding: 8px 14px;
            font-size: 14px;
        }
        QWidget#CamConfigWid QPushButton:hover {
            background: #36405a;
        }
        QWidget#CamConfigWid QPushButton:pressed {
            background: #2a3042;
        }
    )");

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 6);
    setGraphicsEffect(shadow);
    setupUi();
    updateInfoDisplay();
}

void TF::CamConfigWid::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(createInfoPanel());

    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    mainLayout->addWidget(createButtonPanel());
}

QWidget *TF::CamConfigWid::createInfoPanel() {
    auto *panel = new QWidget(this);
    auto *grid = new QGridLayout(panel);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(6);

    mFocusEdit = createInfoField(tr("焦距"), grid, 0);
    mBrightnessEdit = createInfoField(tr("亮度"), grid, 1);
    mApertureEdit = createInfoField(tr("光圈"), grid, 2);
    mExposureEdit = createInfoField(tr("曝光"), grid, 3);

    return panel;
}

QWidget *TF::CamConfigWid::createButtonPanel() {
    auto *panel = new QWidget(this);
    auto *layout = new QGridLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

    auto createActionButton = [panel](const QString &text) {
        auto *btn = new TechActionButton(text, panel);
        btn->setMinimumSize(90, 44);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return btn;
    };

    auto *connectBtn = createActionButton(tr("连接"));
    auto *focusIncBtn = createActionButton(tr("焦距 +"));
    auto *focusDecBtn = createActionButton(tr("焦距 -"));
    auto *brightnessIncBtn = createActionButton(tr("亮度 +"));
    auto *brightnessDecBtn = createActionButton(tr("亮度 -"));

    layout->addWidget(connectBtn, 0, 0);
    layout->addWidget(focusIncBtn, 0, 1);
    layout->addWidget(focusDecBtn, 0, 2);
    layout->addWidget(brightnessIncBtn, 1, 0);
    layout->addWidget(brightnessDecBtn, 1, 1);
    layout->setColumnStretch(2, 1);

    connect(connectBtn, &QPushButton::clicked, this, &CamConfigWid::onConnectClicked);
    connect(focusIncBtn, &QPushButton::clicked, this, &CamConfigWid::onFocusIncrease);
    connect(focusDecBtn, &QPushButton::clicked, this, &CamConfigWid::onFocusDecrease);
    connect(brightnessIncBtn, &QPushButton::clicked, this, &CamConfigWid::onBrightnessIncrease);
    connect(brightnessDecBtn, &QPushButton::clicked, this, &CamConfigWid::onBrightnessDecrease);

    return panel;
}

QLineEdit *TF::CamConfigWid::createInfoField(const QString &labelText, QGridLayout *layout, int row) {
    auto *label = new QLabel(labelText, this);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(68);

    auto *edit = new QLineEdit(this);
    edit->setReadOnly(true);
    edit->setAlignment(Qt::AlignCenter);
    edit->setMinimumHeight(40);

    layout->addWidget(label, row, 0, 1, 1);
    layout->addWidget(edit, row, 1, 1, 2);

    return edit;
}

void TF::CamConfigWid::updateInfoDisplay() {
    if (mFocusEdit) {
        mFocusEdit->setText(QString::number(mFocus));
    }
    if (mBrightnessEdit) {
        mBrightnessEdit->setText(QString::number(mBrightness));
    }
    if (mApertureEdit) {
        mApertureEdit->setText(QString::number(mAperture));
    }
    if (mExposureEdit) {
        mExposureEdit->setText(QString::number(mExposure));
    }
}

void TF::CamConfigWid::showAt(const QRect &targetRect) {
    const int desiredWidth = qMin(targetRect.width(), 360);
    const int desiredHeight = sizeHint().height();
    resize(desiredWidth, desiredHeight);

    const int x = targetRect.right() - desiredWidth;
    move(QPoint(x, targetRect.top()));
    show();
    raise();
    activateWindow();
}

void TF::CamConfigWid::onConnectClicked() {
    // Placeholder for real connection logic
    mExposure = (mExposure + 1) % 10;
    updateInfoDisplay();
}

void TF::CamConfigWid::adjustFocus(int delta) {
    mFocus += delta;
    updateInfoDisplay();
}

void TF::CamConfigWid::adjustBrightness(int delta) {
    mBrightness += delta;
    updateInfoDisplay();
}

void TF::CamConfigWid::onFocusIncrease() {
    adjustFocus(1);
}

void TF::CamConfigWid::onFocusDecrease() {
    adjustFocus(-1);
}

void TF::CamConfigWid::onBrightnessIncrease() {
    adjustBrightness(1);
}

void TF::CamConfigWid::onBrightnessDecrease() {
    adjustBrightness(-1);
}


