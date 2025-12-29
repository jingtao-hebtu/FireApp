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
#include "HKCamTypes.h"
#include <QDialog>
#include <QDebug>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPoint>
#include <QRect>
#include <QPushButton>
#include <QColor>
#include <QSizePolicy>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QTimer>
#include <optional>
#include <QtConcurrent/QtConcurrent>


namespace {
    constexpr const char *kHKCamEndpoint = "tcp://127.0.0.1:5555";
    constexpr int kHKCamTimeoutMs = 3000;
    constexpr int kHKCamRetries = 3;
    constexpr int kHKCamConnectUiTimeoutMs = 8000;
}


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
    updateConnectionStatus();
    updateControlsEnabled();
}

void TF::CamConfigWid::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(createConnectionPanel());
    mainLayout->addWidget(createInfoPanel());

    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    mainLayout->addWidget(createButtonPanel());

    mAdjustTimer = new QTimer(this);
    mAdjustTimer->setInterval(150);
    connect(mAdjustTimer, &QTimer::timeout, this, [this]() { applyAdjustment(mCurrentAction); });
}

QWidget *TF::CamConfigWid::createConnectionPanel() {
    auto *panel = new QWidget(this);
    auto *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto *labelTitle = new QLabel(tr("连接状态"), panel);
    labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    mConnectionStatusLabel = new QLabel(tr("未连接"), panel);
    mConnectionStatusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *connectBtn = new TechActionButton(tr("连接"), panel);
    connect(connectBtn, &QPushButton::clicked, this, &CamConfigWid::onConnectClicked);

    layout->addWidget(labelTitle);
    layout->addWidget(mConnectionStatusLabel, 1);
    layout->addWidget(connectBtn);

    return panel;
}

QWidget *TF::CamConfigWid::createInfoPanel() {
    auto *panel = new QWidget(this);
    auto *grid = new QGridLayout(panel);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);

    auto addRow = [this, grid](int row, const QString &title,
                               QLabel **minLabel, QLabel **currentLabel, QLabel **maxLabel) {
        auto *titleLabel = new QLabel(title, this);
        titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        titleLabel->setMinimumWidth(52);

        auto createValueLabel = [this]() {
            auto *lbl = new QLabel("-", this);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setMinimumHeight(32);
            lbl->setStyleSheet("background: #11141c; border: 1px solid #2f3545; border-radius: 8px; padding: 6px; font-weight: 600;");
            return lbl;
        };

        *minLabel = createValueLabel();
        *currentLabel = createValueLabel();
        *maxLabel = createValueLabel();

        grid->addWidget(titleLabel, row, 0);
        grid->addWidget(*minLabel, row, 1);
        grid->addWidget(*currentLabel, row, 2);
        grid->addWidget(*maxLabel, row, 3);
    };

    addRow(0, tr("焦距"), &mFocusMinLabel, &mFocusCurrentLabel, &mFocusMaxLabel);
    addRow(1, tr("曝光"), &mExposureMinLabel, &mExposureCurrentLabel, &mExposureMaxLabel);

    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);
    grid->setColumnStretch(3, 1);

    return panel;
}

