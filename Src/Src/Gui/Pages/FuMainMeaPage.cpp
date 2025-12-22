#include "FuMainMeaPage.h"
#include "FuMainMeaPage_Ui.h"
#include "qthelper.h"
#include "videohelper.h"
#include "videowidgetx.h"
#include "TConfig.h"
#include "FuVideoButtons.h"
#include "DetectManager.h"
#include "ThermalManager.h"
#include "TFException.h"
#include <QPushButton>
#include <QVBoxLayout>


TF::FuMainMeaPage::FuMainMeaPage(QWidget* parent) : QWidget(parent), mUi(new FuMainMeaPage_Ui) {
    mUi->setupUi(this);
    this->initForm();

    initActions();
}

TF::FuMainMeaPage::~FuMainMeaPage() {
    delete mUi;
}

void TF::FuMainMeaPage::initActions() {
    connect(mUi->mMainCamToggleBtn, &QPushButton::pressed,
    this, &FuMainMeaPage::onMainCamBtnPressed);

    connect(mUi->mThermalCamToggleBtn, &QPushButton::pressed,
    this, &FuMainMeaPage::onThermalCamBtnPressed);

    connect(mUi->mSaveToggleBtn, &QPushButton::toggled,
        this, &FuMainMeaPage::onSaveBtnToggled);

    connect(mUi->mAiToggleBtn, &QPushButton::toggled,
        this, &FuMainMeaPage::onAiBtnToggled);
}

void TF::FuMainMeaPage::initForm() {
    WidgetPara widgetPara = mUi->mVideoWid->getWidgetPara();
    //widgetPara.borderWidth = 5;
    widgetPara.bgImage = QImage(QString("%1/config/bg_novideo.png").arg(QtHelper::appPath()));
    widgetPara.bannerEnable = true;
    widgetPara.scaleMode = ScaleMode_Auto;
    //widgetPara.videoMode = VideoMode_Hwnd;
    widgetPara.videoMode = VideoMode_Opengl;
    mUi->mVideoWid->setWidgetPara(widgetPara);

    //设置解码结构体参数(具体含义参见结构体定义)
    VideoPara videoPara = mUi->mVideoWid->getVideoPara();
    videoPara.videoCore =
        VideoHelper::getVideoCore();
    videoPara.decodeType = DecodeType_Fast;
    videoPara.hardware = "none";
    videoPara.transport = "tcp";
    videoPara.playRepeat = false;
    videoPara.readTimeout = 0;
    videoPara.connectTimeout = 2000;
    videoPara.decodeAudio = false;
    videoPara.playAudio = false;
    mUi->mVideoWid->setVideoPara(videoPara);

    mUi->mVideoWid->hideButtonAll();

    mVideoWid = mUi->mVideoWid;
}

void TF::FuMainMeaPage::onMainCamBtnPressed() {
    if (!mVideoWid) {
        return;
    }

    // Start stream
    if (!mUi->mMainCamToggleBtn->isChecked()) {
        if (!mMainCamPlaying.load()) {
            auto video_url = GET_STR_CONFIG("VideoURL");
            if (mVideoWid->open(video_url.c_str())) {
                mMainCamPlaying.store(true);
                mCurrentUrl = video_url.c_str();
            } else {
                mUi->mMainCamToggleBtn->setChecked(false);
            }
        }
    }
    // Stop stream
    else if (mUi->mMainCamToggleBtn->isChecked()) {
        if (mMainCamPlaying.load()) {
            mVideoWid->stop();
            mMainCamPlaying.store(false);
        }
    }
}

void TF::FuMainMeaPage::onThermalCamBtnPressed() {
    // Start stream
    if (!mUi->mThermalCamToggleBtn->isChecked()) {
        if (!ThermalManager::instance().getThermalOnState()) {
            try {
                ThermalManager::instance().start();
            } catch (TFPromptException &e) {
                QMessageBox::critical(this, "错误", e.what());
            }
        }
    }
    // Stop stream
    else if (mUi->mThermalCamToggleBtn->isChecked()) {
        if (ThermalManager::instance().getThermalOnState()) {
            try {
                ThermalManager::instance().stop();
            } catch (TFPromptException &e) {
                QMessageBox::critical(this, "错误", e.what());
            }
        }
    }
}

void TF::FuMainMeaPage::onSaveBtnToggled(bool checked) {
    if (!mVideoWid) {
        return;
    }

    if (checked) {
        if (!mVideoWid->getIsRunning()) {
            mUi->mSaveToggleBtn->setChecked(false);
            return;
        }

        QDir dir(QDir::currentPath());
        QString videoDir = dir.filePath("videos");
        if (!dir.exists("videos")) {
            dir.mkpath("videos");
        }

        const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        const QString filePath = QDir(videoDir).filePath(QString("%1.mp4").arg(timestamp));
        mVideoWid->recordStart(filePath);
        mRecording.store(true);
    } else {
        if (mRecording.load()) {
            mVideoWid->recordStop();
            mRecording.store(false);
        }
    }
}

void TF::FuMainMeaPage::onAiBtnToggled(bool checked) {
    if (checked) {
        if (!mMainCamPlaying.load()) {
            mUi->mAiToggleBtn->blockSignals(true);
            mUi->mAiToggleBtn->setChecked(false);
            mUi->mAiToggleBtn->blockSignals(false);
            return;
        }
    }

    if (checked) {
        //mDetectorThread->setPause(false);
        mVideoWid->startDetect();
        TFDetectManager::instance().startDetect();
    } else {
        //mDetectorThread->setPause(true);
        TFDetectManager::instance().stopDetect();
        mVideoWid->stopDetect();
    }
}
