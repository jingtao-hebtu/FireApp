#include "FuMainMeaPage_Ui.h"
#include "videowidgetx.h"
#include "FuVideoButtons.h"


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
    mVideoWid = new VideoWidget(mWid);
    mVideoWid->setObjectName("VideoWid");

    mMainVLayout->addWidget(mVideoWid);
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
