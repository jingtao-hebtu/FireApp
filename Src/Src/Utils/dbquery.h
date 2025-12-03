#ifndef DBQUERY_H
#define DBQUERY_H

#include "datacsv.h"
#include "dbconnthread.h"

class DbQuery
{
public:
    //是否从数据库加载
    static bool loadfromdb;

    //载入+清空+添加 录像机信息
    static void loadNvrInfo();
    static void addNvrInfo(const QString &ip, const QString &name, const QString &type);

    //载入+清空+删除+批量删除 摄像机信息
    static void loadIpcInfo();
    static void deleteIpcInfo(const QString &nvrName);
    static void deleteIpcInfos(const QString &ids);
    static void updateIpcInfo(const QString &oldName, const QString &newName);

    //根据编号获取信息比如经纬度
    static void getIpcInfo(int ipcID, QString &ipcPosition);
    static void getIpcInfo(const QString &rtsp, QString &ipcPosition);
};

#endif // DBQUERY_H
