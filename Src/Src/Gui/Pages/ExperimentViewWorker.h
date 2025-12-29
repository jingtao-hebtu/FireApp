#pragma once

#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>
#include <memory>

namespace SQLite {
    class Database;
}

namespace TF {

    struct ExperimentBrief;
    struct SampleIndexItem;

    class ExperimentViewWorker : public QObject {
        Q_OBJECT
    public:
        explicit ExperimentViewWorker(QString dbPath, QObject *parent = nullptr);

    public slots:
        void queryExperiments(qint64 from, qint64 to);
        void loadExperimentSamples(int expId);
        void loadSampleValues(int expId, int sampleId);

    signals:
        void experimentsReady(const QVector<ExperimentBrief> &experiments);
        void experimentSamplesReady(int expId, const QVector<SampleIndexItem> &samples);
        void sampleValuesReady(int expId, int sampleId, const QVector<QPair<QString, double>> &values, const QString &imagePath);
        void errorOccurred(const QString &message);

    private:
        bool ensureDb();
        void reportError(const QString &message);

    private:
        QString mDbPath;
        std::unique_ptr<SQLite::Database> mDb;
    };
}

