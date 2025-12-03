#include "ffmpegdevice.h"
#include "widgethead.h"

QString FFmpegDevice::logType = QString();
QString FFmpegDevice::logFlag = QString();
QStringList FFmpegDevice::logResult = QStringList();

void FFmpegDevice::logCallback(void *ptr, int level, const char *fmt, va_list vl) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    char buf[1024];
    vsprintf(buf, fmt, vl);
    QString line = buf;
    line = line.trimmed();
    line.replace("\r", "");
    line.replace("\n", "");
    line.replace("\"", "");
    //qDebug() << TIMEMS << line;

    //根据不同的类型解析不同的结果
    if (FFmpegDevice::logType == "list_devices") {
        FFmpegDevice::log_list_devices(line);
    } else if (FFmpegDevice::logType == "list_options") {
        FFmpegDevice::log_list_options(line);
    }
}

void FFmpegDevice::log_list_devices(const QString &line) {
#if (FFMPEG_VERSION_MAJOR < 5)
    //搞个标记记录接下来是什么结果
    if (line.startsWith("DirectShow video devices")) {
        FFmpegDevice::logFlag = "video";
    } else if (line.startsWith("DirectShow audio devices")) {
        FFmpegDevice::logFlag = "audio";
    }

    //过滤一些不需要的结果
    if (line.startsWith("DirectShow") || line.startsWith("Alternative")) {
        return;
    }

    QString result = QString("%1|%2").arg(FFmpegDevice::logFlag).arg(line);
    if (!FFmpegDevice::logResult.contains(result)) {
        FFmpegDevice::logResult << result;
    }
#else
    if (line.startsWith("(video") || line.startsWith("(audio")) {
        QString type = line.mid(1, line.length());
        QString result = QString("%1|%2").arg(type).arg(FFmpegDevice::logFlag);
        if (!FFmpegDevice::logResult.contains(result)) {
            FFmpegDevice::logResult << result;
        }
    } else {
        FFmpegDevice::logFlag = line;
    }
#endif
}

void FFmpegDevice::log_list_options(const QString &line) {
    //搞个标记记录接下来是什么结果
    if (line.startsWith("vcodec=") || line.startsWith("pixel_format=")) {
        FFmpegDevice::logFlag = line;
    }

    if (line.startsWith("min s=") && !FFmpegDevice::logFlag.isEmpty()) {
        QString result = QString("%1 %2").arg(FFmpegDevice::logFlag).arg(line);
        if (!FFmpegDevice::logResult.contains(result)) {
            FFmpegDevice::logResult << result;
        }
    }

    //音频设备参数
    if (line.contains("ch=")) {
        FFmpegDevice::logResult << line;
    }
}

void FFmpegDevice::getInputDevices(bool video, QStringList &devices) {
    QStringList names;

    //先用命令行获取/ffmpeg5以上版本可以直接通过函数获取
#if (FFMPEG_VERSION_MAJOR < 5)
    QStringList infos = FFmpegDevice::getInputDevices(video ? Device_Video : Device_Audio);
    foreach (QString info, infos) {
        QString name = info.split("|").last();
        if (video && info.startsWith("video")) {
            names << name;
        } else if (!video && info.startsWith("audio")) {
            names << name;
        }
    }
#endif

    //没有获取到再用函数获取
    if (names.isEmpty()) {
#ifdef ffmpegdevice
        names << FFmpegDevice::getInputDevices(video);
#endif
    }

    //去重复加入到集合
    //qDebug() << video << names;
            foreach (QString name, names) {
            if (!devices.contains(name)) {
                devices << name;
            }
        }
}

//ffmpeg -f dshow -list_devices true -i dummy
QStringList FFmpegDevice::getInputDevices(const char *flag) {
    FFmpegHelper::initLib();

    //启用日志回调接收输出信息
    av_log_set_callback(FFmpegDevice::logCallback);

    //设置相关参数以便记录对应结果
    FFmpegDevice::logType = "list_devices";
    FFmpegDevice::logFlag.clear();
    FFmpegDevice::logResult.clear();

    AVFormatContext *ctx = avformat_alloc_context();
    AVInputFormatx *fmt = av_find_input_format(flag);

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "list_devices", "true", 0);

    if (strcmp(flag, "vfwcap") == 0) {
        avformat_open_input(&ctx, "list", fmt, NULL);
    } else if (strcmp(flag, "dshow") == 0) {
        avformat_open_input(&ctx, "dummy", fmt, &opts);
    } else {
        avformat_open_input(&ctx, "", fmt, &opts);
    }

    //释放资源
    av_dict_free(&opts);
    avformat_close_input(&ctx);

    //重新设置日志回调以便恢复原样
    av_log_set_callback(av_log_default_callback);

    //取出日志结果
    return FFmpegDevice::logResult;
}

