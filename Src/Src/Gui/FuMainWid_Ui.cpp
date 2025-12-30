/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuMainWid_Ui.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "FuMainWid_Ui.h"
#include "FuMainMeaPage.h"
#include "ExperimentDataViewPage.h"
#include "FuSideTabBar.h"
#include "TFMeaManager.h"

#include <QLabel>
#include <QObject>
#include <QSizePolicy>
#include <QStackedLayout>


void TF::FuMainWid_Ui::setupUi(QWidget *wid) {
    mWid = wid;
    mWid->setObjectName("FuMainWid");
    mWid->resize(1440, 720);

    mLayout = new QHBoxLayout(mWid);
    mLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setSpacing(0);

    mSideTabBar = new FuSideTabBar(mWid);
    mSideTabBar->setObjectName("SideTabBar");
    mSideTabBar->setFixedWidth(80);

    mContentWidget = new QWidget(mWid);
    mContentLayout = new QStackedLayout(mContentWidget);
    mContentLayout->setContentsMargins(0, 0, 0, 0);
    mContentLayout->setStackingMode(QStackedLayout::StackOne);

    mMainMeaPage = new FuMainMeaPage(mContentWidget);
    mMainMeaPage->setObjectName("VideoPage");
    TFMeaManager::instance().setMeaPage(mMainMeaPage);

    mExperimentViewPage = new ExperimentDataViewPage(mContentWidget);
    mExperimentViewPage->setObjectName("ExperimentViewPage");
    mExperimentViewPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto createPage = [](const QString &title, const QString &desc) {
        auto *page = new QWidget();
        auto *layout = new QVBoxLayout(page);
        layout->setAlignment(Qt::AlignCenter);

        auto *titleLabel = new QLabel(title, page);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setProperty("title", true);

        auto *descLabel = new QLabel(desc, page);
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setProperty("description", true);

        layout->addWidget(titleLabel);
        layout->addWidget(descLabel);
        return page;
    };

    mPages.append(mMainMeaPage);
    mPages.append(mExperimentViewPage);
    mPages.append(createPage(QObject::tr("记录"), QObject::tr("历史记录与回放")));
    mPages.append(createPage(QObject::tr("状态"), QObject::tr("当前运行状态详情")));

    for (int i = 0; i < mPages.size(); ++i) {
        auto *page = mPages.at(i);
        page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mContentLayout->addWidget(page);
    }
    mContentLayout->setCurrentIndex(0);

    mLayout->addWidget(mSideTabBar);
    mLayout->addWidget(mContentWidget);
    mLayout->setStretch(1, 1);
}
