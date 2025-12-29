/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DbManager.cpp
   Author : tao.jing
   Date   : 2025.12.28
   Brief  :
**************************************************************************/
#include "DbManager.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "TConfig.h"
#include "TSysUtils.h"
#include "PathConfig.h"
#include "TLog.h"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <iostream>


namespace TF {
    const char* InsertSql(DbManager::ConflictPolicy policy) {
        switch (policy) {
        case DbManager::ConflictPolicy::Abort:
            return "INSERT INTO Data(exp_id, channel_id, sample_id, DateTime, value) "
                   "VALUES(?,?,?,?,?)";
        case DbManager::ConflictPolicy::Ignore:
            return "INSERT OR IGNORE INTO Data(exp_id, channel_id, sample_id, DateTime, value) "
                   "VALUES(?,?,?,?,?)";
        case DbManager::ConflictPolicy::Replace:
            return "INSERT OR REPLACE INTO Data(exp_id, channel_id, sample_id, DateTime, value) "
                   "VALUES(?,?,?,?,?)";
        default:
            return "INSERT INTO Data(exp_id, channel_id, sample_id, DateTime, value) "
                   "VALUES(?,?,?,?,?)";
        }
    }

    template <class Fn, class Arg>
    void InvokeRowCallback(Fn&& fn, const Arg& row) {
        using Ret = std::invoke_result_t<Fn, const Arg&>;
        if constexpr (std::is_same_v<Ret, bool>) {
            // bool: true to continue, false to abort
            if (!fn(row)) {
                throw std::runtime_error("__stop_iteration__");
            }
        } else {
            fn(row);
        }
    }
};


void TF::DbManager::init() {
    initParams();
    initDb();
}

void TF::DbManager::initParams() {
    auto db_file_dir = GET_STR_CONFIG("Database", "DbDir");
    db_file_dir = TBase::joinPath({TFPathParam("AppConfigDir"), db_file_dir});
    auto db_file_name = GET_STR_CONFIG("Database", "DbFileName");
    auto db_file_path = TBase::joinPath(db_file_dir, db_file_name);
    mDBFile = db_file_path;
}

void TF::DbManager::initDb() {
    if (mDBFile.empty()) {
        return;
    }

    if (mInitialized) {
        return;
    }

    try {
        mDB = new SQLite::Database(mDBFile, SQLite::OPEN_READWRITE);
    }
    catch (std::exception &e) {
        LOG_F(ERROR, "SQLite open exception: %s.", e.what());
        return;
    }

    LOG_F(INFO, "Load database file path %s.", mDBFile.c_str());
    mInitialized = true;
}

void TF::DbManager::EnsureSchema() {
    std::scoped_lock lk(mMtx);
    auto& d = db();

    // Create Table
    d.exec(
        "CREATE TABLE IF NOT EXISTS Data ("
        "  exp_id     INTEGER NOT NULL,"
        "  channel_id INTEGER NOT NULL,"
        "  sample_id  INTEGER NOT NULL,"
        "  DateTime   INTEGER NOT NULL,"
        "  value      REAL    NOT NULL,"
        "  PRIMARY KEY (exp_id, channel_id, sample_id)"
        ") WITHOUT ROWID;"
    );

    // Indexes
    d.exec(
        "CREATE INDEX IF NOT EXISTS idx_Data_exp_ch_time "
        "ON Data(exp_id, channel_id, DateTime);"
    );
    d.exec(
        "CREATE INDEX IF NOT EXISTS idx_Data_exp_sample_ch "
        "ON Data(exp_id, sample_id, channel_id);"
    );

    // Experiment
    d.exec(
        "CREATE TABLE IF NOT EXISTS Experiment ("
        "  exp_id     INTEGER PRIMARY KEY,"
        "  name       TEXT    NOT NULL,"
        "  start_time INTEGER NOT NULL,"
        "  end_time   INTEGER,"
        "  CHECK (length(name) > 0),"
        "  CHECK (end_time IS NULL OR end_time >= start_time)"
        ");"
    );
    d.exec("CREATE INDEX IF NOT EXISTS idx_Experiment_start_time ON Experiment(start_time);");
    d.exec("CREATE INDEX IF NOT EXISTS idx_Experiment_name ON Experiment(name);");
}

void TF::DbManager::ConfigureForIngest() {
    std::scoped_lock lk(mMtx);
    auto& d = db();

    d.exec("PRAGMA journal_mode = WAL;");
    d.exec("PRAGMA synchronous = NORMAL;");
    d.exec("PRAGMA busy_timeout = 5000;");
    d.exec("PRAGMA temp_store = MEMORY;");
}

