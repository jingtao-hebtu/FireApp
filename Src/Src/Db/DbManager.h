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

#if 0

#include "TSingleton.h"
#include <string>
#include <vector>


namespace SQLite {
    class Database;
};


namespace TF {

    class DbManager : public TBase::TSingleton<DbManager>
    {

    public:
        void init();

    private:
        void initParams();

        void initDb();


    private:
        bool mInitialized{false};

        std::string mDBFile;

        SQLite::Database *mDB {nullptr};

    };

};

#endif

#endif //FIREAPP_DBMANAGER_H