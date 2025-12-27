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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRect>
#include <QPushButton>


TF::CamConfigWid::CamConfigWid(QWidget *parent)
    : QWidget(parent) {
    setObjectName("CamConfigWid");
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    updateInfoDisplay();
}

void TF::CamConfigWid::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

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
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);

    mFocusEdit = createInfoField(tr("焦距"), grid, 0);
    mBrightnessEdit = createInfoField(tr("亮度"), grid, 1);
    mApertureEdit = createInfoField(tr("光圈"), grid, 2);
    mExposureEdit = createInfoField(tr("曝光"), grid, 3);

    return panel;
}

QWidget *TF::CamConfigWid::createButtonPanel() {
    auto *panel = new QWidget(this);
    auto *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto createActionButton = [panel](const QString &text) {
        auto *btn = new TechActionButton(text, panel);
        btn->setMinimumSize(70, 60);
        return btn;
    };

    auto *connectBtn = createActionButton(tr("连接"));
    auto *focusIncBtn = createActionButton(tr("焦距 +"));
    auto *focusDecBtn = createActionButton(tr("焦距 -"));
    auto *brightnessIncBtn = createActionButton(tr("亮度 +"));
    auto *brightnessDecBtn = createActionButton(tr("亮度 -"));

    layout->addWidget(connectBtn);
    layout->addWidget(focusIncBtn);
    layout->addWidget(focusDecBtn);
    layout->addWidget(brightnessIncBtn);
    layout->addWidget(brightnessDecBtn);
    layout->addStretch();

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
    label->setMinimumWidth(80);

    auto *edit = new QLineEdit(this);
    edit->setReadOnly(true);
    edit->setAlignment(Qt::AlignCenter);
    edit->setMinimumHeight(44);

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
    resize(targetRect.size());
    move(targetRect.topLeft());
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


