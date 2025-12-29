#pragma once

#include <QDateTime>
#include <QImage>
#include <QMutex>
#include <QQueue>
#include <QElapsedTimer>
#include <QThread>
#include <QWaitCondition>
#include <atomic>
#include <cstddef>
#include <vector>

#include "TSingleton.h"

namespace TF {

    struct AiResultMetaInfo {
        QString filePath;
        QString description;
        QDateTime timestamp;
    };

    class AiResultSaveWorker : public QObject {
    Q_OBJECT
    public:
        void enqueue(const QImage &image, const QString &filePath, const QString &description);

    public slots:
        void startWork();
        void stopWork();

    private:
        struct Task {
            QImage image;
            QString filePath;
            QString description;
        };

        QMutex mMutex;
        QWaitCondition mCond;
        QQueue<Task> mTasks;
        std::atomic<bool> mRunning{false};
    };

    class AiResultSaveManager : public QObject, public TBase::TSingleton<AiResultSaveManager> {
    Q_OBJECT
    public:
        void setEnabled(bool enabled);

        [[nodiscard]] bool isEnabled() const { return mEnabled.load(); }

        void setSaveFrequency(int imagesPerSecond);

        [[nodiscard]] int saveFrequency() const { return mSaveFrequency.load(); }

        void submitResult(const QImage &image,
                          const QString &sourceFlag,
                          int timeCost,
                          int detectionId,
                          std::size_t detectedCount,
                          float fireHeight,
                          float fireArea);

        [[nodiscard]] std::vector<AiResultMetaInfo> recentRecords() const;

    signals:
        void stopWorker();

    private:
        friend class TBase::TSingleton<AiResultSaveManager>;
        explicit AiResultSaveManager(QObject *parent = nullptr);

        void ensureWorker();

        bool shouldSaveNow();

        void recordMeta(const QString &path, const QString &description);

        void resetFrequencyWindow();

        QThread *mThread{nullptr};
        AiResultSaveWorker *mWorker{nullptr};

        std::atomic<bool> mEnabled{false};
        std::atomic<int> mSaveFrequency{1};

        mutable QMutex mFreqMutex;
        QElapsedTimer mFrequencyTimer;
        int mSavedInWindow{0};

        mutable QMutex mRecordMutex;
        std::vector<AiResultMetaInfo> mRecentRecords;
    };
}

