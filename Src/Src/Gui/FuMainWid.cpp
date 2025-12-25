/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuMainWid.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "FuMainWid.h"
#include "FuMainWid_Ui.h"
#include "FuSideTabBar.h"
#include "FuMainMeaPage.h"
#include <QFile>
#include <QMessageBox>


TF::FuMainWid::FuMainWid(QWidget* parent) : QWidget(parent) {
    setupUi();
    initStyle();
    initActions();

    setupConnections();
}

TF::FuMainWid::~FuMainWid() {
    delete mUi;
}

void TF::FuMainWid::initAfterDisplay() {
    mUi->mMainMeaPage->initAfterDisplay();
}

void TF::FuMainWid::setupUi() {
    mUi = new FuMainWid_Ui();
    mUi->setupUi(this);
}

void TF::FuMainWid::initStyle() {
    QFile win_style_file(QString(":/qss/FuMainWid.css"));
    if (win_style_file.open(QFile::ReadOnly)) {
        QString styleStr = win_style_file.readAll();
        setStyleSheet(styleStr);
        win_style_file.close();
    }
}

void TF::FuMainWid::initActions() {
    connect(this, &FuMainWid::promptError, this, &FuMainWid::promptError);
}

void TF::FuMainWid::onPromptError(const QString &msg) {
    //QMessageBox::critical(this, "错误", msg);
}

void TF::FuMainWid::setupConnections() {
    if (mUi == nullptr || mUi->mSideTabBar == nullptr) {
        return;
    }

    connect(mUi->mSideTabBar, &FuSideTabBar::tabSelected, this, [this](int index) {
        if (mUi->mPages.isEmpty() || index < 0 || index >= mUi->mPages.size()) {
            return;
        }

        for (int i = 0; i < mUi->mPages.size(); ++i) {
            auto *page = mUi->mPages.at(i);
            const bool visible = i == index;
            page->setVisible(visible);
            if (visible) {
                page->raise();
            }
        }
    });

    mUi->mSideTabBar->setCurrentIndex(0);
}
