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
#include "ExpInfoDialog.h"
#include "ExperimentParamManager.h"
#include <QApplication>
#include <QFile>
#include <QCoreApplication>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QLayout>
#include <QSizePolicy>
#include <algorithm>
#include <vector>


TF::FuMainWid::FuMainWid(QWidget* parent) : QWidget(parent) {
    setupUi();
    initStyle();
    initActions();
    startHKCamPythonServer();

    setupConnections();
}

TF::FuMainWid::~FuMainWid() {
    stopHKCamPythonServer();
    delete mUi;
}

void TF::FuMainWid::initAfterDisplay() {
    mUi->mMainMeaPage->initAfterDisplay();
}

namespace {

    QString policyToString(QSizePolicy::Policy policy) {
        switch (policy) {
            case QSizePolicy::Fixed: return QStringLiteral("Fixed");
            case QSizePolicy::Minimum: return QStringLiteral("Minimum");
            case QSizePolicy::Maximum: return QStringLiteral("Maximum");
            case QSizePolicy::Preferred: return QStringLiteral("Preferred");
            case QSizePolicy::Expanding: return QStringLiteral("Expanding");
            case QSizePolicy::MinimumExpanding: return QStringLiteral("MinimumExpanding");
            case QSizePolicy::Ignored: return QStringLiteral("Ignored");
        }
        return QStringLiteral("Unknown");
    }

    int widgetMinWidthContribution(const QWidget *widget) {
        if (widget == nullptr) {
            return 0;
        }
        const auto hint = widget->minimumSizeHint();
        const auto minSize = widget->minimumSize();
        const auto sizeHint = widget->sizeHint();

        return std::max({hint.width(), minSize.width(), sizeHint.width()});
    }

}

void TF::FuMainWid::dumpLayoutDiagnostics() const {
    const QScreen *screen = this->screen();
    if (screen == nullptr) {
        screen = QGuiApplication::primaryScreen();
    }

    if (screen) {
        qInfo().noquote() << "[Screen] size:" << screen->size()
                          << "available:" << screen->availableGeometry()
                          << "logicalDPI:" << screen->logicalDotsPerInch()
                          << "physicalDPI:" << screen->physicalDotsPerInch();
    } else {
        qInfo() << "[Screen] unavailable";
    }

    qInfo().noquote() << "[Window] minSize:" << minimumSize()
                      << "minHint:" << minimumSizeHint()
                      << "maxSize:" << maximumSize();

    if (layout()) {
        qInfo().noquote() << "[Layout] sizeConstraint:" << layout()->sizeConstraint()
                          << "minHint:" << layout()->minimumSize();
    } else {
        qInfo() << "[Layout] none";
    }

    const auto widgetsList = findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
    std::vector<std::pair<int, QWidget*>> contributions;
    contributions.reserve(widgetsList.size());
    for (auto *widget : widgetsList) {
        contributions.emplace_back(widgetMinWidthContribution(widget), widget);
    }

    std::sort(contributions.begin(), contributions.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.first > rhs.first;
    });

    const int limit = std::min<int>(20, contributions.size());
    qInfo() << "[Widgets] Top" << limit << "by minimum width contribution:";
    for (int i = 0; i < limit; ++i) {
        const auto contrib = contributions.at(i).first;
        const auto *widget = contributions.at(i).second;
        const auto minSize = widget->minimumSize();
        const auto minHint = widget->minimumSizeHint();
        const auto sizeHint = widget->sizeHint();
        const auto policy = widget->sizePolicy();

        qInfo().noquote() << QString("  [%1] %2 | min:%3x%4 minHint:%5x%6 sizeHint:%7x%8 | policy:%9/%10 HFW:%11 | contrib:%12")
                             .arg(widget->objectName().isEmpty() ? QStringLiteral("<no object>") : widget->objectName())
                             .arg(widget->metaObject()->className())
                             .arg(minSize.width()).arg(minSize.height())
                             .arg(minHint.width()).arg(minHint.height())
                             .arg(sizeHint.width()).arg(sizeHint.height())
                             .arg(policyToString(policy.horizontalPolicy()))
                             .arg(policyToString(policy.verticalPolicy()))
                             .arg(policy.hasHeightForWidth())
                             .arg(contrib);
    }
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

        mUi->mContentLayout->setCurrentIndex(index);
    });

    connect(mUi->mSideTabBar, &FuSideTabBar::cameraConfigRequested,
            mUi->mMainMeaPage, &FuMainMeaPage::onCamConfigBtnPressed);

    connect(mUi->mSideTabBar, &FuSideTabBar::newExperimentRequested, this, [this]() {
        ExpInfoDialog dialog(this);
        dialog.setExperimentName(mUi->mMainMeaPage->experimentName());
        while (dialog.exec() == QDialog::Accepted) {
            const auto name = dialog.experimentName();
            if (ExperimentParamManager::instance().experimentNameExists(name)) {
                QMessageBox::warning(this, tr("提示"), tr("实验已存在，请重新输入"));
                continue;
            }

            mUi->mMainMeaPage->setExperimentName(name);
            break;
        }
    });

    mUi->mSideTabBar->setCurrentIndex(0);
}

namespace {
    TF::HKCamServerConfig DefaultHKCamServerConfig()
    {
        TF::HKCamServerConfig cfg;
        cfg.pythonExe = "D:\\Software\\anaconda3\\envs\\fire_onvif\\python.exe";
        cfg.scriptPath = "E:\\Project\\Fire\\FireSensors\\HKCam\\FireOnvif\\app.py ";
        cfg.cameraIp = "192.168.6.66";
        cfg.cameraPort = 80;
        cfg.username = "admin";
        cfg.password = "fireAi1A";
        cfg.profileIndex = 0;
        cfg.endpoint = "tcp://127.0.0.1:5555";
        cfg.timeoutMs = 4000;
        cfg.startRetries = 20;
        cfg.startPollIntervalMs = 200;
        cfg.createNoWindow = true;
        cfg.allowStopExternal = false;
        return cfg;
    }
}

void TF::FuMainWid::startHKCamPythonServer()
{
    std::string error;
    if (!mCamPythonServer.StartBlocking(DefaultHKCamServerConfig(), error))
    {
        qWarning().noquote() << "Failed to start HKCam python server:" << QString::fromStdString(error);
    }
}

void TF::FuMainWid::stopHKCamPythonServer()
{
    mCamPythonServer.Stop();
}