QWidget *TF::CamConfigWid::createButtonPanel() {
    auto *panel = new QWidget(this);
    auto *layout = new QGridLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

    auto createActionButton = [panel](const QString &text) {
        auto *btn = new TF::TechActionButton(text, panel);
        btn->setMinimumSize(90, 44);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return btn;
    };

    mFocusIncBtn = createActionButton(tr("焦距 +"));
    mFocusDecBtn = createActionButton(tr("焦距 -"));
    mFocusResetBtn = createActionButton(tr("默认焦距"));
    mExposureIncBtn = createActionButton(tr("曝光 +"));
    mExposureDecBtn = createActionButton(tr("曝光 -"));
    mExposureAutoBtn = createActionButton(tr("自动曝光"));

    layout->addWidget(mFocusIncBtn, 0, 0);
    layout->addWidget(mFocusDecBtn, 0, 1);
    layout->addWidget(mFocusResetBtn, 0, 2);
    layout->addWidget(mExposureIncBtn, 1, 0);
    layout->addWidget(mExposureDecBtn, 1, 1);
    layout->addWidget(mExposureAutoBtn, 1, 2);

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);

    connect(mFocusIncBtn, &QPushButton::pressed, this, &CamConfigWid::onFocusIncrease);
    connect(mFocusIncBtn, &QPushButton::released, this, &CamConfigWid::stopContinuousAdjust);
    connect(mFocusDecBtn, &QPushButton::pressed, this, &CamConfigWid::onFocusDecrease);
    connect(mFocusDecBtn, &QPushButton::released, this, &CamConfigWid::stopContinuousAdjust);
    connect(mFocusResetBtn, &QPushButton::clicked, this, &CamConfigWid::onFocusReset);

    connect(mExposureIncBtn, &QPushButton::pressed, this, &CamConfigWid::onExposureIncrease);
    connect(mExposureIncBtn, &QPushButton::released, this, &CamConfigWid::stopContinuousAdjust);
    connect(mExposureDecBtn, &QPushButton::pressed, this, &CamConfigWid::onExposureDecrease);
    connect(mExposureDecBtn, &QPushButton::released, this, &CamConfigWid::stopContinuousAdjust);
    connect(mExposureAutoBtn, &QPushButton::clicked, this, &CamConfigWid::onExposureAuto);

    return panel;
}

void TF::CamConfigWid::updateInfoDisplay() {
    const auto focusMin = mapToDisplay(mFocusMinNormalized, mFocusDisplayMin, mFocusDisplayMax);
    const auto focusMax = mapToDisplay(mFocusMaxNormalized, mFocusDisplayMin, mFocusDisplayMax);
    const auto focusCurrent = mapToDisplay(mFocusValue, mFocusDisplayMin, mFocusDisplayMax);

    const auto exposureMin = mapToDisplay(mExposureMinNormalized, mExposureDisplayMin, mExposureDisplayMax);
    const auto exposureMax = mapToDisplay(mExposureMaxNormalized, mExposureDisplayMin, mExposureDisplayMax);
    const auto exposureCurrent = mapToDisplay(mExposureValue, mExposureDisplayMin, mExposureDisplayMax);

    if (mFocusMinLabel) mFocusMinLabel->setText(QString::number(focusMin, 'f', 2));
    if (mFocusMaxLabel) mFocusMaxLabel->setText(QString::number(focusMax, 'f', 2));
    if (mFocusCurrentLabel) mFocusCurrentLabel->setText(QString::number(focusCurrent, 'f', 2));

    if (mExposureMinLabel) mExposureMinLabel->setText(QString::number(exposureMin, 'f', 2));
    if (mExposureMaxLabel) mExposureMaxLabel->setText(QString::number(exposureMax, 'f', 2));
    if (mExposureCurrentLabel) mExposureCurrentLabel->setText(QString::number(exposureCurrent, 'f', 2));
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
    ensureConnected();
}

void TF::CamConfigWid::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
}

void TF::CamConfigWid::updateConnectionStatus() {
    if (!mConnectionStatusLabel) {
        return;
    }
    if (mConnected) {
        mConnectionStatusLabel->setText(tr("已连接"));
        mConnectionStatusLabel->setStyleSheet("color: #4caf50; font-weight: bold;");
    } else if (mConnecting) {
        mConnectionStatusLabel->setText(tr("连接中..."));
        mConnectionStatusLabel->setStyleSheet("color: #ffc107; font-weight: bold;");
    } else {
        mConnectionStatusLabel->setText(tr("未连接"));
        mConnectionStatusLabel->setStyleSheet("color: #f44336; font-weight: bold;");
    }
}

