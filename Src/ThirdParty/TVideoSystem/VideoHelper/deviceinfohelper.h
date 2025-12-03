#ifndef DEVICEINFOHELPER_H
#define DEVICEINFOHELPER_H

#include <QObject>
#include <QRect>
#include <QComboBox>

class DeviceInfoHelper {
public:
    //获取当前屏幕区域
    static QList<QRect> getScreenRects();

    static QRect getScreenRect(int screenIndex = -1);

    //获取本地屏幕信息
    static QStringList getScreenInfo();

    static QString getScreenUrl(const QString &url);

    //传入宽高输出分辨率字符串(矫正奇数)
    static void checkSize(int &width, int &height);

    static QString getResolution(int width, int height);

    static QString getResolution(const QString &resolution);

    //分辨率字符串分割 640x480/640*480
    static QStringList getSizes(const QString &size);

    //根据屏幕区域校验参数
    static void checkRect(int screenIndex, QString &resolution, int &offsetX, int &offsetY);

    //从设备参数字符串获取分辨率帧率等
    static void checkOption(const QString &option, QString &format, QString &resolution, int &minFps, int &maxFps);

    //获取音视频设备的参数
    static QStringList getOptions(const QString &audioDevice, const QString &videoDevice);

    //根据音视频设备名称和参数获取新的地址
    static QString getOptionUrl(const QString &audioDevice, const QString &videoDevice, const QString &option);

    //获取本地设备相关参数/防止重复获取这里搞个变量存储/需要重新获取则清空即可
    static QStringList deviceOption;

    static void getDeviceOption(const QString &url);

    static QStringList getDeviceOption2(const QString &url);

    //获取本地设备支持哪些格式
    static QStringList getDeviceFormat(const QString &url);

    //获取本地设备对应格式支持哪些分辨率
    static QStringList getDeviceResolution(const QString &url, const QString &format);

    //获取本地设备对应格式对应分辨率的帧率范围
    static QStringList getDeviceFrameRate(const QString &url, const QString &format, const QString &resolution);

    //获取本地视音频输入设备名称
    static QStringList getAudioDevices();

    static QStringList getVideoDevices();

    static QStringList getVideoDevices2();

    static void getInputDevices(QStringList &audioDevices, QStringList &videoDevices, bool screen = true);

    static QString getDeviceUrl(const QString &audioDevice, const QString &videoDevice);

    //添加音视频设备到下拉框
    static void
    initInputDevices(QComboBox *cboxAudioDevice, QComboBox *cboxVideoDevice, bool none = true, bool screen = true);

    //获取音视频设备的参数
    static void initInputOptions(QComboBox *cboxAudioDevice, QComboBox *cboxVideoDevice, QComboBox *cboxDeviceOption);

    //从字符串数组设置参数
    static void setPara(const QStringList &list, int count, int index, int &para);

    static void setPara(const QStringList &list, int count, int index, float &para);

    static void setPara(const QStringList &list, int count, int index, QString &para);

    //通用函数取出本地设备带分辨率和帧率等信息
    static void getDevicePara(const QString &url, QString &resolution, int &frameRate, QString &codecName);

    static void getDevicePara(const QString &url, QString &resolution, int &frameRate, QString &codecName, int &offsetX,
                              int &offsetY, float &encodeVideoRatio, QString &encodeVideoScale);

    static void
    getNormalPara(const QString &url, QString &transport, int &decodeType, int &encodeVideo, int &encodeVideoFps,
                  float &encodeVideoRatio, QString &encodeVideoScale);
};

#endif // DEVICEINFOHELPER_H