void TF::DbManager::WalCheckpoint(bool truncate) {
    std::scoped_lock lk(mMtx);
    auto& d = db();
    if (truncate) {
        d.exec("PRAGMA wal_checkpoint(TRUNCATE);");
    } else {
        d.exec("PRAGMA wal_checkpoint(PASSIVE);");
    }
}

std::size_t TF::DbManager::InsertBatch(std::span<const DataRow> rows, ConflictPolicy policy) {
    if (rows.empty()) {
        return 0;
    }

    std::scoped_lock lk(mMtx);
    auto& d = db();

    SQLite::Transaction txn(d);
    SQLite::Statement stmt(d, InsertSql(policy));

    std::size_t changed = 0;
    for (const auto& r : rows) {
        stmt.bind(1, r.exp_id);
        stmt.bind(2, r.channel_id);
        stmt.bind(3, r.sample_id);
        stmt.bind(4, static_cast<std::int64_t>(r.datetime));
        stmt.bind(5, r.value);

        changed += static_cast<std::size_t>(stmt.exec());

        stmt.reset();
        stmt.clearBindings();
    }

    txn.commit();
    return changed;
}

TF::DbManager::Writer::Writer(TF::DbManager& mgr, ConflictPolicy policy)
    : mDbMgr(&mgr)
    , mLock(mgr.mMtx)
    , mTxn(mgr.db())
    , mStmt(mgr.db(), InsertSql(policy)) {
}

std::size_t TF::DbManager::Writer::Add(const DataRow& r) {
    return Add(std::span<const DataRow>(&r, 1));
}

std::size_t TF::DbManager::Writer::Add(std::span<const DataRow> rows) {
    if (rows.empty()) {
        return 0;
    }
    if (!mDbMgr) {
        throw std::runtime_error("DbManager::Writer is not valid.");
    }
    if (mCommitted) {
        throw std::runtime_error("Writer already committed.");
    }

    std::size_t changed = 0;
    for (const auto& r : rows) {
        mStmt.bind(1, r.exp_id);
        mStmt.bind(2, r.channel_id);
        mStmt.bind(3, r.sample_id);
        mStmt.bind(4, static_cast<std::int64_t>(r.datetime));
        mStmt.bind(5, r.value);

        changed += static_cast<std::size_t>(mStmt.exec());

        mStmt.reset();
        mStmt.clearBindings();
    }
    return changed;
}

void TF::DbManager::Writer::Commit() {
    if (!mDbMgr) {
        throw std::runtime_error("DbManager::Writer is not valid.");
    }
    if (mCommitted) {
        return;
    }
    mTxn.commit();
    mCommitted = true;
}

TF::DbManager::Writer TF::DbManager::BeginWriter(ConflictPolicy policy) {
    return Writer(*this, policy);
}

std::vector<TF::DbManager::DataRow> TF::DbManager::QueryExperimentAll(int exp_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT exp_id, channel_id, sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? "
        "ORDER BY channel_id, sample_id;"
    );
    q.bind(1, exp_id);

    std::vector<DataRow> out;
    out.reserve(1024);

    while (q.executeStep()) {
        DataRow r;
        r.exp_id     = q.getColumn(0).getInt();
        r.channel_id = q.getColumn(1).getInt();
        r.sample_id  = q.getColumn(2).getInt();
        r.datetime   = static_cast<i64>(q.getColumn(3).getInt64());
        r.value      = q.getColumn(4).getDouble();
        out.push_back(r);
    }
    return out;
}

template <typename Fn>
void TF::DbManager::ForEachExperimentRow(int exp_id, Fn&& fn) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT exp_id, channel_id, sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? "
        "ORDER BY channel_id, sample_id;"
    );
    q.bind(1, exp_id);

    try {
        while (q.executeStep()) {
            DataRow r;
            r.exp_id     = q.getColumn(0).getInt();
            r.channel_id = q.getColumn(1).getInt();
            r.sample_id  = q.getColumn(2).getInt();
            r.datetime   = static_cast<i64>(q.getColumn(3).getInt64());
            r.value      = q.getColumn(4).getDouble();
            InvokeRowCallback(fn, r);
        }
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "__stop_iteration__") {
            return;
        }
        throw;
    }
}

template void TF::DbManager::ForEachExperimentRow<int(*)(const TF::DbManager::DataRow&)>(int, int(*&&)(const DataRow&)) const;

