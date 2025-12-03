#include "dbquery.h"
#include "qthelper.h"
#include "dbdata.h"
#include "otherconfig.h"

bool DbQuery::loadfromdb = false;

#include "devicetree.h"
void DbQuery::loadNvrInfo()
{
    DbData::NvrInfo_Count = 0;
    DbData::NvrInfo_NvrID.clear();
    DbData::NvrInfo_NvrName.clear();
    DbData::NvrInfo_NvrType.clear();
    DbData::NvrInfo_NvrIP.clear();
    DbData::NvrInfo_UserName.clear();
    DbData::NvrInfo_UserPwd.clear();
    DbData::NvrInfo_NvrEnable.clear();

    QString sql = "select NvrID,NvrName,NvrType,NvrIP,UserName,UserPwd,NvrEnable from NvrInfo order by NvrID asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int nvrID = query.value(0).toInt();
        QString nvrName = query.value(1).toString();
        QString nvrType = query.value(2).toString();
        QString nvrIP = query.value(3).toString();
        QString userName = query.value(4).toString();
        QString userPwd = query.value(5).toString();
        QString nvrEnable = query.value(6).toString();
        if (nvrEnable != "启用") {
            continue;
        }

        DbData::NvrInfo_Count++;
        DbData::NvrInfo_NvrID << nvrID;
        DbData::NvrInfo_NvrName << nvrName;
        DbData::NvrInfo_NvrType << nvrType;
        DbData::NvrInfo_NvrIP << nvrIP;
        DbData::NvrInfo_UserName << userName;
        DbData::NvrInfo_UserPwd << userPwd;
        DbData::NvrInfo_NvrEnable << nvrEnable;
    }

    //把录像机的名称加在设备分组最前面
    OtherConfig::DeviceGroupFId.clear();
    OtherConfig::DeviceGroupId.clear();
    OtherConfig::DeviceGroupName.clear();
    for (int i = 0; i < DbData::NvrInfo_Count; ++i) {
        QString name = DbData::NvrInfo_NvrName.at(i);
        if (OtherConfig::DeviceGroupName.contains(name)) {
            continue;
        }

        OtherConfig::DeviceGroupFId << "";
        OtherConfig::DeviceGroupId << DeviceTree::getId();
        OtherConfig::DeviceGroupName << name;
    }

    OtherConfig::writeConfig();
}

void DbQuery::addNvrInfo(const QString &ip, const QString &name, const QString &type)
{
    QString sql = "insert into NvrInfo(NvrID,NvrName,NvrType,NvrIP,UserName,UserPwd,NvrEnable) values";
    QStringList values;
    values << QString::number(DbHelper::getMaxID("NvrInfo", "NvrID") + 1);
    values << name;
    //后期这里可以根据搜索那边传过来的NVR类型进行对应查找
    values << type;
    values << ip;
    values << "admin";
    values << "123456";
    values << "启用";

    sql = QString("%1('%2')").arg(sql).arg(values.join("','"));
    DbHelper::execSql(sql);
}

