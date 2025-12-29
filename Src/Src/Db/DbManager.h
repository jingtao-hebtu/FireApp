/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DbManager.h
   Author : tao.jing
   Date   : 2025年12月28日
   Brief  :
**************************************************************************/
#ifndef FIREAPP_DBMANAGER_H
#define FIREAPP_DBMANAGER_H

#include "TSingleton.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <mutex>
#include <type_traits>
#include <concepts>
#include <utility>


namespace SQLite {
    class Database;
};


namespace TF {
    class DbManager : public TBase::TSingleton<DbManager>
    {
    public:
        using i64 = std::int64_t;

        struct DataRow
        {
            int exp_id{};
            int channel_id{};
            int sample_id{};
            i64 datetime{}; // epoch us/ns
            double value{}; // SQLite REAL -> double
        };

        struct ChannelPoint
        {
            int sample_id{};
            i64 datetime{};
            double value{};
        };

        enum class ConflictPolicy
        {
            Abort, // Default: Directly throw an error when a conflict occurs (transaction rollback)
            Ignore, // INSERT OR IGNORE
            Replace // INSERT OR REPLACE
        };

    public:
        void init();

        // ---------- Schema / PRAGMA ----------
        void EnsureSchema(); // Create Data table and index
        void ConfigureForIngest(); // PRAGMA (WAL/timeout/synchronous)
        void WalCheckpoint(bool truncate); // checkpoint WAL

        // ---------- Write Batch ----------
        // One invocation = one transaction
        // Suitable for scenarios where submissions are made per second or every few seconds.
        // Return: The actual number of rows written/modified in this operation.
        std::size_t InsertBatch(std::span<const DataRow> rows,
                                ConflictPolicy policy = ConflictPolicy::Abort);

        // High-throughput Writing: RAII Writer
        // Internally holds transactions + prepared statements, supports multiple appends
        class Writer
        {
        public:
            Writer(Writer&&) noexcept = default;
            Writer& operator=(Writer&&) noexcept = default;

            Writer(const Writer&) = delete;
            Writer& operator=(const Writer&) = delete;

            // Single row and multiples rows append
            std::size_t Add(const DataRow& row);
            std::size_t Add(std::span<const DataRow> rows);

            // Commit transcations
            void Commit();

        private:
            friend class DbManager;
            Writer(DbManager& mgr, ConflictPolicy policy);

            DbManager* mDbMgr{};
            // Lock held throughout the entire write transaction
            // Ensures that the same connection is not used concurrently.
            std::unique_lock<std::mutex> mLock;
            SQLite::Transaction mTxn;
            SQLite::Statement mStmt;
            bool mCommitted{false};
        };

        [[nodiscard]] Writer BeginWriter(ConflictPolicy policy = ConflictPolicy::Abort);

        // ---------- Query by experiment ----------
        // 1) Return all data at once
        //    May occupy a large amount of memory when the data volume is high
        std::vector<DataRow> QueryExperimentAll(int exp_id) const;

        // 2) Streaming Callback
        //    Recommended: low memory footprint; the function can return a boolean to abort traversal
        template <typename Fn>
        void ForEachExperimentRow(int exp_id, Fn&& fn) const;

        // ---------- Query by experiment and channel ----------
        std::vector<ChannelPoint> QueryChannelAll(int exp_id, int channel_id) const;

        template <typename Fn>
        void ForEachChannelPoint(int exp_id, int channel_id, Fn&& fn) const;

        // Range query (by sample_id or DateTime)
        std::vector<ChannelPoint> QueryChannelBySampleRange(
            int exp_id, int channel_id, int sample_begin, int sample_end_exclusive) const;

        std::vector<ChannelPoint> QueryChannelByTimeRange(
            int exp_id, int channel_id, i64 t_begin, i64 t_end_exclusive) const;

        std::vector<int> ListExperiments() const; // SELECT DISTINCT exp_id ...
        std::vector<int> ListChannels(int exp_id) const; // DISTINCT channel_id
        std::size_t CountExperimentRows(int exp_id) const; // COUNT(*)
        void DeleteExperiment(int exp_id); // DELETE FROM Data WHERE exp_id=?
        std::optional<ChannelPoint> GetLastPoint(int exp_id, int channel_id) const;

    private:
        void initParams();

        void initDb();

        SQLite::Database& db();

        const SQLite::Database& db() const;

    private:
        bool mInitialized{false};

        std::string mDBFile;

        SQLite::Database* mDB{nullptr};

        mutable std::mutex mMtx;
    };
};

#endif //FIREAPP_DBMANAGER_H