void TF::CamConfigWid::updateControlsEnabled() {
    const bool enableControls = mConnected && !mConnecting;
    const std::initializer_list<TechActionButton **> buttons = {
        &mFocusIncBtn,
        &mFocusDecBtn,
        &mFocusResetBtn,
        &mExposureIncBtn,
        &mExposureDecBtn,
        &mExposureAutoBtn
    };

    for (auto btnPtr : buttons) {
        if (btnPtr && *btnPtr) {
            (*btnPtr)->setEnabled(enableControls);
        }
    }
}

bool TF::CamConfigWid::ensureConnected() {
    if (mConnected) {
        return true;
    }

    if (mConnecting) {
        return false;
    }

    startConnectionAttempt();
    return mConnected;
}

void TF::CamConfigWid::startConnectionAttempt() {
    mConnecting = true;
    mConnectTimedOut = false;
    updateConnectionStatus();
    updateControlsEnabled();

    if (!mConnectDialog) {
        mConnectDialog = new QDialog(this);
        mConnectDialog->setModal(true);
        mConnectDialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        mConnectDialog->setStyleSheet(
        "QDialog {"
        "  background-color: #333333;"
        "  border: 1px solid #555555;"
        "  border-radius: 8px;"
        "}"
        "QLabel {"
        "  color: white;"
        "}"
    );

        auto *dialogLayout = new QVBoxLayout(mConnectDialog);
        dialogLayout->setContentsMargins(20, 20, 20, 20);
        dialogLayout->setSpacing(12);
        mConnectDialogLabel = new QLabel(tr("正在连接..."), mConnectDialog);
        mConnectDialogLabel->setAlignment(Qt::AlignCenter);
        dialogLayout->addWidget(mConnectDialogLabel);
    }

    if (!mConnectDialogTimer) {
        mConnectDialogTimer = new QTimer(this);
        mConnectDialogTimer->setInterval(200);
        connect(mConnectDialogTimer, &QTimer::timeout, this, &CamConfigWid::updateConnectionProgress);
    }

    if (mConnectWatcher) {
        mConnectWatcher->deleteLater();
        mConnectWatcher = nullptr;
    }

    mConnectElapsed.restart();
    mConnectDialogTimer->start();
    mConnectDialog->show();

    auto future = QtConcurrent::run([]() {
        ConnectResult result;
        std::string error;
        auto &client = HKCamZmqClient::instance();
        result.success = client.Connect(kHKCamEndpoint, kHKCamTimeoutMs, kHKCamRetries, error);
        result.error = error;
        return result;
    });

    mConnectWatcher = new QFutureWatcher<ConnectResult>(this);
    connect(mConnectWatcher, &QFutureWatcher<ConnectResult>::finished, this, &CamConfigWid::handleConnectFinished);
    mConnectWatcher->setFuture(future);
}

void TF::CamConfigWid::updateConnectionProgress() {
    const double seconds = static_cast<double>(mConnectElapsed.elapsed()) / 1000.0;
    if (mConnectDialogLabel) {
        mConnectDialogLabel->setText(tr("正在连接... %1s").arg(QString::number(seconds, 'f', 1)));
    }

    if (mConnecting && mConnectElapsed.elapsed() >= kHKCamConnectUiTimeoutMs) {
        handleConnectTimeout();
    }
}

void TF::CamConfigWid::handleConnectFinished() {
    if (!mConnectWatcher) {
        return;
    }

    if (mConnectTimedOut) {
        mConnectWatcher->deleteLater();
        mConnectWatcher = nullptr;
        cleanupConnectUi();
        return;
    }

    const auto result = mConnectWatcher->result();
    mConnectWatcher->deleteLater();
    mConnectWatcher = nullptr;

    mConnecting = false;
    mConnected = result.success;
    if (!mConnected) {
        qWarning("HKCam connect failed: %s", result.error.c_str());
    }

    cleanupConnectUi();
    updateConnectionStatus();
    updateControlsEnabled();
    if (mConnected) {
        loadRanges();
        refreshCurrentValues();
    } else {
        QMessageBox::warning(this, tr("连接失败"), tr("未连接上，请检查相机服务是否开启。"));
    }
}

