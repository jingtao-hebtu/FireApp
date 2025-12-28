/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DbManager.cpp
   Author : tao.jing
   Date   : 2025.12.28
   Brief  :
**************************************************************************/
#include "DbManager.h"

# if 0
#include "SQLiteCpp.h"
#include "TConfig.h"
#include "TSysUtils.h"
#include "PathConfig.h"
#include "TLog.h"
#include <iostream>


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

#endif