std::vector<TF::DbManager::ChannelPoint> TF::DbManager::QueryChannelAll(int exp_id, int channel_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? AND channel_id=? "
        "ORDER BY sample_id;"
    );
    q.bind(1, exp_id);
    q.bind(2, channel_id);

    std::vector<ChannelPoint> out;
    out.reserve(1024);

    while (q.executeStep()) {
        ChannelPoint p;
        p.sample_id = q.getColumn(0).getInt();
        p.datetime  = static_cast<i64>(q.getColumn(1).getInt64());
        p.value     = q.getColumn(2).getDouble();
        out.push_back(p);
    }
    return out;
}

template <typename Fn>
void TF::DbManager::ForEachChannelPoint(int exp_id, int channel_id, Fn&& fn) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? AND channel_id=? "
        "ORDER BY sample_id;"
    );
    q.bind(1, exp_id);
    q.bind(2, channel_id);

    try {
        while (q.executeStep()) {
            ChannelPoint p;
            p.sample_id = q.getColumn(0).getInt();
            p.datetime  = static_cast<i64>(q.getColumn(1).getInt64());
            p.value     = q.getColumn(2).getDouble();
            InvokeRowCallback(fn, p);
        }
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "__stop_iteration__") {
            return;
        }
        throw;
    }
}

template void TF::DbManager::ForEachChannelPoint<int(*)(const TF::DbManager::ChannelPoint&)>(int, int, int(*&&)(const ChannelPoint&)) const;

std::vector<TF::DbManager::ChannelPoint> TF::DbManager::QueryChannelBySampleRange(
    int exp_id, int channel_id, int sample_begin, int sample_end_exclusive) const {

    if (sample_end_exclusive <= sample_begin) {
        return {};
    }

    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? AND channel_id=? "
        "AND sample_id>=? AND sample_id<? "
        "ORDER BY sample_id;"
    );
    q.bind(1, exp_id);
    q.bind(2, channel_id);
    q.bind(3, sample_begin);
    q.bind(4, sample_end_exclusive);

    std::vector<ChannelPoint> out;
    while (q.executeStep()) {
        ChannelPoint p;
        p.sample_id = q.getColumn(0).getInt();
        p.datetime  = static_cast<i64>(q.getColumn(1).getInt64());
        p.value     = q.getColumn(2).getDouble();
        out.push_back(p);
    }
    return out;
}

std::vector<TF::DbManager::ChannelPoint> TF::DbManager::QueryChannelByTimeRange(
    int exp_id, int channel_id, i64 t_begin, i64 t_end_exclusive) const {

    if (t_end_exclusive <= t_begin) {
        return {};
    }

    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? AND channel_id=? "
        "AND DateTime>=? AND DateTime<? "
        "ORDER BY DateTime;"
    );
    q.bind(1, exp_id);
    q.bind(2, channel_id);
    q.bind(3, static_cast<std::int64_t>(t_begin));
    q.bind(4, static_cast<std::int64_t>(t_end_exclusive));

    std::vector<ChannelPoint> out;
    while (q.executeStep()) {
        ChannelPoint p;
        p.sample_id = q.getColumn(0).getInt();
        p.datetime  = static_cast<i64>(q.getColumn(1).getInt64());
        p.value     = q.getColumn(2).getDouble();
        out.push_back(p);
    }
    return out;
}

std::vector<int> TF::DbManager::ListExperiments() const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d, "SELECT DISTINCT exp_id FROM Data ORDER BY exp_id;");
    std::vector<int> out;
    while (q.executeStep()) {
        out.push_back(q.getColumn(0).getInt());
    }
    return out;
}

std::vector<int> TF::DbManager::ListChannels(int exp_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT DISTINCT channel_id FROM Data WHERE exp_id=? ORDER BY channel_id;"
    );
    q.bind(1, exp_id);

    std::vector<int> out;
    while (q.executeStep()) {
        out.push_back(q.getColumn(0).getInt());
    }
    return out;
}

std::size_t TF::DbManager::CountExperimentRows(int exp_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d, "SELECT COUNT(*) FROM Data WHERE exp_id=?;");
    q.bind(1, exp_id);
    if (!q.executeStep()) {
        return 0;
    }
    return static_cast<std::size_t>(q.getColumn(0).getInt64());
}

void TF::DbManager::DeleteExperiment(int exp_id) {
    std::scoped_lock lk(mMtx);
    auto& d = db();
    SQLite::Statement st(d, "DELETE FROM Data WHERE exp_id=?;");
    st.bind(1, exp_id);
    st.exec();
}

std::optional<TF::DbManager::ChannelPoint> TF::DbManager::GetLastPoint(int exp_id, int channel_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT sample_id, DateTime, value "
        "FROM Data WHERE exp_id=? AND channel_id=? "
        "ORDER BY sample_id DESC LIMIT 1;"
    );
    q.bind(1, exp_id);
    q.bind(2, channel_id);

    if (!q.executeStep()) {
        return std::nullopt;
    }

    ChannelPoint p;
    p.sample_id = q.getColumn(0).getInt();
    p.datetime  = static_cast<i64>(q.getColumn(1).getInt64());
    p.value     = q.getColumn(2).getDouble();
    return p;
}