void TF::CamConfigWid::handleConnectTimeout() {
    if (!mConnecting || mConnectTimedOut) {
        return;
    }

    mConnectTimedOut = true;
    mConnecting = false;
    mConnected = false;
    if (mConnectWatcher && mConnectWatcher->isRunning()) {
        mConnectWatcher->cancel();
    }

    cleanupConnectUi();
    updateConnectionStatus();
    updateControlsEnabled();
    QMessageBox::warning(this, tr("连接超时"), tr("未连接上，请检查相机服务是否开启。"));
}

void TF::CamConfigWid::cleanupConnectUi() {
    if (mConnectDialogTimer) {
        mConnectDialogTimer->stop();
    }
    if (mConnectDialog) {
        mConnectDialog->hide();
    }
}

void TF::CamConfigWid::loadRanges() {
    if (mRangesLoaded || !mConnected) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    std::string error;
    QJsonObject range;
    if (client.GetZoomRange(range, error)) {
        mFocusMinNormalized = readDoubleFromKeys(range, {"min", "min_zoom", "zoom_min"}, 0.0);
        mFocusMaxNormalized = readDoubleFromKeys(range, {"max", "max_zoom", "zoom_max"}, 1.0);
    } else {
        qWarning("GetZoomRange failed: %s", error.c_str());
    }

    QJsonObject exposureRange;
    if (client.GetExposureRange(exposureRange, error)) {
        mExposureMinNormalized = readDoubleFromKeys(exposureRange, {"min", "min_exposure", "exposure_min"}, 0.0);
        mExposureMaxNormalized = readDoubleFromKeys(exposureRange, {"max", "max_exposure", "exposure_max"}, 1.0);
    } else {
        qWarning("GetExposureRange failed: %s", error.c_str());
    }

    mRangesLoaded = true;
    updateInfoDisplay();
}

void TF::CamConfigWid::refreshCurrentValues() {
    refreshFocus();
    refreshExposure();
    updateInfoDisplay();
}

void TF::CamConfigWid::refreshFocus() {
    if (!mConnected) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    std::string error;
    QJsonObject raw;
    double zoom = 0.0;
    if (client.GetZoom(zoom, raw, error)) {
        mFocusValue = clampNormalized(zoom, mFocusMinNormalized, mFocusMaxNormalized);
    } else {
        qWarning("GetZoom failed: %s", error.c_str());
    }
}

void TF::CamConfigWid::refreshExposure() {
    if (!mConnected) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    std::string error;
    QJsonObject exposureObj;
    if (client.GetExposure(exposureObj, error)) {
        const auto exposure = readDoubleFromKeys(exposureObj, {"value", "exposure", "exposure_s", "exposure_us"}, mExposureValue);
        mExposureValue = clampNormalized(exposure, mExposureMinNormalized, mExposureMaxNormalized);
    } else {
        qWarning("GetExposure failed: %s", error.c_str());
    }
}

void TF::CamConfigWid::startContinuousAdjust(int action) {
    mCurrentAction = action;
    applyAdjustment(action);
    mAdjustTimer->start();
}

void TF::CamConfigWid::stopContinuousAdjust() {
    if (mAdjustTimer) {
        mAdjustTimer->stop();
    }
    mCurrentAction = -1;
}

void TF::CamConfigWid::applyAdjustment(int action) {
    if (mApplyingAdjustment || action == -1) {
        return;
    }
    mApplyingAdjustment = true;

    constexpr double kFocusStep = 0.02;
    constexpr double kExposureStep = 0.02;

    switch (action) {
        case FocusIncreaseAction:
            adjustFocus(kFocusStep);
            break;
        case FocusDecreaseAction:
            adjustFocus(-kFocusStep);
            break;
        case ExposureIncreaseAction:
            adjustExposure(kExposureStep);
            break;
        case ExposureDecreaseAction:
            adjustExposure(-kExposureStep);
            break;
        default:
            break;
    }

    mApplyingAdjustment = false;
}

