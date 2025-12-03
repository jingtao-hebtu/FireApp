/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : otherconfig.h
   Author : tao.jing
   Date   : 2025/1/10
   Brief  :
**************************************************************************/
#ifndef OTHERCONFIG_H
#define OTHERCONFIG_H

#include <QString>


class OtherConfig
{
public:
    static QString ConfigFile;      //配置文件文件路径及名称

    //树状控件配置参数
    static bool CheckOffline;       //检测设备离线
    static bool TreeBig;            //设备列表放大
    static bool TreeRtsp;           //显示码流节点
    static bool HideNoDevice;       //隐藏空值分组
    static int RtspType;            //默认码流类型
    static int OpenIpcPolicy;       //双击打开设备对应策略 0-最后空白通道 1-最后按下通道
    static int OpenNvrPolicy;       //双击打开分组对应策略 0-最后空白通道 1-从头清空通道
    static int OpenMaxCount;        //双击打开分组最大数量 0-最大通道数量 1-当前通道数量 2-自动调整数量

    //设备分组配置参数
    static QStringList DeviceGroupFId;  //父节点标识
    static QStringList DeviceGroupId;   //子节点标识
    static QStringList DeviceGroupName; //节点名称集合
    static QStringList DeviceExpandItem;//展开节点集合

    //警情字典配置参数
    static QStringList EventLevel;  //事件等级
    static QStringList EventName;   //名称标识
    static QStringList EventAlarm;  //报警字样
    static QStringList EventNormal; //解除字样

    //天气采集配置参数
    static QString WeatherHttp;     //天气网址
    static QString WeatherCity;     //城市名称
    static int WeatherInterval;     //采集间隔(单位秒 0-不采集)
    static int WeatherStyle;        //展示样式

    //网络地图配置参数
    static int MapCore;             //地图内核
    static bool MapLocal;           //离线地图
    static int MapZoom;             //缩放级别
    static QString MapCenterPoint;  //中心点坐标
    static QString MapVersionKey;   //秘钥字符串

    //文件上传配置参数
    static QString UploadHost;      //文件服务器地址
    static int UploadPort;          //文件服务器端口

    //存储路径配置参数
    static QString VideoNormalPath; //正常视频存储目录
    static QString VideoAlarmPath;  //报警视频存储目录
    static QString ImageNormalPath; //正常图片存储目录
    static QString ImageAlarmPath;  //报警图片存储目录

    //串口通信配置参数
    static QString PortNameA;       //通信串口A
    static QString PortNameB;       //通信串口B
    static QString PortNameC;       //通信串口C
    static QString PortNameD;       //通信串口D
    static int BaudRateA;           //通信串口A波特率
    static int BaudRateB;           //通信串口B波特率
    static int BaudRateC;           //通信串口C波特率
    static int BaudRateD;           //通信串口D波特率

    //网络通信配置参数
    static QString TcpServerHost;   //Tcp通信地址
    static int TcpServerPort;       //Tcp通信端口
    static int TcpListenPort;       //Tcp监听端口
    static int UdpListenPort;       //Udp监听端口

    //网络请求配置参数
    static QString HttpRequestUrl;  //接口地址
    static QString HttpPlayUrl;     //播放地址
    static QString HttpVodUrl;      //回放地址
    static QString HttpToken;       //请求标识

    //读写校验配置文件
    static void readConfig();       //读取配置文件
    static void checkConfig();      //检验配置参数
    static void writeConfig();      //写入配置文件
};

#endif // OTHERCONFIG_H
