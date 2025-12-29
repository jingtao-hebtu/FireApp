#include "FuMainMeaPage_Ui.h"
#include "videowidgetx.h"
#include "FuVideoButtons.h"
#include "ThermalCamera.h"
#include "ThermalWidget.h"
#include "TSweepCurveViewer.h"
#include "TFMeaManager.h"
#include "ThermalManager.h"
#include "TConfig.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QWidget>
#include <QString>
#include <QFrame>
#include <QProgressBar>
#include <QStyle>
#include <QSpacerItem>
#include <QLabel>


void TF::FuMainMeaPage_Ui::setupUi(QWidget* wid) {
    mWid = wid;

    if (mWid->objectName().isEmpty())
        mWid->setObjectName("mWid");

    auto height = GET_INT_CONFIG("MainUi", "Height");
    auto width = GET_INT_CONFIG("MainUi", "Width");
    mWid->resize(width, height);
    mMainVLayout = new QVBoxLayout(mWid);
    mMainVLayout->setSpacing(6);
    mMainVLayout->setObjectName("MainVLayout");
    mMainVLayout->setContentsMargins(6, 6, 6, 6);

    initVideoArea();
    initCtrlArea();
}

void TF::FuMainMeaPage_Ui::initVideoArea() {
    mVideoArea = new QWidget(mWid);
    mVideoArea->setObjectName("VideoArea");
    mVideoHLayout = new QHBoxLayout(mVideoArea);
    mVideoHLayout->setSpacing(6);
    mVideoHLayout->setContentsMargins(0, 0, 0, 0);

    mVideoWid = new VideoWidget(mVideoArea);
    mVideoWid->setObjectName("VideoWid");
    mVideoWid->resize(780, 440);
    QSizePolicy mainVideoPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainVideoPolicy.setHorizontalStretch(mMainVideoStretch);
    mVideoWid->setSizePolicy(mainVideoPolicy);

    mVideoSideWid = new QWidget(mVideoArea);
    mVideoSideWid->setObjectName("VideoSideWid");
    mVideoSideVLayout = new QVBoxLayout(mVideoSideWid);
    mVideoSideVLayout->setSpacing(6);
    mVideoSideVLayout->setContentsMargins(0, 0, 0, 0);

    initThermalCamera();
    initStatistics();

    mVideoSideVLayout->addWidget(mThermalWid, 1);
    mVideoSideVLayout->addWidget(mStatisticsWid, 1);

    mVideoHLayout->addWidget(mVideoWid);
    mVideoHLayout->addWidget(mVideoSideWid);
    setVideoAreaStretch(mMainVideoStretch, mSideColumnStretch);

    mMainVLayout->addWidget(mVideoArea);
}

void TF::FuMainMeaPage_Ui::initThermalCamera() {
    mThermalWid = new QWidget(mVideoSideWid);
    mThermalWid->setObjectName("InfraredVideoWid");
    mThermalWid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto *thermalLayout = new QVBoxLayout(mThermalWid);
    thermalLayout->setContentsMargins(12, 12, 12, 12);
    thermalLayout->setSpacing(8);

    auto *thermalTitle = new QLabel(QCoreApplication::translate("Page", "红外模组"), mThermalWid);
    thermalTitle->setObjectName("PanelTitle");
    thermalTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *thermalFrame = new QFrame(mThermalWid);
    thermalFrame->setObjectName("InfraredFrame");
    auto *thermalFrameLayout = new QVBoxLayout(thermalFrame);
    thermalFrameLayout->setContentsMargins(6, 6, 6, 6);
    thermalFrameLayout->setSpacing(4);

    mThermalCamera = new ThermalCamera(thermalFrame);
    mThermalWidget = new ThermalWidget(mThermalCamera, thermalFrame);
    mThermalWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ThermalManager::instance().setThermalCamera(mThermalCamera);

    thermalFrameLayout->addWidget(mThermalWidget);

    thermalLayout->addWidget(thermalTitle);
    thermalLayout->addWidget(thermalFrame);
    thermalLayout->setStretchFactor(thermalFrame, 1);
}

