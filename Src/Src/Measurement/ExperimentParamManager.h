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
#include <QDateTime>
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
        double maxTemp{};     // 红外最高温度 (°C)
        double minTemp{};     // 红外最低温度 (°C)
        QString imagePath;
        QString oriImagePath;
        QString irImgPath;    // 红外伪彩色图像路径
        QString irDatPath;    // 红外原始温度数据路径
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
        QString buildImageDir() const;
        QString buildDetImagePath(int sampleId) const;
        QString buildOriImagePath(int sampleId) const;
        QString buildIrImagePath(int sampleId) const;
        QString buildIrDataPath(int sampleId) const;
        qint64 currentTimestampMs() const;

    private:
        QString mExperimentName;
        std::atomic<bool> mRecording{false};
        int mExperimentId{-1};
        int mNextSampleId{-1};
        QDateTime mRecordingStartTime;

        QThread *mWorkerThread{nullptr};
        ExperimentDbWorker *mWorker{nullptr};
    };
}