void TF::DbManager::UpsertExperiment(const ExperimentInfo& info) {
    std::scoped_lock lk(mMtx);
    auto& d = db();

    SQLite::Transaction txn(d);

    SQLite::Statement st(d,
        "INSERT INTO Experiment(exp_id, name, start_time, end_time) "
        "VALUES(?,?,?,?) "
        "ON CONFLICT(exp_id) DO UPDATE SET "
        "  name=excluded.name, "
        "  start_time=excluded.start_time, "
        "  end_time=excluded.end_time;"
    );

    st.bind(1, info.exp_id);
    st.bind(2, info.name);
    st.bind(3, static_cast<long long>(info.start_time));

    if (info.end_time.has_value()) {
        st.bind(4, static_cast<long long>(*info.end_time));
    } else {
        st.bind(4); // SQLiteCpp：bind(index) 绑定 NULL（常用写法）
    }

    st.exec();
    txn.commit();
}

void TF::DbManager::BeginExperiment(int exp_id, std::string_view name, std::int64_t start_time) {
    ExperimentInfo info;
    info.exp_id = exp_id;
    info.name = std::string{name};
    info.start_time = start_time;
    info.end_time.reset();
    UpsertExperiment(info);
}

void TF::DbManager::EndExperiment(int exp_id, std::int64_t end_time) {
    std::scoped_lock lk(mMtx);
    auto& d = db();

    SQLite::Transaction txn(d);

    SQLite::Statement st(d,
        "UPDATE Experiment SET end_time=? WHERE exp_id=?;"
    );
    st.bind(1, static_cast<long long>(end_time));
    st.bind(2, exp_id);

    st.exec();
    txn.commit();
}

std::optional<TF::DbManager::ExperimentInfo> TF::DbManager::GetExperiment(int exp_id) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    SQLite::Statement q(d,
        "SELECT exp_id, name, start_time, end_time "
        "FROM Experiment WHERE exp_id=?;"
    );
    q.bind(1, exp_id);

    if (!q.executeStep()) {
        return std::nullopt;
    }

    ExperimentInfo info;
    info.exp_id = q.getColumn(0).getInt();
    info.name = q.getColumn(1).getString();
    info.start_time = static_cast<std::int64_t>(q.getColumn(2).getInt64());

    if (q.getColumn(3).isNull()) {
        info.end_time.reset();
    } else {
        info.end_time = static_cast<std::int64_t>(q.getColumn(3).getInt64());
    }
    return info;
}

std::vector<TF::DbManager::ExperimentInfo> TF::DbManager::ListExperiments(int limit, int offset) const {
    std::scoped_lock lk(mMtx);
    const auto& d = db();

    // limit<=0 视为不限制
    std::string sql =
        "SELECT exp_id, name, start_time, end_time "
        "FROM Experiment "
        "ORDER BY start_time DESC ";

    if (limit > 0) {
        sql += "LIMIT ? OFFSET ?;";
    } else {
        sql += ";";
    }

    SQLite::Statement q(d, sql);

    if (limit > 0) {
        q.bind(1, limit);
        q.bind(2, offset);
    }

    std::vector<ExperimentInfo> out;
    while (q.executeStep()) {
        ExperimentInfo info;
        info.exp_id = q.getColumn(0).getInt();
        info.name = q.getColumn(1).getString();
        info.start_time = static_cast<std::int64_t>(q.getColumn(2).getInt64());
        if (q.getColumn(3).isNull()) info.end_time.reset();
        else info.end_time = static_cast<std::int64_t>(q.getColumn(3).getInt64());
        out.push_back(std::move(info));
    }
    return out;
}

void TF::DbManager::DeleteExperimentAll(int exp_id) {
    std::scoped_lock lk(mMtx);
    auto& d = db();

    SQLite::Transaction txn(d);

    {
        SQLite::Statement st(d, "DELETE FROM Data WHERE exp_id=?;");
        st.bind(1, exp_id);
        st.exec();
    }
    {
        SQLite::Statement st(d, "DELETE FROM Experiment WHERE exp_id=?;");
        st.bind(1, exp_id);
        st.exec();
    }

    txn.commit();
}

// ---------------- db(): Get connection ----------------
SQLite::Database& TF::DbManager::db() {
    return *mDB;
}

const SQLite::Database& TF::DbManager::db() const {
    return *mDB;
}