void TF::FuMainMeaPage_Ui::initStatistics() {
    mStatisticsWid = new QWidget(mVideoSideWid);
    mStatisticsWid->setObjectName("StatisticsWid");
    QSizePolicy statisticsPolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    statisticsPolicy.setHorizontalStretch(mSideColumnStretch);
    mStatisticsWid->setSizePolicy(statisticsPolicy);

    mStatisticsVLayout = new QVBoxLayout(mStatisticsWid);
    mStatisticsVLayout->setSpacing(8);
    mStatisticsVLayout->setContentsMargins(12, 12, 12, 12);

    auto *statisticsTitle = new QLabel(QCoreApplication::translate("Page", "统计曲线"), mStatisticsWid);
    statisticsTitle->setObjectName("PanelTitle");
    statisticsTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *statisticsFrame = new QFrame(mStatisticsWid);
    statisticsFrame->setObjectName("StatisticsFrame");
    auto *statisticsFrameLayout = new QVBoxLayout(statisticsFrame);
    statisticsFrameLayout->setSpacing(6);
    statisticsFrameLayout->setContentsMargins(8, 8, 8, 8);

    mFireHeightCurveViewer = createCurveViewer("火焰高度", 0, 100, 0, 1000);
    mFireAreaCurveViewer = createCurveViewer("火焰面积", 0, 100, 0, 1E6);

    statisticsFrameLayout->addWidget(mFireHeightCurveViewer);
    statisticsFrameLayout->addWidget(mFireAreaCurveViewer);
    statisticsFrameLayout->setStretch(0, 1);
    statisticsFrameLayout->setStretch(1, 1);

    mStatisticsVLayout->addWidget(statisticsTitle);
    mStatisticsVLayout->addWidget(statisticsFrame);
    mStatisticsVLayout->setStretchFactor(statisticsFrame, 1);

    TFMeaManager::instance().setHeightCurveViewer(mFireHeightCurveViewer);
    TFMeaManager::instance().setAreaCurveViewer(mFireAreaCurveViewer);
}

T_QtBase::TSweepCurveViewer* TF::FuMainMeaPage_Ui::createCurveViewer(const QString &name, int x_min, int x_max, int y_min, int y_max) {
    auto *viewer = new T_QtBase::TSweepCurveViewer();
    viewer->setObjectName(name + "CurveViewer");
    viewer->updateXAxisRange(x_min, x_max);
    viewer->updateYAxisRange(y_min, y_max);

    return viewer;
}

