/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : otherconfig.cpp
   Author : tao.jing
   Date   : 2025/1/10
   Brief  :
**************************************************************************/
#include "otherconfig.h"
#include <QStringList>
#include <QSettings>


QString OtherConfig::ConfigFile = "config.ini";

bool OtherConfig::CheckOffline = false;
bool OtherConfig::TreeBig = false;
bool OtherConfig::TreeRtsp = true;
bool OtherConfig::HideNoDevice = false;
int OtherConfig::RtspType = 1;
int OtherConfig::OpenIpcPolicy = 1;
int OtherConfig::OpenNvrPolicy = 1;
int OtherConfig::OpenMaxCount = 2;

QStringList OtherConfig::DeviceGroupFId = QString("|1001").split("|");
QStringList OtherConfig::DeviceGroupId = QString("1001|1002").split("|");
QStringList OtherConfig::DeviceGroupName = QString::fromUtf8("默认分组|网络视频").split("|");
QStringList OtherConfig::DeviceExpandItem = QString("").split("|");

QStringList OtherConfig::EventLevel = QString("1|1|1|1|1|1").split("|");
QStringList OtherConfig::EventName = QString("IsMotion|IsInside|LogicalState|State|IsTamper|AlarmActive").split("|");
QStringList OtherConfig::EventAlarm = QString::fromUtf8("有人在移动|有人在闯入|开关量报警|开关量联动|遮挡报警|温度报警").split("|");
QStringList OtherConfig::EventNormal = QString::fromUtf8("移动结束|入侵结束|报警结束|联动结束|遮挡结束|温度正常").split("|");

int OtherConfig::MapCore = 1;
bool OtherConfig::MapLocal = false;
int OtherConfig::MapZoom = 19;
QString OtherConfig::MapCenterPoint = "121.413937,31.182925";
QString OtherConfig::MapVersionKey = "123456789abc";

QString OtherConfig::UploadHost = "127.0.0.1";
int OtherConfig::UploadPort = 6000;

QString OtherConfig::WeatherHttp = "http://www.nmc.cn";
QString OtherConfig::WeatherCity = QString::fromUtf8("北京市|北京市|东城区");
int OtherConfig::WeatherInterval = 60;
int OtherConfig::WeatherStyle = 4;

QString OtherConfig::VideoNormalPath = "./video_normal";
QString OtherConfig::VideoAlarmPath = "./video_alarm";
QString OtherConfig::ImageNormalPath = "./image_normal";
QString OtherConfig::ImageAlarmPath = "./image_alarm";

QString OtherConfig::PortNameA = "COM1";
QString OtherConfig::PortNameB = "COM2";
QString OtherConfig::PortNameC = "COM3";
QString OtherConfig::PortNameD = "COM4";
int OtherConfig::BaudRateA = 9600;
int OtherConfig::BaudRateB = 9600;
int OtherConfig::BaudRateC = 115200;
int OtherConfig::BaudRateD = 9600;

QString OtherConfig::TcpServerHost = "127.0.0.1";
int OtherConfig::TcpServerPort = 6000;
int OtherConfig::TcpListenPort = 6000;
int OtherConfig::UdpListenPort = 6000;

QString OtherConfig::HttpRequestUrl = "http://218.0.49.23:38080";
QString OtherConfig::HttpPlayUrl = "http://218.0.49.23:1580";
QString OtherConfig::HttpVodUrl = "http://218.0.49.23:38081";
QString OtherConfig::HttpToken = "";

