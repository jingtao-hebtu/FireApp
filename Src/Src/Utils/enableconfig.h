#ifndef ENABLECONFIG_H
#define ENABLECONFIG_H

#include <QString>


class EnableConfig
{
public:
    static QString ConfigFile;      //配置文件文件路径及名称

    //通道启用配置参数
    static bool EnableChannelx;     //启用x画面切换
    static bool EnableChannely;     //启用y画面切换
    static bool EnableChannel1;     //启用1画面切换
    static bool EnableChannel4;     //启用4画面切换
    static bool EnableChannel6;     //启用6画面切换
    static bool EnableChannel8;     //启用8画面切换
    static bool EnableChannel9;     //启用9画面切换
    static bool EnableChannel13;    //启用13画面切换
    static bool EnableChannel16;    //启用16画面切换
    static bool EnableChannel25;    //启用25画面切换
    static bool EnableChannel36;    //启用36画面切换
    static bool EnableChannel64;    //启用64画面切换

    //主界面启用配置参数
    static bool EnableMainVideo;    //启用视频回放
    static bool EnableMainMap;      //启用电子地图
    static bool EnableMainData;     //启用日志记录
    static bool EnableMainOther;    //启用备用模块

    //视频回放模块配置参数
    static bool EnableVideoPlayLocal;//启用网络回放模块
    static bool EnableVideoPlayWeb; //启用网络回放模块
    static bool EnableVideoPlayRtsp;//启用远程回放模块
    static bool EnableVideoPlayImage;//启用图片回放模块
    static bool EnableVideoUpload;  //启用视频上传模块

    //电子地图模块配置参数
    static bool EnableMapImage;     //启用图片地图模块/加载后可能会把界面撑起
    static bool EnableMapDevice;    //启用设备地图模块
    static bool EnableMapGpsMove;   //启用设备移动模块
    static bool EnableMapGpsPlay;   //启用轨迹回放模块

    //日志记录模块配置参数
    static bool EnableDataUser;     //启用本地日志模块
    static bool EnableDataDevice;   //启用设备日志模块

    //系统设置模块配置参数
    static bool EnableConfigNvr;    //启用录像机管理模块
    static bool EnableConfigIpc;    //启用摄像机管理模块
    static bool EnableConfigPoll;   //启用轮询配置模块
    static bool EnableConfigUser;   //启用用户管理模块
    static bool EnableConfigRecord; //启用录像计划模块
    static bool EnableConfigOther;  //启用其他配置模块    

    //悬浮条配置参数
    static bool EnableResolution;   //悬浮条显示分辨率
    static bool EnableFrameRate;    //悬浮条显示帧率
    static bool EnableMediaUrl;     //悬浮条显示设备地址
    static bool EnableMediaName;    //悬浮条显示设备名称
    static bool EnableFormatName;   //悬浮条显示封装格式
    static bool EnableVideoCodecName;//悬浮条显示视频解码格式
    static bool EnableAudioCodecName;//悬浮条显示音频解码格式
    static bool EnableRealBitRate;  //悬浮条显示实时码率
    static bool EnableHardware;     //悬浮条显示硬解名称
    static bool EnableVideoMode;    //悬浮条显示视频模式

    //其他窗体配置参数
    static bool EnableTestData;     //启用模拟数据
    static bool EnableEditGroup;    //启用分组编辑
    static bool EnableCpuMemory;    //启用CPU内存显示
    static bool EnableWeather;      //启用天气预报模块
    static bool EnableVideoPanelTool;//启用视频底部工具栏
    static bool EnableTitleMenu;    //启用顶部标题右键菜单
    static bool EnableDockMenu;     //启用悬停模块右键菜单
    static bool EnableDockMove;     //启用悬停模块拖动位置

    //读写校验配置文件
    static void readConfig();       //读取配置文件
    static void checkConfig();      //检验配置参数
    static void writeConfig();      //写入配置文件
};

#endif // ENABLECONFIG_H