void TF::FuMainMeaPage_Ui::initCtrlArea() {
    mCtrlWid = new QWidget(mWid);
    mCtrlWid->setObjectName("CtrlWid");
    QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mCtrlWid->sizePolicy().hasHeightForWidth());
    mCtrlWid->setSizePolicy(sizePolicy);
    mMainVLayout->addWidget(mCtrlWid);

    mCtrlHLayout = new QHBoxLayout(mCtrlWid);
    mCtrlHLayout->setSpacing(6);
    mCtrlHLayout->setObjectName("CtrlHLayout");
    mCtrlHLayout->setContentsMargins(0, 0, 0, 0);

    mMainCamToggleBtn = new TechToggleButton("主相机", mWid);
    mMainCamToggleBtn->setObjectName("StartStopStream");

    mThermalCamToggleBtn = new TechToggleButton("红外模块", mWid);
    mThermalCamToggleBtn->setObjectName("StartStopThermal");

    mAiToggleBtn = new TechToggleButton("AI检测", mWid);
    mAiToggleBtn->setObjectName("StartStopAi");

    mAiSaveToggleBtn = new TechToggleButton("保存AI结果", mWid);
    mAiSaveToggleBtn->setObjectName("StartStopAiSave");

    mSaveToggleBtn = new TechToggleButton("保存视频", mWid);
    mSaveToggleBtn->setObjectName("StartStopSave");

    mCamConfigBtn = new TechActionButton("相机配置", mWid);
    mCamConfigBtn->setObjectName("CamConfig");

    const int controlButtonHeight = 66;
    mMainCamToggleBtn->setFixedHeight(controlButtonHeight);
    mThermalCamToggleBtn->setFixedHeight(controlButtonHeight);
    mAiToggleBtn->setFixedHeight(controlButtonHeight);
    mAiSaveToggleBtn->setFixedHeight(controlButtonHeight);
    mSaveToggleBtn->setFixedHeight(controlButtonHeight);
    mCamConfigBtn->setFixedHeight(controlButtonHeight);

    mCtrlHLayout->addWidget(mMainCamToggleBtn);
    mCtrlHLayout->addWidget(mThermalCamToggleBtn);
    mCtrlHLayout->addWidget(mAiToggleBtn);
    mCtrlHLayout->addWidget(mAiSaveToggleBtn);
    mCtrlHLayout->addWidget(mSaveToggleBtn);
    mCtrlHLayout->addWidget(mCamConfigBtn);
    mCtrlHLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    mMetricsPanel = new QFrame(mWid);
    mMetricsPanel->setObjectName("MetricsPanel");
    mMetricsPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mMetricsPanel->setMinimumWidth(140);
    mMetricsPanel->setFixedHeight(66);

    auto *metricsLayout = new QHBoxLayout(mMetricsPanel);
    metricsLayout->setContentsMargins(12, 6, 12, 6);
    metricsLayout->setSpacing(10);

    auto createMetricWidget = [&](const QString& title, QLabel** valueLabel) {
        auto *item = new QWidget(mMetricsPanel);
        item->setObjectName("MetricItem");
        auto *itemLayout = new QVBoxLayout(item);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(2);

        auto *titleLabel = new QLabel(title, item);
        titleLabel->setObjectName("MetricTitle");
        titleLabel->setAlignment(Qt::AlignCenter);

        auto *value = new QLabel(QCoreApplication::translate("Page", "---"), item);
        value->setObjectName("MetricValue");
        value->setAlignment(Qt::AlignCenter);

        itemLayout->addWidget(titleLabel);
        itemLayout->addWidget(value);

        *valueLabel = value;
        return item;
    };

    metricsLayout->addWidget(createMetricWidget(QCoreApplication::translate("Page", "距离 (m)"), &mDistValueLabel));

    auto *metricDivider = new QFrame(mMetricsPanel);
    metricDivider->setObjectName("MetricDivider");
    metricDivider->setFrameShape(QFrame::VLine);
    metricDivider->setFrameShadow(QFrame::Plain);
    metricsLayout->addWidget(metricDivider);

    metricsLayout->addWidget(createMetricWidget(QCoreApplication::translate("Page", "倾角 (°)"), &mTiltAngleValueLabel));

    mCtrlHLayout->addWidget(mMetricsPanel);
    mCtrlHLayout->setAlignment(mMetricsPanel, Qt::AlignVCenter);

    initBatteryInfoArea();
}

void TF::FuMainMeaPage_Ui::initBatteryInfoArea() {
    mBatteryPanel = new QFrame(mWid);
    mBatteryPanel->setObjectName("BatteryPanel");
    mBatteryPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mBatteryPanel->setFixedHeight(66);
    mBatteryPanel->setMinimumWidth(200);
    mBatteryPanel->setMaximumWidth(320);

    auto *batteryLayout = new QHBoxLayout(mBatteryPanel);
    batteryLayout->setObjectName("BatteryLayout");
    batteryLayout->setContentsMargins(10, 6, 10, 6);
    batteryLayout->setSpacing(10);

    auto *batteryColumn = new QVBoxLayout();
    batteryColumn->setContentsMargins(0, 0, 0, 0);
    batteryColumn->setSpacing(4);

    mBatteryLevelBar = new QProgressBar(mBatteryPanel);
    mBatteryLevelBar->setObjectName("BatteryBar");
    mBatteryLevelBar->setRange(0, 100);
    mBatteryLevelBar->setFormat("%p%");
    mBatteryLevelBar->setAlignment(Qt::AlignCenter);
    mBatteryLevelBar->setTextVisible(true);
    mBatteryLevelBar->setFixedSize(130, 22);

    mBatteryStatusLabel = new QLabel(QCoreApplication::translate("Page", "电量 --% · 未连接"), mBatteryPanel);
    mBatteryStatusLabel->setObjectName("BatteryStatusLabel");
    mBatteryStatusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    batteryColumn->addWidget(mBatteryLevelBar);
    batteryColumn->addWidget(mBatteryStatusLabel);

    batteryLayout->addLayout(batteryColumn);

    mChargeInfoWidget = new QWidget(mBatteryPanel);
    auto *chargeLayout = new QVBoxLayout(mChargeInfoWidget);
    chargeLayout->setContentsMargins(0, 0, 0, 0);
    chargeLayout->setSpacing(2);

    mBatteryChargeIcon = new QLabel(QString::fromUtf8("⚡"), mChargeInfoWidget);
    mBatteryChargeIcon->setObjectName("BatteryChargeIcon");
    mBatteryChargeIcon->setAlignment(Qt::AlignCenter);

    mBatteryChargeCurrent = new QLabel(QCoreApplication::translate("Page", "1.8 A"), mChargeInfoWidget);
    mBatteryChargeCurrent->setObjectName("BatteryChargeCurrent");
    mBatteryChargeCurrent->setAlignment(Qt::AlignCenter);

    chargeLayout->addWidget(mBatteryChargeIcon);
    chargeLayout->addWidget(mBatteryChargeCurrent);

    batteryLayout->addWidget(mChargeInfoWidget);

    mBatteryTempLabel = new QLabel(QCoreApplication::translate("Page", "温度 36.5°C"), mBatteryPanel);
    mBatteryTempLabel->setObjectName("BatteryTempLabel");
    mBatteryTempLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    batteryLayout->addWidget(mBatteryTempLabel);
    batteryLayout->setAlignment(mBatteryTempLabel, Qt::AlignVCenter);

    mCtrlHLayout->addWidget(mBatteryPanel);
    mCtrlHLayout->setAlignment(mBatteryPanel, Qt::AlignVCenter);
    showBatteryPlaceholders();
}