void OtherConfig::readConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("TreeConfig");
    CheckOffline = set.value("CheckOffline", CheckOffline).toBool();
    TreeBig = set.value("TreeBig", TreeBig).toBool();
    TreeRtsp = set.value("TreeRtsp", TreeRtsp).toBool();
    HideNoDevice = set.value("HideNoDevice", HideNoDevice).toBool();
    RtspType = set.value("RtspType", RtspType).toInt();
    OpenIpcPolicy = set.value("OpenIpcPolicy", OpenIpcPolicy).toInt();
    OpenNvrPolicy = set.value("OpenNvrPolicy", OpenNvrPolicy).toInt();
    OpenMaxCount = set.value("OpenMaxCount", OpenMaxCount).toInt();    
    set.endGroup();

    set.beginGroup("DeviceGroup");
    DeviceGroupFId = set.value("DeviceGroupFId", DeviceGroupFId).toStringList();
    DeviceGroupId = set.value("DeviceGroupId", DeviceGroupId).toStringList();
    DeviceGroupName = set.value("DeviceGroupName", DeviceGroupName).toStringList();
    DeviceExpandItem = set.value("DeviceExpandItem", DeviceExpandItem).toStringList();
    set.endGroup();

    set.beginGroup("EventConfig");
    EventLevel = set.value("EventLevel", EventLevel).toStringList();
    EventName = set.value("EventName", EventName).toStringList();
    EventAlarm = set.value("EventAlarm", EventAlarm).toStringList();
    EventNormal = set.value("EventNormal", EventNormal).toStringList();
    set.endGroup();

    set.beginGroup("MapConfig");
    MapCore = set.value("MapCore", MapCore).toInt();
    MapLocal = set.value("MapLocal", MapLocal).toBool();
    MapZoom = set.value("MapZoom", MapZoom).toInt();
    MapCenterPoint = set.value("MapCenterPoint", MapCenterPoint).toString();
    MapVersionKey = set.value("MapVersionKey", MapVersionKey).toString();
    set.endGroup();

    set.beginGroup("UploadConfig");
    UploadHost = set.value("UploadHost", UploadHost).toString();
    UploadPort = set.value("UploadPort", UploadPort).toInt();
    set.endGroup();

    set.beginGroup("WeatherConfig");
    WeatherHttp = set.value("WeatherHttp", WeatherHttp).toString();
    WeatherCity = set.value("WeatherCity", WeatherCity).toString();
    WeatherInterval = set.value("WeatherInterval", WeatherInterval).toInt();
    WeatherStyle = set.value("WeatherStyle", WeatherStyle).toInt();
    set.endGroup();

    set.beginGroup("PathConfig");
    VideoNormalPath = set.value("VideoNormalPath", VideoNormalPath).toString();
    VideoAlarmPath = set.value("VideoAlarmPath", VideoAlarmPath).toString();
    ImageNormalPath = set.value("ImageNormalPath", ImageNormalPath).toString();
    ImageAlarmPath = set.value("ImageAlarmPath", ImageAlarmPath).toString();
    set.endGroup();

    set.beginGroup("ComConfig");
    PortNameA = set.value("PortNameA", PortNameA).toString();
    PortNameB = set.value("PortNameB", PortNameB).toString();
    PortNameC = set.value("PortNameC", PortNameC).toString();
    PortNameD = set.value("PortNameD", PortNameD).toString();
    BaudRateA = set.value("BaudRateA", BaudRateA).toInt();
    BaudRateB = set.value("BaudRateB", BaudRateB).toInt();
    BaudRateC = set.value("BaudRateC", BaudRateC).toInt();
    BaudRateD = set.value("BaudRateD", BaudRateD).toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    TcpServerHost = set.value("TcpServerHost", TcpServerHost).toString();
    TcpServerPort = set.value("TcpServerPort", TcpServerPort).toInt();
    TcpListenPort = set.value("TcpListenPort", TcpListenPort).toInt();
    UdpListenPort = set.value("UdpListenPort", UdpListenPort).toInt();
    set.endGroup();

    set.beginGroup("HttpConfig");
    HttpRequestUrl = set.value("HttpRequestUrl", HttpRequestUrl).toString();
    HttpPlayUrl = set.value("HttpPlayUrl", HttpPlayUrl).toString();
    HttpVodUrl = set.value("HttpVodUrl", HttpVodUrl).toString();
    set.endGroup();

    OtherConfig::checkConfig();
}

void OtherConfig::checkConfig()
{

}

void OtherConfig::writeConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("TreeConfig");
    set.setValue("CheckOffline", CheckOffline);
    set.setValue("TreeBig", TreeBig);
    set.setValue("TreeRtsp", TreeRtsp);
    set.setValue("HideNoDevice", HideNoDevice);
    set.setValue("RtspType", RtspType);
    set.setValue("OpenIpcPolicy", OpenIpcPolicy);
    set.setValue("OpenNvrPolicy", OpenNvrPolicy);
    set.setValue("OpenMaxCount", OpenMaxCount);
    set.endGroup();

    set.beginGroup("DeviceGroup");
    set.setValue("DeviceGroupFId", DeviceGroupFId);
    set.setValue("DeviceGroupId", DeviceGroupId);
    set.setValue("DeviceGroupName", DeviceGroupName);
    set.setValue("DeviceExpandItem", DeviceExpandItem);
    set.endGroup();

    set.beginGroup("EventConfig");
    set.setValue("EventLevel", EventLevel);
    set.setValue("EventName", EventName);
    set.setValue("EventAlarm", EventAlarm);
    set.setValue("EventNormal", EventNormal);
    set.endGroup();

    set.beginGroup("MapConfig");
    set.setValue("MapCore", MapCore);
    set.setValue("MapLocal", MapLocal);
    set.setValue("MapZoom", MapZoom);
    set.setValue("MapCenterPoint", MapCenterPoint);
    set.setValue("MapVersionKey", MapVersionKey);
    set.endGroup();

    set.beginGroup("UploadConfig");
    set.setValue("UploadHost", UploadHost);
    set.setValue("UploadPort", UploadPort);
    set.endGroup();

    set.beginGroup("WeatherConfig");
    set.setValue("WeatherHttp", WeatherHttp);
    set.setValue("WeatherCity", WeatherCity);
    set.setValue("WeatherInterval", WeatherInterval);
    set.setValue("WeatherStyle", WeatherStyle);
    set.endGroup();

    set.beginGroup("PathConfig");
    set.setValue("VideoNormalPath", VideoNormalPath);
    set.setValue("VideoAlarmPath", VideoAlarmPath);
    set.setValue("ImageNormalPath", ImageNormalPath);
    set.setValue("ImageAlarmPath", ImageAlarmPath);
    set.endGroup();

    set.beginGroup("ComConfig");
    set.setValue("PortNameA", PortNameA);
    set.setValue("PortNameB", PortNameB);
    set.setValue("PortNameC", PortNameC);
    set.setValue("PortNameD", PortNameD);
    set.setValue("BaudRateA", BaudRateA);
    set.setValue("BaudRateB", BaudRateB);
    set.setValue("BaudRateC", BaudRateC);
    set.setValue("BaudRateD", BaudRateD);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("TcpServerHost", TcpServerHost);
    set.setValue("TcpServerPort", TcpServerPort);
    set.setValue("TcpListenPort", TcpListenPort);
    set.setValue("UdpListenPort", UdpListenPort);
    set.endGroup();

    set.beginGroup("HttpConfig");
    set.setValue("HttpRequestUrl", HttpRequestUrl);
    set.setValue("HttpPlayUrl", HttpPlayUrl);
    set.setValue("HttpVodUrl", HttpVodUrl);
    set.endGroup();
}
