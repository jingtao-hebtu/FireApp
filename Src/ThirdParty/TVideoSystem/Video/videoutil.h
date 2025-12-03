#ifndef VIDEOUTIL_H
#define VIDEOUTIL_H

#include "videohead.h"

class VideoWidget;

class VideoUtil {
public:
    //获取当前视频内核版本
    static QString getVersion();

    static QString getVersion(const VideoCore &videoCore);

    //设置解码内核(type可以指定内核/否则按照定义的优先级)
    static VideoCore getVideoCore(int type = 0);

    //获取声卡设备名称
    static QStringList getAudioDevice(const VideoCore &videoCore);

    //创建视频采集类
    static VideoThread *newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode);

    //检查保存的文件是否符合要求
    static bool checkSaveFile(const QString &fullName, const QString &fileName, bool isffmpeg);

    //重命名录制的文件(vlc内核专用)
    static void renameFile(const QString &fileName);

    //鼠标指针复位
    static void resetCursor();

    //加载解析内核到下拉框
    static void loadVideoCore(QComboBox *cbox, int &videoCore, bool simple = false);

    //加载媒体地址到下拉框
    static void loadMediaUrl(QComboBox *cbox, const QString &mediaUrl, int type);

    //去重添加地址到下拉框
    static void loadMediaUrls(QComboBox *cbox, const QString &mediaUrls, const QString &mediaUrl);

    //媒体地址切换改变光标
    static void changeMediaUrl(QComboBox *cbox, QString &mediaUrl);

    //加载保存地址到下拉框
    static void loadSaveUrl(QComboBox *cbox);

    //加载画面策略到下拉框
    static void loadScaleMode(QComboBox *cbox);

    //加载视频模式到下拉框
    static void loadVideoMode(QComboBox *cbox);

    //加载解码策略到下拉框
    static void loadDecodeType(QComboBox *cbox);

    //加载编码策略到下拉框
    static void loadEncodeVideo(QComboBox *cbox);

    //加载视频质量到下拉框
    static void loadEncodeVideoRatio(QComboBox *cbox);

    //加载视频缩放到下拉框
    static void loadEncodeVideoScale(QComboBox *cbox);

    //加载复制通道数量到下拉框
    static void loadCopyNumber(QComboBox *cbox);

    //加载硬件加速名称到下拉框
    static QStringList getHardware(const VideoCore &videoCore);

    static void loadHardware(QComboBox *cbox, int videoCore, QString &hardware);

    static void loadHardware(QComboBox *cbox, const VideoCore &videoCore, QString &hardware);

    //加载通信协议到下拉框
    static void loadTransport(QComboBox *cbox);

    //加载缓存时间到下拉框
    static void loadCaching(QComboBox *cbox, int videoCore, int &caching);

    static void loadCaching(QComboBox *cbox, const VideoCore &videoCore, int &caching);

    //加载打开间隔到下拉框
    static void loadOpenSleepTime(QComboBox *cbox);

    //加载读取超时到下拉框
    static void loadReadTimeout(QComboBox *cbox);

    //加载连接超时到下拉框
    static void loadConnectTimeout(QComboBox *cbox);

    //加载倍速到下拉框
    static void loadSpeed(QComboBox *cbox);

    static void loadSpeed2(QComboBox *cbox);

    //矫正毫秒数到整点
    static qint64 checkDuration(qint64 duration);

    //毫秒数转时间
    static QString getTimeString(qint64 time, bool showMsec = false);

    //复位时长进度
    static void resetDuration(QLabel *labDuration, QLabel *labPosition, QSlider *sliderPosition, bool showMsec = false);

    //设置文件时长
    static void setDuration(QLabel *labDuration, QSlider *sliderPosition, qint64 duration, bool showMsec = false);

    //设置播放进度
    static void setPosition(QLabel *labPosition, QSlider *sliderPosition, qint64 position, bool showMsec = false);

    //写入数据到文件
    static void writeData(QFile *file, char *data, qint64 len);
};

#endif // VIDEOUTIL_H
