#ifndef VIDEOHELPER_H
#define VIDEOHELPER_H

#include "videohead.h"

class VideoWidget;

class VideoHelper {
public:
    //获取当前视频内核版本
    static QString getVersion();

    static QString getVersion(const VideoCore &videoCore);

    //设置解码内核(type可以指定内核/否则按照定义的优先级)
    static VideoCore getVideoCore(int type = 0);

    //检查保存的文件是否符合要求
    static bool checkSaveFile(const QString &fullName, const QString &fileName, bool isffmpeg);

    //重命名录制的文件(vlc内核专用)
    static void renameFile(const QString &fileName);

    //鼠标指针复位
    static void resetCursor();

    //根据旋转角度旋转图片
    static void rotateImage(int rotate, QImage &image);

    //根据旋转度数交换宽高
    static void rotateSize(int rotate, int &width, int &height);

    //根据旧的范围值和值计算新的范围值对应的值
    static int getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax);

    //检查地址是否正常(文件是否存在或者网络地址是否可达)
    static bool
    checkUrl(VideoThread *videoThread, const MediaType &mediaType, const QString &mediaUrl, int timeout = 500);

    //获取封装格式
    static QString getFormatName(VideoThread *videoThread);

    //获取最终播放的地址(有些视频流带了用户信息有特殊字符需要先转义)
    static QString getPlayUrl(const VideoCore &videoCore, const MediaType &mediaType, const QString &mediaUrl);

    //qmedia内核专用/区分本地文件和网络文件
    static QUrl getPlayUrl(const MediaType &mediaType, const QString &mediaUrl);

    //ffmpeg内核专用/获取播放地址字节数组
    static QByteArray getUrlData(const MediaType &mediaType, const QString &mediaUrl, bool gbk);

    //根据地址获取类型
    static MediaType getMediaType(const QString &mediaUrl);

    //获取本地音视频设备
    static void getDeviceName(const QString &mediaUrl, QString &audioDevice, QString &videoDevice);

    //根据地址获取本地设备参数
    static void getDevicePara(QString &mediaUrl, QString &bufferSize, int &frameRate, QString &codecName, int &offsetX,
                              int &offsetY, float &encodeVideoRatio, QString &encodeVideoScale);

    //根据地址获取通用地址参数
    static void getNormalPara(QString &mediaUrl, QString &transport, DecodeType &decodeType, EncodeVideo &encodeVideo,
                              int &encodeVideoFps, float &encodeVideoRatio, QString &encodeVideoScale);

    //对参数进行矫正
    static MediaType initPara(WidgetPara &widgetPara, VideoPara &videoPara, EncodePara &encodePara);

    //创建视频采集类
    static VideoThread *newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode);

    //对采集线程设置参数
    static void initVideoThread(VideoThread *videoThread, const VideoPara &videoPara, const EncodePara &encodePara);

    //根据地址取出本地设备或者桌面的参数以及其他处理(用于单独的解码线程初始化使用/推流只需要用解码线程不需要视频控件)
    static void initVideoPara(VideoThread *videoThread, const QString &mediaUrl);

    static void initVideoPara(VideoThread *videoThread, const QString &mediaUrl, float &encodeVideoRatio,
                              QString &encodeVideoScale);

    //初始化视频控件参数
    static void
    initVideoWidget(VideoWidget *videoWidget, const QString &mediaUrl, int videoCore, int scaleMode, int videoMode,
                    const QString &hardware, const QString &audioDevice = QString());
};

#endif // VIDEOHELPER_H
