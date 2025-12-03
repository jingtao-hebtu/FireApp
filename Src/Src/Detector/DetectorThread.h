/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DetectorThread.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_DETECTORTHREAD_H
#define FIREUI_DETECTORTHREAD_H

#include <QThread>
#include <atomic>


class VideoWidget;

namespace TF {
    class DetectorThread : public QThread
    {
        Q_OBJECT

    public:
        explicit DetectorThread(
            VideoWidget* videoWidget,
            int sleep_ms = 100,
            QObject* parent = 0
        );

    public:
        void setStopped(bool pause) { mIsStopped.store(pause); }
        void setPause(bool pause) { mIsPaused.store(pause); }

    protected:
        void run();

    public slots:
        void onStop();
        void onPause();
        void onResume();

    private:
        std::atomic<bool> mIsStopped{false};
        std::atomic<bool> mIsPaused{true};

        int mSleepMs {100};

        VideoWidget* mVideoWid{nullptr};
    };
};


#endif //FIREUI_DETECTORTHREAD_H
