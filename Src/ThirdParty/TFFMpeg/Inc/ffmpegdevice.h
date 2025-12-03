#ifndef FFMPEGDEVICE_H
#define FFMPEGDEVICE_H

#include "ffmpeghelper.h"

class FFmpegDevice {
public:
    //设置当前日志回调的类型和解析结果
    static QString logType;
    static QString logFlag;
    static QStringList logResult;

    //日志回调
    static void logCallback(void *ptr, int level, const char *fmt, va_list vl);

    static void log_list_devices(const QString &line);

    static void log_list_options(const QString &line);

    //获取输入设备名称
    static void getInputDevices(bool video, QStringList &devices);

    //打印设备列表和参数 type: vfwcap dshow v4l2 avfoundation
    static QStringList getInputDevices(const char *flag);

    static QStringList getDeviceOption(const QString &url);

    static QStringList getDeviceOption(const char *flag, const QString &device);

    //获取输入输出设备名称集合
#ifdef ffmpegdevice
    static QStringList getInputDevices(bool video);
    static QStringList getDeviceNames(bool input, bool video);
    static QStringList getDeviceNames(AVDeviceInfoList *devices, bool video);
#endif
};

#endif // FFMPEGDEVICE_H
