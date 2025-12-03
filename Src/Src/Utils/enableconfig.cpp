#include "enableconfig.h"
#include <QSettings>
#include "appconfig.h"


QString EnableConfig::ConfigFile = "config.ini";

bool EnableConfig::EnableChannelx = true;
bool EnableConfig::EnableChannely = true;
bool EnableConfig::EnableChannel1 = true;
bool EnableConfig::EnableChannel4 = true;
bool EnableConfig::EnableChannel6 = true;
bool EnableConfig::EnableChannel8 = true;
bool EnableConfig::EnableChannel9 = true;
bool EnableConfig::EnableChannel13 = true;
bool EnableConfig::EnableChannel16 = true;
bool EnableConfig::EnableChannel25 = true;
bool EnableConfig::EnableChannel36 = true;
bool EnableConfig::EnableChannel64 = true;

bool EnableConfig::EnableMainVideo = true;
bool EnableConfig::EnableMainMap = true;
bool EnableConfig::EnableMainData = true;
bool EnableConfig::EnableMainOther = false;

bool EnableConfig::EnableVideoPlayLocal = true;
bool EnableConfig::EnableVideoPlayWeb = false;
bool EnableConfig::EnableVideoPlayRtsp = true;
bool EnableConfig::EnableVideoPlayImage = true;
bool EnableConfig::EnableVideoUpload = false;

bool EnableConfig::EnableMapImage = false;
bool EnableConfig::EnableMapDevice = true;
bool EnableConfig::EnableMapGpsMove = false;
bool EnableConfig::EnableMapGpsPlay = false;

bool EnableConfig::EnableDataUser = true;
bool EnableConfig::EnableDataDevice = true;

bool EnableConfig::EnableConfigNvr = true;
bool EnableConfig::EnableConfigIpc = true;
bool EnableConfig::EnableConfigPoll = true;
bool EnableConfig::EnableConfigUser = true;
bool EnableConfig::EnableConfigRecord = true;
bool EnableConfig::EnableConfigOther = true;

bool EnableConfig::EnableResolution = true;
bool EnableConfig::EnableFrameRate = false;
bool EnableConfig::EnableMediaUrl = false;
bool EnableConfig::EnableMediaName = false;
bool EnableConfig::EnableFormatName = false;
bool EnableConfig::EnableVideoCodecName = true;
bool EnableConfig::EnableAudioCodecName = false;
bool EnableConfig::EnableRealBitRate = false;
bool EnableConfig::EnableHardware = false;
bool EnableConfig::EnableVideoMode = false;

bool EnableConfig::EnableTestData = true;
bool EnableConfig::EnableEditGroup = true;
bool EnableConfig::EnableCpuMemory = false;
bool EnableConfig::EnableWeather = false;
bool EnableConfig::EnableVideoPanelTool = true;
bool EnableConfig::EnableTitleMenu = true;
bool EnableConfig::EnableDockMenu = true;
bool EnableConfig::EnableDockMove = true;