void TF::CamConfigWid::adjustFocus(double delta) {
    if (!ensureConnected()) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    std::string error;
    QJsonObject raw;
    if (client.ZoomStep(delta, std::nullopt, raw, error)) {
        const auto zoom = readDoubleFromKeys(raw, {"value", "zoom"}, mFocusValue + delta);
        mFocusValue = clampNormalized(zoom, mFocusMinNormalized, mFocusMaxNormalized);
    } else {
        qWarning("ZoomStep failed: %s", error.c_str());
    }
    updateInfoDisplay();
}

void TF::CamConfigWid::setFocusNormalized(double value) {
    if (!ensureConnected()) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    const double target = clampNormalized(value, mFocusMinNormalized, mFocusMaxNormalized);
    std::string error;
    QJsonObject raw;
    if (client.SetZoomAbs(target, std::nullopt, raw, error)) {
        const auto zoom = readDoubleFromKeys(raw, {"value", "zoom"}, target);
        mFocusValue = clampNormalized(zoom, mFocusMinNormalized, mFocusMaxNormalized);
    } else {
        qWarning("SetZoomAbs failed: %s", error.c_str());
    }
    updateInfoDisplay();
}

void TF::CamConfigWid::adjustExposure(double delta) {
    if (!ensureConnected()) {
        return;
    }

    const double target = clampNormalized(mExposureValue + delta, mExposureMinNormalized, mExposureMaxNormalized);
    setExposureNormalized(target);
}

void TF::CamConfigWid::setExposureNormalized(double value) {
    if (!ensureConnected()) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    const double target = clampNormalized(value, mExposureMinNormalized, mExposureMaxNormalized);
    const double exposureSeconds = mapToDisplay(target, mExposureDisplayMin, mExposureDisplayMax);

    std::string error;
    QJsonObject raw;
    if (client.SetExposureBySeconds(exposureSeconds, true, true, raw, error)) {
        const auto exposure = readDoubleFromKeys(raw, {"value", "exposure", "exposure_s"}, target);
        mExposureValue = clampNormalized(exposure, mExposureMinNormalized, mExposureMaxNormalized);
    } else {
        qWarning("SetExposureBySeconds failed: %s", error.c_str());
    }
    updateInfoDisplay();
}

void TF::CamConfigWid::setExposureAutoInternal() {
    if (!ensureConnected()) {
        return;
    }

    auto &client = HKCamZmqClient::instance();
    std::string error;
    QJsonObject raw;
    if (!client.SetExposureByShutter("auto", true, true, raw, error)) {
        qWarning("SetExposure auto failed: %s", error.c_str());
    } else {
        refreshExposure();
        updateInfoDisplay();
    }
}

double TF::CamConfigWid::mapToDisplay(double normalized, double min, double max) const {
    return min + (max - min) * normalized;
}

double TF::CamConfigWid::clampNormalized(double value, double min, double max) const {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

double TF::CamConfigWid::readDoubleFromKeys(const QJsonObject &obj, const std::initializer_list<QString> &keys, double defaultValue) const {
    for (const auto &key : keys) {
        if (obj.contains(key)) {
            return obj.value(key).toDouble(defaultValue);
        }
    }
    return defaultValue;
}

void TF::CamConfigWid::onFocusIncrease() {
    startContinuousAdjust(FocusIncreaseAction);
}

void TF::CamConfigWid::onFocusDecrease() {
    startContinuousAdjust(FocusDecreaseAction);
}

void TF::CamConfigWid::onFocusReset() {
    stopContinuousAdjust();
    setFocusNormalized(mFocusMinNormalized);
}

void TF::CamConfigWid::onExposureIncrease() {
    startContinuousAdjust(ExposureIncreaseAction);
}

void TF::CamConfigWid::onExposureDecrease() {
    startContinuousAdjust(ExposureDecreaseAction);
}

void TF::CamConfigWid::onExposureAuto() {
    stopContinuousAdjust();
    setExposureAutoInternal();
}


