/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuSideTabBar.cpp
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "FuSideTabBar.h"
#include "FuSideTabBar_Ui.h"

#include <QMessageBox>
#include <QProcess>
#include <QToolButton>
#include <QStringList>

namespace TF {

    FuSideTabBar::FuSideTabBar(QWidget *parent) : QWidget(parent) {
        setupUi();
        setupConnections();
        setCurrentIndex(0);
    }

    FuSideTabBar::~FuSideTabBar() {
        delete mButtonGroup;
        delete mUi;
    }

    int FuSideTabBar::currentIndex() const {
        if (mButtonGroup == nullptr) {
            return -1;
        }
        return mButtonGroup->checkedId();
    }

    void FuSideTabBar::setCurrentIndex(int index) {
        if (mButtonGroup == nullptr) {
            return;
        }
        auto *button = mButtonGroup->button(index);
        if (button != nullptr) {
            button->setChecked(true);
        }
    }

    void FuSideTabBar::setupUi() {
        mUi = new FuSideTabBar_Ui();
        mUi->setupUi(this);
    }

    void FuSideTabBar::setupConnections() {
        mButtonGroup = new QButtonGroup(this);
        mButtonGroup->setExclusive(true);

        for (int i = 0; i < mUi->mButtons.size(); ++i) {
            auto *button = mUi->mButtons.at(i);
            mButtonGroup->addButton(button, i);
        }

        connect(mButtonGroup, &QButtonGroup::idClicked, this, &FuSideTabBar::tabSelected);
        connect(mUi->mShutdownButton, &QToolButton::clicked, this, &FuSideTabBar::confirmShutdown);
    }

    void FuSideTabBar::confirmShutdown() {
        const auto result = QMessageBox::question(this,
                                                  tr("确认关机"),
                                                  tr("是否确认关机？"),
                                                  QMessageBox::Yes | QMessageBox::No,
                                                  QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }

        QString program;
        QStringList arguments;

#if defined(Q_OS_WIN)
        program = QStringLiteral("shutdown");
        arguments = {QStringLiteral("/s"), QStringLiteral("/t"), QStringLiteral("0")};
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_LINUX)
        program = QStringLiteral("bash");
        arguments = {QStringLiteral("-c"),
                     QStringLiteral("echo fire | sudo -S shutdown -h now")};
        QProcess::startDetached(program, arguments);
#else
        return;
#endif
    }
}
