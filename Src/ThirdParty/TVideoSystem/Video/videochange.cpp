#include "videochange.h"
#include "videowidgetx.h"

VideoChange::VideoChange(QObject *parent) : QObject(parent) {
    sleepTime = -1;
    currentIndex = 0;
    videoWidget1 = NULL;
    videoWidget2 = NULL;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(change()));
    timer->setSingleShot(true);
}

void VideoChange::change() {
    if (currentIndex == 0) {
        videoWidget2->setVisible(false);
        videoWidget1->setVisible(true);
        videoWidget2->stop();
    } else if (currentIndex == 1) {
        videoWidget1->setVisible(false);
        videoWidget2->setVisible(true);
        videoWidget1->stop();
    }

    emit changed();
}

void VideoChange::receivePlayStart(int time) {
    //如果没有延时时间则自动计算/将要播放的视频分辨率越大则延时越大
    int sleepTime = this->sleepTime;
    if (sleepTime < 0) {
        VideoWidget *videoWidget = (VideoWidget *) sender();
        int width = videoWidget->getVideoWidth();
        if (width >= 3840) {
            sleepTime = 300;
        } else if (width >= 2048) {
            sleepTime = 200;
        } else if (width >= 1920) {
            sleepTime = 100;
        } else {
            sleepTime = 0;
        }
    }

    timer->stop();
    timer->start(sleepTime);
}

int VideoChange::getCurrentIndex() {
    return this->currentIndex;
}

VideoWidget *VideoChange::getVideoWidget1() {
    return this->videoWidget1;
}

VideoWidget *VideoChange::getVideoWidget2() {
    return this->videoWidget2;
}

void VideoChange::setSleepTime(int sleepTime) {
    this->sleepTime = sleepTime;
}

void VideoChange::setVideoWidget(VideoWidget *videoWidget1, VideoWidget *videoWidget2) {
    connect(videoWidget1, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
    connect(videoWidget2, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
    this->videoWidget1 = videoWidget1;
    this->videoWidget2 = videoWidget2;
    this->reset();
}

void VideoChange::reset() {
    currentIndex = 1;
    videoWidget1->setVisible(true);
    videoWidget2->setVisible(false);
}

void VideoChange::open(const QString &url1, const QString &url2) {
    //先后台默默的打开不显示/等待打开成功后再显示
    currentIndex = (currentIndex == 0 ? 1 : 0);
    if (currentIndex == 0) {
        videoWidget1->open(url1);
    } else if (currentIndex == 1) {
        videoWidget2->open(url2);
    }
}
