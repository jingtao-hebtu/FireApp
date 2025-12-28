#include "FuMainMeaPage.h"
#include "FuMainMeaPage_Ui.h"
#include "AppMonitor.h"
#include "qthelper.h"
#include "videohelper.h"
#include "videowidgetx.h"
#include "TConfig.h"
#include "FuVideoButtons.h"
#include "DetectManager.h"
#include "ThermalManager.h"
#include "TFDistClient.h"
#include "WitImuSerial.h"
#include "HKCamSearcher.h"
#include "TFException.h"
#include "CamConfigWid.h"
#include "BmsWorker.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QPoint>
#include <QRect>


TF::FuMainMeaPage::FuMainMeaPage(QWidget* parent) : QWidget(parent), mUi(new FuMainMeaPage_Ui) {
    mUi->setupUi(this);
    this->initForm();

    initActions();
}

TF::FuMainMeaPage::~FuMainMeaPage() {
    deinitMea();
    delete mUi;
}

void TF::FuMainMeaPage::initAfterDisplay() {
    initMea();
    initHardware();
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

    connect(this, &FuMainMeaPage::updateDist,
            this, &FuMainMeaPage::onUpdateDist);

    connect(this, &FuMainMeaPage::updateWitImuData,
            this, &FuMainMeaPage::onUpdateWitImuData);

    connect(mUi->mCamConfigBtn, &QPushButton::pressed,
        this, &FuMainMeaPage::onCamConfigBtnPressed);
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

void TF::FuMainMeaPage::initHardware() {
    // HK Cam
    mCamSearcher = new HKCamSearcher(this);
    mCamSearcher->searchDevice();

    // BMS
    qRegisterMetaType<BmsStatus>("BmsStatus");
    mBmsWorker = new BmsWorker();
    mBmsThread = new QThread(this);
    mBmsWorker->moveToThread(mBmsThread);

    // 线程结束后安全销毁 worker（deleteLater 会在其线程上下文执行析构）
    connect(mBmsThread, &QThread::finished, mBmsWorker, &QObject::deleteLater);

    // UI -> worker（QueuedConnection，确保在 worker 线程执行）
    connect(this, &FuMainMeaPage::initBms,  mBmsWorker, &BmsWorker::onInitialize, Qt::QueuedConnection);
    connect(this, &FuMainMeaPage::startBms, mBmsWorker, &BmsWorker::onStart,      Qt::QueuedConnection);
    connect(this, &FuMainMeaPage::stopBms,  mBmsWorker, &BmsWorker::onStop,       Qt::QueuedConnection);

    // worker -> UI
    connect(mBmsWorker, &BmsWorker::statusUpdated, this,
        &FuMainMeaPage::onBmsStatusUpdated, Qt::QueuedConnection);
    connect(mBmsWorker, &BmsWorker::connectionStateChanged, this,
        &FuMainMeaPage::onBmsConnectionStateChanged, Qt::QueuedConnection);

    mBmsThread->start();
}

void TF::FuMainMeaPage::initMea() {
    // Measurement
    mDistTimeoutTimer = new QTimer(this);
    mDistTimeoutTimer->setInterval(3000);
    mDistTimeoutTimer->setSingleShot(true);
    connect(mDistTimeoutTimer, &QTimer::timeout, this, &FuMainMeaPage::onDistTimeout);

    mDistClient = new TFDistClient(this);
    try {
        mDistClient->open();
    }
    catch (TFPromptException& e) {
        QMessageBox::critical(this, "错误", e.what());
    }

    mWitImuSerialThread = new QThread(this);
    mWitImuSerial = new WitImuSerial();
    mWitImuSerial->moveToThread(mWitImuSerialThread);

    connect(mWitImuSerialThread, &QThread::finished,
            mWitImuSerial, &QObject::deleteLater);
    connect(this, &FuMainMeaPage::requestWitImuOpen,
            mWitImuSerial, &WitImuSerial::onRequestWitImuOpen, Qt::QueuedConnection);
    connect(this, &FuMainMeaPage::requestWitImuClose,
            mWitImuSerial, &WitImuSerial::onRequestWitImuClose, Qt::QueuedConnection);

    mWitImuSerialThread->start();
    emit requestWitImuOpen();
}

void TF::FuMainMeaPage::deinitMea() {
    if (mBmsThread && mBmsThread->isRunning()) {
        if (mBmsWorker) {
            QMetaObject::invokeMethod(
                mBmsWorker,
                &BmsWorker::onStop,
                Qt::BlockingQueuedConnection
            );
        }

        mBmsThread->quit();
        mBmsThread->wait();
    }

    if (mWitImuSerialThread && mWitImuSerialThread->isRunning() && mWitImuSerial) {
        QMetaObject::invokeMethod(
            mWitImuSerial,
            &WitImuSerial::close,
            Qt::BlockingQueuedConnection
        );

        mWitImuSerialThread->quit();
        mWitImuSerialThread->wait();
    }
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
            }
            else {
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
            }
            catch (TFPromptException& e) {
                QMessageBox::critical(this, "错误", e.what());
            }
        }
    }
    // Stop stream
    else if (mUi->mThermalCamToggleBtn->isChecked()) {
        if (ThermalManager::instance().getThermalOnState()) {
            try {
                ThermalManager::instance().stop();
            }
            catch (TFPromptException& e) {
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
    }
    else {
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
    }
    else {
        //mDetectorThread->setPause(true);
        TFDetectManager::instance().stopDetect();
        mVideoWid->stopDetect();
    }
}

void TF::FuMainMeaPage::onCamConfigBtnPressed() {
    if (!mUi || !mUi->mVideoSideWid) {
        return;
    }

    if (!mCamConfigWid) {
        mCamConfigWid = new CamConfigWid(this);
    }

    const QRect targetRect(mUi->mVideoSideWid->mapToGlobal(QPoint(0, 0)), mUi->mVideoSideWid->size());
    mCamConfigWid->showAt(targetRect);
}

void TF::FuMainMeaPage::onBmsStatusUpdated(const BmsStatus& status) {
    LOG_F(INFO, "BMS total voltage %f", status.mTotalVoltage_V);
    LOG_F(INFO, "BMS remain capacity %f", status.mRemainCapacity_Ah);
}

void TF::FuMainMeaPage::onBmsConnectionStateChanged(bool connected) {
    LOG_F(INFO, "BMS connection state %d", connected);
}


void TF::FuMainMeaPage::onUpdateDist(float dist) {
    mUi->mDistValueLabel->setText(QString::number(dist, 'f', 1));
    mDistTimeoutTimer->start();
}

void TF::FuMainMeaPage::onDistTimeout() {
    mUi->mDistValueLabel->setText("---");
}

void TF::FuMainMeaPage::onUpdateWitImuData(const WitImuData& data) {
    mImuData = data;
    auto tilt_angle = data.angle.x;
    mUi->mTiltAngleValueLabel->setText(QString::number(tilt_angle, 'f', 1));
}