void TF::FuMainMeaPage_Ui::updateBatteryLevelVisuals(int level) {
    if (!mBatteryLevelBar || !mBatteryStatusLabel) {
        return;
    }

    if (level < 0) {
        level = 0;
    }
    if (level > 100) {
        level = 100;
    }

    QString state = "high";
    if (level < 20) {
        state = "low";
    } else if (level < 60) {
        state = "mid";
    }

    mBatteryLevelBar->setProperty("batteryState", state);
    mBatteryLevelBar->style()->unpolish(mBatteryLevelBar);
    mBatteryLevelBar->style()->polish(mBatteryLevelBar);
    mBatteryLevelBar->setFormat("%p%");
    mBatteryLevelBar->setValue(level);

    QString statusText;
    if (level >= 95) {
        statusText = QCoreApplication::translate("Page", "满电");
    } else if (level < 20) {
        statusText = QCoreApplication::translate("Page", "低电");
    } else {
        statusText = QCoreApplication::translate("Page", "正常");
    }

    mBatteryStatusLabel->setText(QCoreApplication::translate("Page", "电量 %1% · %2").arg(level).arg(statusText));
}

void TF::FuMainMeaPage_Ui::showBatteryPlaceholders() {
    if (!mBatteryLevelBar || !mBatteryStatusLabel || !mBatteryChargeCurrent || !mBatteryTempLabel) {
        return;
    }

    mBatteryLevelBar->setProperty("batteryState", "unknown");
    mBatteryLevelBar->style()->unpolish(mBatteryLevelBar);
    mBatteryLevelBar->style()->polish(mBatteryLevelBar);
    mBatteryLevelBar->setValue(0);
    mBatteryLevelBar->setFormat(" --- ");

    const auto placeholder = QCoreApplication::translate("Page", "---");
    mBatteryStatusLabel->setText(QCoreApplication::translate("Page", "电量 %1 · %2").arg(placeholder, placeholder));
    mBatteryChargeCurrent->setText(placeholder);
    mBatteryTempLabel->setText(QCoreApplication::translate("Page", "温度 %1").arg(placeholder));
}

void TF::FuMainMeaPage_Ui::setVideoAreaStretch(int mainVideoStretch, int sideColumnStretch) {
    if (mainVideoStretch < 1)
        mainVideoStretch = 1;
    if (sideColumnStretch < 1)
        sideColumnStretch = 1;

    mMainVideoStretch = mainVideoStretch;
    mSideColumnStretch = sideColumnStretch;

    if (mVideoHLayout) {
        mVideoHLayout->setStretch(0, mMainVideoStretch);
        mVideoHLayout->setStretch(1, mSideColumnStretch);
    }

    if (mVideoWid) {
        auto policy = mVideoWid->sizePolicy();
        policy.setHorizontalStretch(mMainVideoStretch);
        mVideoWid->setSizePolicy(policy);
    }

    if (mVideoSideWid) {
        auto policy = mVideoSideWid->sizePolicy();
        policy.setHorizontalStretch(mSideColumnStretch);
        mVideoSideWid->setSizePolicy(policy);
    }
}