QStringList FFmpegDevice::getDeviceOption(const QString &url) {
    const char *flag = (url.startsWith("video=") ? Device_Video : Device_Audio);
    QString device = url;
#ifndef Q_OS_WIN
    device.replace("video=", "");
    device.replace("audio=", "");
#endif

    return FFmpegDevice::getDeviceOption(flag, device);
}

//ffmpeg -f dshow -list_options true -i video="Webcam"
QStringList FFmpegDevice::getDeviceOption(const char *flag, const QString &device) {
    FFmpegHelper::initLib();

    //启用日志回调接收输出信息
    av_log_set_callback(FFmpegDevice::logCallback);

    //设置相关参数以便记录对应结果
    FFmpegDevice::logType = "list_options";
    FFmpegDevice::logFlag.clear();
    FFmpegDevice::logResult.clear();

    AVFormatContext *ctx = avformat_alloc_context();
    AVInputFormatx *fmt = av_find_input_format(flag);

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "list_options", "true", 0);
    avformat_open_input(&ctx, device.toUtf8().constData(), fmt, &opts);

    //释放资源
    av_dict_free(&opts);
    avformat_close_input(&ctx);

    //重新设置日志回调以便恢复原样
    av_log_set_callback(av_log_default_callback);

    //取出日志结果
    return FFmpegDevice::logResult;
}

#ifdef ffmpegdevice
QStringList FFmpegDevice::getInputDevices(bool video)
{
    FFmpegHelper::initLib();

    //测试发现在windows上要ffmpeg5开始才能获取到值/以前的版本内部没有实现/linux上ffmpeg4也支持
    QStringList names;
    AVDeviceInfoList *devices = NULL;
    AVInputFormatx *fmt = NULL;
    fmt = av_find_input_format(video ? Device_Video : Device_Audio);
    if (fmt) {
        //下面这行代码在部分环境中执行会崩溃/ffmpeg内部错误导致
#ifndef __arm__
        if (avdevice_list_input_sources(fmt, NULL, NULL, &devices) >= 0) {
            names = getDeviceNames(devices, video);
        }
#endif
    }

    return names;
}

QStringList FFmpegDevice::getDeviceNames(bool input, bool video)
{
    FFmpegHelper::initLib();

    QStringList names;
    AVDeviceInfoList *devices = NULL;
    if (input) {
        AVInputFormatx *fmt = NULL;
        do {
            names.clear();
            fmt = (video ? av_input_video_device_next(fmt) : av_input_audio_device_next(fmt));
            if (fmt) {
                if (avdevice_list_input_sources(fmt, NULL, NULL, &devices) >= 0) {
                    names = getDeviceNames(devices, video);
                }
                //qDebug() << "input" << fmt->name << names;
            }
        } while (fmt);
    } else {
        AVOutputFormatx *fmt = NULL;
        do {
            names.clear();
            fmt = (video ? av_output_video_device_next(fmt) : av_output_audio_device_next(fmt));
            if (fmt) {
                if (avdevice_list_output_sinks(fmt, NULL, NULL, &devices) >= 0) {
                    names = getDeviceNames(devices, video);
                }
                //qDebug() << "output" << fmt->name << names;
            }
        } while (fmt);
    }

    return names;
}

QStringList FFmpegDevice::getDeviceNames(AVDeviceInfoList *devices, bool video)
{
    QStringList names;
    int count = devices->nb_devices;
    for (int i = 0; i < count; ++i) {
        AVDeviceInfo *device = devices->devices[i];
#if (FFMPEG_VERSION_MAJOR > 4)
        if (device->nb_media_types > 0) {
            AVMediaType type = *device->media_types;
            if ((video && type != AVMEDIA_TYPE_VIDEO) || (!video && type != AVMEDIA_TYPE_AUDIO)) {
                continue;
            }
        }
#endif
        //在win上设备名传描述符/linux是设备名
#ifdef Q_OS_WIN
        names << device->device_description;
#else
        names << device->device_name;
#endif
    }

    //释放设备列表
    avdevice_free_list_devices(&devices);
    return names;
}
#endif
