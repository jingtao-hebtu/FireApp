#ifndef DEVICEUTIL_H
#define DEVICEUTIL_H

#include <QString>

class VideoWidget;

class DeviceUtil
{
public:
    //获取设备信息
    static void getDeviceInfo(QStringList &names, QStringList &addrs, QStringList &points);

    //检查地址在线
    static bool checkOnline(const QString &url);
    //弹出预览视频
    static void showVideo(const QString &url, const QString &flag = QString(), int recordTime = 0, const QString &fileName = QString());

    //报警视频回放
    static void showAlarmVideo(const QString &url);
    //报警图片预览
    static void showAlarmImage(const QString &url);

    //通用初始化视频控件
    static void initVideoWidget(VideoWidget *videoWidget, int decodeType = 0, int soundValue = 100, bool soundMuted = false, bool bannerEnable = true, int readTimeout = 0);
    static void initVideoWidget2(VideoWidget *videoWidget);

    //播放声音
    static void playAlarm(const QString &soundName, bool stop = false);
    static void playSound(const QString &soundName, int playCount = 1, bool stop = true);
    static void stopSound();

    //网页预览
    static void preview();
    static void preview(int serverType, const QString &hostName, int hostPort, const QStringList &mediaUrls);
};

#endif // DEVICEUTIL_H