void DbQuery::loadIpcInfo()
{
    DbData::IpcInfo_Count = 0;
    DbData::IpcInfo_IpcID.clear();
    DbData::IpcInfo_IpcName.clear();
    DbData::IpcInfo_NvrName.clear();
    DbData::IpcInfo_IpcType.clear();
    DbData::IpcInfo_OnvifAddr.clear();
    DbData::IpcInfo_ProfileToken.clear();
    DbData::IpcInfo_VideoSource.clear();
    DbData::IpcInfo_RtspMain.clear();
    DbData::IpcInfo_RtspSub.clear();
    DbData::IpcInfo_IpcPosition.clear();
    DbData::IpcInfo_IpcImage.clear();
    DbData::IpcInfo_IpcX.clear();
    DbData::IpcInfo_IpcY.clear();
    DbData::IpcInfo_UserName.clear();
    DbData::IpcInfo_UserPwd.clear();
    DbData::IpcInfo_IpcEnable.clear();
    DbData::IpcInfo_IpcOnline.clear();

    QString column1 = "IpcID,IpcName,NvrName,IpcType";
    QString column2 = "OnvifAddr,ProfileToken,VideoSource,RtspMain,RtspSub";
    QString column3 = "IpcPosition,IpcImage,IpcX,IpcY,UserName,UserPwd,IpcEnable";
    QString sql = QString("select %1,%2,%3 from IpcInfo order by IpcID asc").arg(column1).arg(column2).arg(column3);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int ipcID = query.value(0).toInt();
        QString ipcName = query.value(1).toString();
        QString nvrName = query.value(2).toString();
        QString ipcType = query.value(3).toString();
        QString onvifAddr = query.value(4).toString();
        QString profileToken = query.value(5).toString();
        QString videoSource = query.value(6).toString();
        QString rtspMain = query.value(7).toString();
        QString rtspSub = query.value(8).toString();
        QString ipcPosition = query.value(9).toString();
        QString ipcImage = query.value(10).toString();
        int ipcX = query.value(11).toInt();
        int ipcY = query.value(12).toInt();
        QString userName = query.value(13).toString();
        QString userPwd = query.value(14).toString();
        QString ipcEnable = query.value(15).toString();
        if (ipcEnable != "启用") {
            continue;
        }

        //如果经纬度坐标为空则默认一个
        if (ipcPosition.isEmpty()) {
            ipcPosition = OtherConfig::MapCenterPoint;
            ipcPosition.replace(",", "|");
        }

        DbData::IpcInfo_Count++;
        DbData::IpcInfo_IpcID << ipcID;
        DbData::IpcInfo_IpcName << ipcName;
        DbData::IpcInfo_NvrName << nvrName;
        DbData::IpcInfo_IpcType << ipcType;
        DbData::IpcInfo_OnvifAddr << onvifAddr;
        DbData::IpcInfo_ProfileToken << profileToken;
        DbData::IpcInfo_VideoSource << videoSource;
        DbData::IpcInfo_RtspMain << rtspMain;
        DbData::IpcInfo_RtspSub << rtspSub;
        DbData::IpcInfo_IpcPosition << ipcPosition;
        DbData::IpcInfo_IpcImage << ipcImage;
        DbData::IpcInfo_IpcX << ipcX;
        DbData::IpcInfo_IpcY << ipcY;
        DbData::IpcInfo_UserName << userName;
        DbData::IpcInfo_UserPwd << userPwd;
        DbData::IpcInfo_IpcEnable << ipcEnable;
        DbData::IpcInfo_IpcOnline << false;
    }
}


void DbQuery::deleteIpcInfo(const QString &nvrName)
{
    QString sql = QString("delete from IpcInfo where NvrName='%1'").arg(nvrName);
    DbHelper::execSql(sql);
}

void DbQuery::deleteIpcInfos(const QString &ids)
{
    //合并成一条语句删除
    QString sql = QString("delete from IpcInfo where IpcID in (%1)").arg(ids);
    DbHelper::execSql(sql);
}

void DbQuery::updateIpcInfo(const QString &oldName, const QString &newName)
{
    QString sql = QString("update IpcInfo set NvrName='%1' where NvrName='%2'").arg(newName).arg(oldName);
    DbHelper::execSql(sql);
}

void DbQuery::getIpcInfo(int ipcID, QString &ipcPosition)
{
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        if (DbData::IpcInfo_IpcID.at(i) == ipcID) {
            ipcPosition = DbData::IpcInfo_IpcPosition.at(i);
            break;
        }
    }
}

void DbQuery::getIpcInfo(const QString &rtsp, QString &ipcPosition)
{
    int index = DbData::getIpcIndex(rtsp);
    if (index >= 0) {
        ipcPosition = DbData::IpcInfo_IpcPosition.at(index);
    }
}
