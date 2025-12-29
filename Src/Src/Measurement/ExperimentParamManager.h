/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : ExperimentParamManager.h
   Author : OpenAI ChatGPT
   Date   : 2025/01/17
   Brief  : 实验参数管理与异步保存
**************************************************************************/
#pragma once

#include <atomic>
#include <optional>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>
#include <QString>

#include "TSingleton.h"

namespace TF {

    struct ExperimentRecord {
        int expId{};
        int sampleId{};
        qint64 timestampMs{};
        double dist{};
        double tilt{};
        double fireHeight{};
        double fireArea{};
        QString imagePath;
    };

    class ExperimentDbWorker : public QObject {
        Q_OBJECT
    public:
        explicit ExperimentDbWorker(QObject *parent = nullptr);

        void enqueue(const ExperimentRecord &record);

    public slots:
        void startWork();
        void stopWork();

    private:
        void process(const ExperimentRecord &record);

    private:
        QMutex mMutex;
        QWaitCondition mCond;
        QQueue<ExperimentRecord> mQueue;
        std::atomic<bool> mRunning{false};
    };

    class ExperimentParamManager : public QObject, public TBase::TSingleton<ExperimentParamManager>
    {
        Q_OBJECT

    public:
        bool experimentNameExists(const QString &name) const;

        [[nodiscard]] bool isRecording() const { return mRecording.load(); }
        [[nodiscard]] QString currentExperimentName() const { return mExperimentName; }

        bool startRecording(const QString &name, QString *error = nullptr);
        void stopRecording();

        std::optional<ExperimentRecord> prepareSample(float fireHeight, float fireArea);

    private:
        friend class TBase::TSingleton<ExperimentParamManager>;
        explicit ExperimentParamManager(QObject *parent = nullptr);
        ~ExperimentParamManager() override;

        int nextSampleId();
        void ensureWorker();
        void shutdownWorker();
        QString buildImagePath(int sampleId) const;
        qint64 currentTimestampMs() const;

    private:
        QString mExperimentName;
        std::atomic<bool> mRecording{false};
        int mExperimentId{-1};
        int mNextSampleId{-1};

        QThread *mWorkerThread{nullptr};
        ExperimentDbWorker *mWorker{nullptr};
    };
}

