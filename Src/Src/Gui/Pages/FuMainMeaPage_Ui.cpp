#include "FuMainMeaPage_Ui.h"
#include "videowidgetx.h"
#include "FuVideoButtons.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>


void TF::FuMainMeaPage_Ui::setupUi(QWidget* wid) {
    mWid = wid;

    if (mWid->objectName().isEmpty())
        mWid->setObjectName("mWid");
    mWid->resize(800, 600);
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
    mVideoWid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mVideoSideWid = new QWidget(mVideoArea);
    mVideoSideWid->setObjectName("VideoSideWid");
    mVideoSideVLayout = new QVBoxLayout(mVideoSideWid);
    mVideoSideVLayout->setSpacing(6);
    mVideoSideVLayout->setContentsMargins(0, 0, 0, 0);

    mInfraredVideoWid = new QWidget(mVideoSideWid);
    mInfraredVideoWid->setObjectName("InfraredVideoWid");
    mInfraredVideoWid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    mStatisticsWid = new QWidget(mVideoSideWid);
    mStatisticsWid->setObjectName("StatisticsWid");
    mStatisticsWid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    mVideoSideVLayout->addWidget(mInfraredVideoWid);
    mVideoSideVLayout->addWidget(mStatisticsWid);

    mVideoHLayout->addWidget(mVideoWid, 3);
    mVideoHLayout->addWidget(mVideoSideWid, 2);

    mMainVLayout->addWidget(mVideoArea);
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

    mAiToggleBtn = new TechToggleButton("AI检测", mWid);
    mAiToggleBtn->setObjectName("StartStopAi");

    mSaveToggleBtn = new TechToggleButton("保存视频", mWid);
    mSaveToggleBtn->setObjectName("StartStopSave");

    mRefreshOnceBtn = new TechActionButton("刷新显示", mWid);
    mRefreshOnceBtn->setObjectName("RefreshOnce");

    mCtrlHLayout->addWidget(mMainCamToggleBtn);
    mCtrlHLayout->addWidget(mAiToggleBtn);
    mCtrlHLayout->addWidget(mSaveToggleBtn);
    mCtrlHLayout->addWidget(mRefreshOnceBtn);
    mCtrlHLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

}