void EnableConfig::readConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("ChannelConfig");
    EnableChannelx = set.value("EnableChannelx", EnableChannelx).toBool();
    EnableChannely = set.value("EnableChannely", EnableChannely).toBool();
    EnableChannel1 = set.value("EnableChannel1", EnableChannel1).toBool();
    EnableChannel4 = set.value("EnableChannel4", EnableChannel4).toBool();
    EnableChannel6 = set.value("EnableChannel6", EnableChannel6).toBool();
    EnableChannel8 = set.value("EnableChannel8", EnableChannel8).toBool();
    EnableChannel9 = set.value("EnableChannel9", EnableChannel9).toBool();
    EnableChannel13 = set.value("EnableChannel13", EnableChannel13).toBool();
    EnableChannel16 = set.value("EnableChannel16", EnableChannel16).toBool();
    EnableChannel25 = set.value("EnableChannel25", EnableChannel25).toBool();
    EnableChannel36 = set.value("EnableChannel36", EnableChannel36).toBool();
    EnableChannel64 = set.value("EnableChannel64", EnableChannel64).toBool();
    set.endGroup();

    set.beginGroup("FormMain");
    EnableMainVideo = set.value("EnableMainVideo", EnableMainVideo).toBool();
    EnableMainMap = set.value("EnableMainMap", EnableMainMap).toBool();
    EnableMainData = set.value("EnableMainData", EnableMainData).toBool();
    EnableMainOther = set.value("EnableMainOther", EnableMainOther).toBool();
    set.endGroup();

    set.beginGroup("FormVideo");
    EnableVideoPlayLocal = set.value("EnableVideoPlayLocal", EnableVideoPlayLocal).toBool();
    EnableVideoPlayWeb = set.value("EnableVideoPlayWeb", EnableVideoPlayWeb).toBool();
    EnableVideoPlayRtsp = set.value("EnableVideoPlayRtsp", EnableVideoPlayRtsp).toBool();
    EnableVideoPlayImage = set.value("EnableVideoPlayImage", EnableVideoPlayImage).toBool();
    EnableVideoUpload = set.value("EnableVideoUpload", EnableVideoUpload).toBool();
    set.endGroup();

    set.beginGroup("FormMap");
    EnableMapImage = set.value("EnableMapImage", EnableMapImage).toBool();
    EnableMapDevice = set.value("EnableMapDevice", EnableMapDevice).toBool();
    EnableMapGpsMove = set.value("EnableMapGpsMove", EnableMapGpsMove).toBool();
    EnableMapGpsPlay = set.value("EnableMapGpsPlay", EnableMapGpsPlay).toBool();
    set.endGroup();    

    set.beginGroup("FormData");
    EnableDataUser = set.value("EnableDataUser", EnableDataUser).toBool();
    EnableDataDevice = set.value("EnableDataDevice", EnableDataDevice).toBool();
    set.endGroup();

    set.beginGroup("FormConfig");
    EnableConfigNvr = set.value("EnableConfigNvr", EnableConfigNvr).toBool();
    EnableConfigIpc = set.value("EnableConfigIpc", EnableConfigIpc).toBool();
    EnableConfigPoll = set.value("EnableConfigPoll", EnableConfigPoll).toBool();
    EnableConfigUser = set.value("EnableConfigUser", EnableConfigUser).toBool();
    EnableConfigRecord = set.value("EnableConfigRecord", EnableConfigRecord).toBool();
    EnableConfigOther = set.value("EnableConfigOther", EnableConfigOther).toBool();
    set.endGroup();    

    set.beginGroup("BannerConfig");
    EnableResolution = set.value("EnableResolution", EnableResolution).toBool();
    EnableFrameRate = set.value("EnableFrameRate", EnableFrameRate).toBool();
    EnableMediaUrl = set.value("EnableMediaUrl", EnableMediaUrl).toBool();
    EnableMediaName = set.value("EnableMediaName", EnableMediaName).toBool();
    EnableFormatName = set.value("EnableFormatName", EnableFormatName).toBool();
    EnableVideoCodecName = set.value("EnableVideoCodecName", EnableVideoCodecName).toBool();
    EnableAudioCodecName = set.value("EnableAudioCodecName", EnableAudioCodecName).toBool();
    EnableRealBitRate = set.value("EnableRealBitRate", EnableRealBitRate).toBool();
    EnableHardware = set.value("EnableHardware", EnableHardware).toBool();
    EnableVideoMode = set.value("EnableVideoMode", EnableVideoMode).toBool();
    set.endGroup();

    set.beginGroup("OtherConfig");
    EnableTestData = set.value("EnableTestData", EnableTestData).toBool();
    EnableEditGroup = set.value("EnableEditGroup", EnableEditGroup).toBool();
    EnableCpuMemory = set.value("EnableCpuMemory", EnableCpuMemory).toBool();
    EnableWeather = set.value("EnableWeather", EnableWeather).toBool();
    EnableVideoPanelTool = set.value("EnableVideoPanelTool", EnableVideoPanelTool).toBool();
    EnableTitleMenu = set.value("EnableTitleMenu", EnableTitleMenu).toBool();
    EnableDockMenu = set.value("EnableDockMenu", EnableDockMenu).toBool();
    EnableDockMove = set.value("EnableDockMove", EnableDockMove).toBool();
    set.endGroup();

    EnableConfig::checkConfig();
}

