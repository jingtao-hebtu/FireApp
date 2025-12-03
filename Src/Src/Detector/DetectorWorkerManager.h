#pragma once

#include <QThread>
#include <atomic>

#include "DetectorWorker.h"
#include "TSingleton.h"

namespace TF {

    class DetectorWorkerManager : public QObject, public TBase::TSingleton<DetectorWorkerManager> {
    Q_OBJECT

    public:
        DetectorWorkerManager(QObject *parent = nullptr);

        ~DetectorWorkerManager() override;

        void start();

        void stop();

        DetectorWorker *worker() const;

    signals:
        void frameProcessed(const QString &sourceFlag, const QImage &image, double meanValue, int timeCost);

        void stopWorker();

    private:
        friend class TBase::TSingleton<DetectorWorkerManager>;

        void ensureWorker();

        QThread *mThread{nullptr};
        DetectorWorker *mWorker{nullptr};
        std::atomic<bool> mRunning{false};
    };
}

