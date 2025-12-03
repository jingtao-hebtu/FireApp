/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DetectorThread.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "DetectorThread.h"
#include "videowidgetx.h"
#include <QImage>


TF::DetectorThread::DetectorThread(
    VideoWidget* videoWidget,
    int sleep_ms,
    QObject* parent
) : QThread(parent), mSleepMs(sleep_ms), mVideoWid(videoWidget) {
}

void TF::DetectorThread::run() {
    while (!mIsStopped) {
        if (mIsPaused || !mVideoWid) {
            msleep(10);
            continue;
        }

        QImage image = mVideoWid->getImage();
        qDebug() << TIMEMS << mVideoWid->getVideoThread()->getFlag() << image.size();


        msleep(mSleepMs);
    }
}

void TF::DetectorThread::onStop() {
}

void TF::DetectorThread::onPause() {
}

void TF::DetectorThread::onResume() {
}
