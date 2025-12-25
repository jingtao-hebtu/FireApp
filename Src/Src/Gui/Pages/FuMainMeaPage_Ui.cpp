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
#include <QSizePolicy>
#include <QWidget>
#include <QString>


void TF::FuMainMeaPage_Ui::setupUi(QWidget* wid) {
    mWid = wid;

    if (mWid->objectName().isEmpty())
        mWid->setObjectName("mWid");

    auto height = GET_INT_CONFIG("MainUi", "Height");
    auto width = GET_INT_CONFIG("MainUi", "Width");
    mWid->resize(height, width);
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

    mThermalCamera = new ThermalCamera(mThermalWid);
    mThermalWidget = new ThermalWidget(mThermalCamera, mThermalWid);
    ThermalManager::instance().setThermalCamera(mThermalCamera);
}

void TF::FuMainMeaPage_Ui::initStatistics() {
    mStatisticsWid = new QWidget(mVideoSideWid);
    mStatisticsWid->setObjectName("StatisticsWid");
    QSizePolicy statisticsPolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    statisticsPolicy.setHorizontalStretch(mSideColumnStretch);
    mStatisticsWid->setSizePolicy(statisticsPolicy);

    mStatisticsVLayout = new QVBoxLayout(mStatisticsWid);
    mStatisticsVLayout->setSpacing(0);
    mStatisticsVLayout->setContentsMargins(0, 0, 0, 0);

    mFireHeightCurveViewer = createCurveViewer("火焰高度", 0, 100, 0, 1000);
    mFireAreaCurveViewer = createCurveViewer("火焰面积", 0, 100, 0, 1000);

    mStatisticsVLayout->addWidget(mFireHeightCurveViewer);
    mStatisticsVLayout->addWidget(mFireAreaCurveViewer);
    mStatisticsVLayout->setStretch(0, 1);
    mStatisticsVLayout->setStretch(1, 1);

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

    mSaveToggleBtn = new TechToggleButton("保存视频", mWid);
    mSaveToggleBtn->setObjectName("StartStopSave");

    mCtrlHLayout->addWidget(mMainCamToggleBtn);
    mCtrlHLayout->addWidget(mThermalCamToggleBtn);
    mCtrlHLayout->addWidget(mAiToggleBtn);
    mCtrlHLayout->addWidget(mSaveToggleBtn);
    mCtrlHLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // Distance and Tilt Angle
    auto dist_angle_glayout = new QGridLayout();
    mCtrlHLayout->addLayout(dist_angle_glayout, 1);

    mDistLabel = new QLabel(mWid);
    mDistLabel->setObjectName("DistLabel");
    mDistLabel->setText("距离: ");
    mDistEdit = new QLineEdit(mWid);
    mDistEdit->setObjectName("DistEdit");
    mDistEdit->setText(" --- ");
    dist_angle_glayout->addWidget(mDistLabel, 1, 1, 1, 1);
    dist_angle_glayout->addWidget(mDistEdit, 1, 2, 1, 1);

    mTiltAngleLabel = new QLabel(mWid);
    mTiltAngleLabel->setObjectName("TiltAngleLabel");
    mTiltAngleLabel->setText("倾角: ");
    mTiltAngleEdit = new QLineEdit(mWid);
    mTiltAngleEdit->setObjectName("TiltAngleEdit");
    mTiltAngleEdit->setText(" --- ");
    dist_angle_glayout->addWidget(mTiltAngleLabel, 2, 1, 1, 1);
    dist_angle_glayout->addWidget(mTiltAngleEdit, 2, 2, 1, 1);

    auto hspacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    mCtrlHLayout->addItem(hspacer);
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