void EnableConfig::checkConfig()
{
    //自动纠正不存在的通道
    int count = AppConfig::VideoCount;
    if (count <= 16) {
        EnableConfig::EnableChannel25 = false;
        EnableConfig::EnableChannel36 = false;
        EnableConfig::EnableChannel64 = false;
    } else if (count <= 25) {
        EnableConfig::EnableChannel36 = false;
        EnableConfig::EnableChannel64 = false;
    } else if (count <= 36) {
        EnableConfig::EnableChannel64 = false;
    }
}

void EnableConfig::writeConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("ChannelConfig");
    set.setValue("EnableChannelx", EnableChannelx);
    set.setValue("EnableChannely", EnableChannely);
    set.setValue("EnableChannel1", EnableChannel1);
    set.setValue("EnableChannel4", EnableChannel4);
    set.setValue("EnableChannel6", EnableChannel6);
    set.setValue("EnableChannel8", EnableChannel8);
    set.setValue("EnableChannel9", EnableChannel9);
    set.setValue("EnableChannel13", EnableChannel13);
    set.setValue("EnableChannel16", EnableChannel16);
    set.setValue("EnableChannel25", EnableChannel25);
    set.setValue("EnableChannel36", EnableChannel36);
    set.setValue("EnableChannel64", EnableChannel64);
    set.endGroup();

    set.beginGroup("FormMain");
    set.setValue("EnableMainVideo", EnableMainVideo);
    set.setValue("EnableMainMap", EnableMainMap);
    set.setValue("EnableMainData", EnableMainData);
    set.setValue("EnableMainOther", EnableMainOther);
    set.endGroup();

    set.beginGroup("FormVideo");
    set.setValue("EnableVideoPlayLocal", EnableVideoPlayLocal);
    set.setValue("EnableVideoPlayWeb", EnableVideoPlayWeb);
    set.setValue("EnableVideoPlayRtsp", EnableVideoPlayRtsp);
    set.setValue("EnableVideoPlayImage", EnableVideoPlayImage);
    set.setValue("EnableVideoUpload", EnableVideoUpload);
    set.endGroup();

    set.beginGroup("FormMap");
    set.setValue("EnableMapImage", EnableMapImage);
    set.setValue("EnableMapDevice", EnableMapDevice);
    set.setValue("EnableMapGpsMove", EnableMapGpsMove);
    set.setValue("EnableMapGpsPlay", EnableMapGpsPlay);
    set.endGroup();    

    set.beginGroup("FormData");
    set.setValue("EnableDataUser", EnableDataUser);
    set.setValue("EnableDataDevice", EnableDataDevice);
    set.endGroup();

    set.beginGroup("FormConfig");
    set.setValue("EnableConfigNvr", EnableConfigNvr);
    set.setValue("EnableConfigIpc", EnableConfigIpc);
    set.setValue("EnableConfigPoll", EnableConfigPoll);
    set.setValue("EnableConfigUser", EnableConfigUser);
    set.setValue("EnableConfigRecord", EnableConfigRecord);
    set.setValue("EnableConfigOther", EnableConfigOther);
    set.endGroup();    

    set.beginGroup("BannerConfig");
    set.setValue("EnableResolution", EnableResolution);
    set.setValue("EnableFrameRate", EnableFrameRate);
    set.setValue("EnableMediaUrl", EnableMediaUrl);
    set.setValue("EnableMediaName", EnableMediaName);
    set.setValue("EnableFormatName", EnableFormatName);
    set.setValue("EnableVideoCodecName", EnableVideoCodecName);
    set.setValue("EnableAudioCodecName", EnableAudioCodecName);
    set.setValue("EnableRealBitRate", EnableRealBitRate);
    set.setValue("EnableHardware", EnableHardware);
    set.setValue("EnableVideoMode", EnableVideoMode);
    set.endGroup();

    set.beginGroup("OtherConfig");
    set.setValue("EnableTestData", EnableTestData);
    set.setValue("EnableEditGroup", EnableEditGroup);
    set.setValue("EnableCpuMemory", EnableCpuMemory);
    set.setValue("EnableWeather", EnableWeather);
    set.setValue("EnableVideoPanelTool", EnableVideoPanelTool);
    set.setValue("EnableTitleMenu", EnableTitleMenu);
    set.setValue("EnableDockMenu", EnableDockMenu);
    set.setValue("EnableDockMove", EnableDockMove);
    set.endGroup();
}
