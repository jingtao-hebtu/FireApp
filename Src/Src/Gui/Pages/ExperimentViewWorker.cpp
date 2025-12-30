#include "ExperimentViewWorker.h"
#include "ExperimentDataViewPage.h"

#include "SQLiteCpp/SQLiteCpp.h"

#include <utility>

namespace TF {

    ExperimentViewWorker::ExperimentViewWorker(QString dbPath, QObject *parent)
        : QObject(parent), mDbPath(std::move(dbPath)) {
    }

    bool ExperimentViewWorker::ensureDb() {
        if (mDb) {
            return true;
        }

        try {
            mDb = std::make_unique<SQLite::Database>(mDbPath.toStdString(), SQLite::OPEN_READONLY);
            mDb->exec("PRAGMA journal_mode=WAL;");
            mDb->exec("PRAGMA busy_timeout=3000;");
        }
        catch (const std::exception &e) {
            reportError(QStringLiteral("数据库打开失败: %1").arg(QString::fromUtf8(e.what())));
            mDb.reset();
            return false;
        }
        return true;
    }

    void ExperimentViewWorker::reportError(const QString &message) {
        emit errorOccurred(message);
    }

    void ExperimentViewWorker::queryExperiments(qint64 from, qint64 to) {
        if (!ensureDb()) {
            return;
        }

        try {
            SQLite::Statement stmt(*mDb,
                                   "SELECT exp_id, name, start_time, end_time "
                                   "FROM Experiment "
                                   "WHERE start_time>=? AND start_time<=? "
                                   "ORDER BY start_time DESC;");
            stmt.bind(1, static_cast<std::int64_t>(from));
            stmt.bind(2, static_cast<std::int64_t>(to));

            QVector<ExperimentBrief> experiments;
            while (stmt.executeStep()) {
                ExperimentBrief info;
                info.expId = stmt.getColumn(0).getInt();
                info.name = QString::fromStdString(stmt.getColumn(1).getString());
                info.startTime = stmt.getColumn(2).getInt64();
                if (!stmt.isColumnNull(3)) {
                    info.hasEndTime = true;
                    info.endTime = stmt.getColumn(3).getInt64();
                }
                experiments.append(info);
            }

            emit experimentsReady(experiments);
        }
        catch (const std::exception &e) {
            reportError(QStringLiteral("查询实验列表失败: %1").arg(QString::fromUtf8(e.what())));
        }
    }

    void ExperimentViewWorker::loadExperimentSamples(int expId) {
        if (!ensureDb()) {
            return;
        }

        try {
            SQLite::Statement stmt(*mDb,
                                   "SELECT d.sample_id, d.DateTime, i.image_path "
                                   "FROM Data d "
                                   "JOIN DetectImage i ON i.exp_id=d.exp_id AND i.sample_id=d.sample_id "
                                   "WHERE d.exp_id=? AND d.channel_id=1 "
                                   "ORDER BY d.sample_id;"
            );
            stmt.bind(1, expId);

            QVector<SampleIndexItem> samples;
            while (stmt.executeStep()) {
                SampleIndexItem item;
                item.sampleId = stmt.getColumn(0).getInt();
                item.datetime = stmt.getColumn(1).getInt64();
                item.imagePath = QString::fromStdString(stmt.getColumn(2).getString());
                samples.append(item);
            }

            emit experimentSamplesReady(expId, samples);
        }
        catch (const std::exception &e) {
            reportError(QStringLiteral("加载实验样本失败: %1").arg(QString::fromUtf8(e.what())));
        }
    }

    void ExperimentViewWorker::loadSampleValues(int expId, int sampleId) {
        if (!ensureDb()) {
            return;
        }

        QVector<QPair<QString, double>> values;
        QString imagePath;

        try {
            {
                SQLite::Statement stmt(*mDb,
                                       "SELECT c.name, d.value "
                                       "FROM Data d "
                                       "JOIN Channel c ON c.channel_id=d.channel_id "
                                       "WHERE d.exp_id=? AND d.sample_id=? "
                                       "ORDER BY d.channel_id;"
                );
                stmt.bind(1, expId);
                stmt.bind(2, sampleId);
                while (stmt.executeStep()) {
                    const auto name = QString::fromStdString(stmt.getColumn(0).getString());
                    const double value = stmt.getColumn(1).getDouble();
                    values.append({name, value});
                }
            }

            {
                SQLite::Statement stmt(*mDb,
                                       "SELECT image_path FROM DetectImage WHERE exp_id=? AND sample_id=? LIMIT 1;");
                stmt.bind(1, expId);
                stmt.bind(2, sampleId);
                if (stmt.executeStep()) {
                    imagePath = QString::fromStdString(stmt.getColumn(0).getString());
                }
            }

            emit sampleValuesReady(expId, sampleId, values, imagePath);
        }
        catch (const std::exception &e) {
            reportError(QStringLiteral("加载样本详情失败: %1").arg(QString::fromUtf8(e.what())));
        }
    }
}

