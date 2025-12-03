#ifndef VIDEOSTRUCT_H
#define VIDEOSTRUCT_H

#include <QString>
#include <QDebug>

//解析内核
enum VideoCore {
    VideoCore_None = 0,         //不解析处理(默认值)

    VideoCore_QMedia = 1,       //采用qmedia解析(qt自带且依赖本地解码器且部分平台支持)
    VideoCore_FFmpeg = 2,       //采用ffmpeg解析(通用性最好)
    VideoCore_Vlc = 3,          //采用vlc解析(支持本地文件最好)
    VideoCore_Mpv = 4,          //采用mpv解析(支持本地文件最好且跨平台最多)
    VideoCore_Mdk = 5,          //采用mdk解析(框架精简跨平台好)
    VideoCore_Qtav = 6,         //采用qtav解析(框架结构最好/基于ffmpeg)

    VideoCore_HaiKang = 10,     //采用海康sdk解析
    VideoCore_DaHua = 11,       //采用大华sdk解析
    VideoCore_UniView = 12,     //采用宇视sdk解析

    VideoCore_EasyPlayer = 20   //采用easyplayer解析
};

//视频类型
enum MediaType {
    MediaType_FileLocal = 1,    //本地文件
    MediaType_FileWeb = 2,      //网络文件(可以是http/rtsp等)
    MediaType_Device = 3,       //本地设备(音视频输入设备)
    MediaType_Screen = 4,       //本地屏幕
    MediaType_Rtsp = 10,        //视频流rtsp
    MediaType_Rtmp = 11,        //视频流rtmp
    MediaType_Http = 12,        //视频流http
    MediaType_Srt = 13,         //视频流srt
    MediaType_Udp = 14,         //视频流udp
    MediaType_Tcp = 15,         //视频流tcp
    MediaType_WebSocket = 16,   //视频流websocket
    MediaType_Webrtc = 16,      //视频流webrtc
    MediaType_Other = 255       //其他未知
};

//解码处理策略
enum DecodeType {
    DecodeType_Fast = 0,        //速度优先
    DecodeType_Full = 1,        //质量优先
    DecodeType_Even = 2,        //均衡处理
    DecodeType_Fastest = 3      //最快速度(速度优先且不做任何音视频同步)
};

//视频采集参数
struct VideoPara {
    VideoCore videoCore;        //解析内核
    QString mediaUrl;           //媒体地址
    QString bufferSize;         //缓存分辨率
    int frameRate;              //视频帧率
    int offsetX;                //桌面坐标X
    int offsetY;                //桌面坐标Y

    DecodeType decodeType;      //解码处理策略
    QString hardware;           //硬件加速名称
    QString transport;          //通信协议(tcp/udp)
    int caching;                //缓存时间(不同内核不同单位/vlc是ms/ffmpeg是mb)

    bool audioLevel;            //开启音频振幅
    bool decodeAudio;           //解码音频数据
    bool playAudio;             //解码播放声音
    bool playRepeat;            //重复循环播放
    bool playStep;              //是否逐帧播放
    bool realBitRate;           //统计实时码率

    QString encryptKey;         //加密秘钥字串
    QString decryptKey;         //解密秘钥字串
    QString audioDevice;        //音频输出设备
    QString codecName;          //指定解码名称(本地设备专用)

    int openSleepTime;          //打开休息时间(最低1000/单位毫秒)
    int readTimeout;            //采集超时时间(0=不处理/单位毫秒)
    int connectTimeout;         //连接超时时间(0=不处理/单位毫秒)

    VideoPara() {
        videoCore = VideoCore_None;
        mediaUrl = "";
        bufferSize = "0x0";
        frameRate = 0;
        offsetX = 0;
        offsetY = 0;

        decodeType = DecodeType_Fast;
        hardware = "none";
        transport = "auto";
        caching = 0;

        audioLevel = false;
        decodeAudio = true;
        playAudio = true;
        playRepeat = false;
        playStep = false;
        realBitRate = false;

        encryptKey = "";
        decryptKey = "";
        audioDevice = "";
        codecName = "";

        openSleepTime = 3000;
        readTimeout = 0;
        connectTimeout = 500;
    }

    void reset() {
        mediaUrl = "";
        bufferSize = "0x0";
        frameRate = 0;
        offsetX = 0;
        offsetY = 0;

        encryptKey = "";
        decryptKey = "";
        audioDevice = "";
        codecName = "";
    }
};

#endif // VIDEOSTRUCT_H
