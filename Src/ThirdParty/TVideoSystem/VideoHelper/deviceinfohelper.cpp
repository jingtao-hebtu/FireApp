#include "deviceinfohelper.h"
#include "qapplication.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#include "qscreen.h"

#endif

#ifdef multimedia
#include <QtMultimedia>
#endif
#ifdef ffmpegdevice
#include "ffmpegdevice.h"
#endif

QList<QRect> DeviceInfoHelper::getScreenRects() {
    QList<QRect> rects;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    int screenCount = qApp->screens().count();
    QList<QScreen *> screens = qApp->screens();
    for (int i = 0; i < screenCount; ++i) {
        QScreen *screen = screens.at(i);
        //需要根据缩放比来重新调整宽高
        qreal ratio = screen->devicePixelRatio();
        QRect rect = screen->geometry();
        rect.setWidth(rect.width() * ratio);
        rect.setHeight(rect.height() * ratio);
        rects << rect;
    }
#else
    int screenCount = qApp->desktop()->screenCount();
    QDesktopWidget *desk = qApp->desktop();
    for (int i = 0; i < screenCount; ++i) {
        QRect rect = desk->screenGeometry(i);
        rects << rect;
    }
#endif
    return rects;
}

QRect DeviceInfoHelper::getScreenRect(int screenIndex) {
    //指定了屏幕索引则取指定的(没有指定则取当前鼠标所在屏幕)
    QRect rect;
    QList<QRect> rects = DeviceInfoHelper::getScreenRects();
    if (screenIndex >= 0 && screenIndex < rects.count()) {
        rect = rects.at(screenIndex);
    } else {
        //当前屏幕区域包含当前鼠标所在坐标则说明是当前屏幕
        QPoint pos = QCursor::pos();
                foreach (QRect r, rects) {
                if (r.contains(pos)) {
                    rect = r;
                    break;
                }
            }
    }
    return rect;
}

QStringList DeviceInfoHelper::getScreenInfo() {
    QStringList infos;
    QList<QRect> rects = DeviceInfoHelper::getScreenRects();
    for (int i = 0; i < rects.count(); ++i) {
        QRect rect = rects.at(i);
        infos << QString("DISPLAY%1 (%2 x %3)").arg(i).arg(rect.width()).arg(rect.height());
    }
    return infos;
}

QString DeviceInfoHelper::getScreenUrl(const QString &url) {
    QString device = url;
    int index = device.mid(7, 1).toInt();
    device.replace("(", "");
    device.replace(")", "");
    device.replace("x", "");
    QStringList list = device.split(" ");
    if (list.count() == 4) {
#if defined(Q_OS_WIN)
        QString flag = "desktop";
#elif defined(Q_OS_LINUX)
        QString flag = ":0.0";
#elif defined(Q_OS_MAC)
        QString flag = "0";
#endif
        device = QString("screen=%1|%2x%3|25|||%4").arg(flag).arg(list.at(1)).arg(list.at(3)).arg(index);
    }
    return device;
}

void DeviceInfoHelper::checkSize(int &width, int &height) {
    //ffmpeg默认只支持偶数/虚拟机中很可能是奇数的分辨率
    if (width % 2 != 0) {
        width--;
    }

    if (height % 2 != 0) {
        height--;
    }
}

QString DeviceInfoHelper::getResolution(int width, int height) {
    DeviceInfoHelper::checkSize(width, height);
    return QString("%1x%2").arg(width).arg(height);
}

QString DeviceInfoHelper::getResolution(const QString &resolution) {
    QStringList sizes = DeviceInfoHelper::getSizes(resolution);
    return getResolution(sizes.at(0).toInt(), sizes.at(1).toInt());
}

QStringList DeviceInfoHelper::getSizes(const QString &size) {
    QStringList sizes;
    if (size.contains("*")) {
        sizes = size.split("*");
    } else if (size.contains("x")) {
        sizes = size.split("x");
    }
    return sizes;
}

void DeviceInfoHelper::checkRect(int screenIndex, QString &resolution, int &offsetX, int &offsetY) {
    QRect rect = DeviceInfoHelper::getScreenRect(screenIndex);
    if (resolution == "0x0") {
        resolution = DeviceInfoHelper::getResolution(rect.width(), rect.height());
    } else {
        resolution = DeviceInfoHelper::getResolution(resolution);
    }

    //偏移值必须小于分辨率否则重置
    if (offsetX > rect.width()) {
        offsetX = 0;
    }
    if (offsetY > rect.height()) {
        offsetY = 0;
    }

    //判断设定的偏移值加上设定的分辨率是否超出了真实的分辨率
    QStringList sizes = DeviceInfoHelper::getSizes(resolution);
    int width = sizes.at(0).toInt();
    int height = sizes.at(1).toInt();

    if (offsetX + width > rect.width()) {
        width = rect.width() - offsetX;
    }
    if (offsetY + height > rect.height()) {
        height = rect.height() - offsetY;
    }

    //如果超出了分辨率则重新设置采集的分辨率
    resolution = DeviceInfoHelper::getResolution(width, height);

    //多个屏幕需要加上屏幕起始坐标
    if (offsetX == 0) {
        offsetX = rect.x();
    } else {
        offsetX += rect.x();
    }
    if (offsetY == 0) {
        offsetY = rect.y();
    } else {
        offsetY += rect.y();
    }

    //qDebug() << TIMEMS << screenIndex << offsetX << offsetY << resolution;
}

void
DeviceInfoHelper::checkOption(const QString &option, QString &format, QString &resolution, int &minFps, int &maxFps) {
    //vcodec=mjpeg min s=1920x1080 fps=25 max s=1920x1080 fps=30
    QStringList list = option.split(" ");
    if (list.count() == 7) {
        format = list.at(0).split("=").last();
        resolution = list.at(5).split("=").last();
        //这里为什么转float/因为有些时候是60.001这种帧率/转int会失败
        minFps = list.at(3).split("=").last().toFloat();
        maxFps = list.at(6).split("=").last().toFloat();
    }
}

QStringList DeviceInfoHelper::getOptions(const QString &audioDevice, const QString &videoDevice) {
    if (videoDevice == "none" && audioDevice == "none") {
        return QStringList();
    }

    //优先获取视频设备的参数
    QString url;
    if (videoDevice != "none") {
        url = "video=" + videoDevice;
    } else {
        url = "audio=" + audioDevice;
    }

    DeviceInfoHelper::deviceOption.clear();
    DeviceInfoHelper::getDeviceOption(url);
    return DeviceInfoHelper::deviceOption;
}

QString DeviceInfoHelper::getOptionUrl(const QString &audioDevice, const QString &videoDevice, const QString &option) {
    QString url = DeviceInfoHelper::getDeviceUrl(audioDevice, videoDevice);
    if (!url.isEmpty() && !option.isEmpty() && !url.startsWith("audio=")) {
        QString format, resolution;
        int minFps, maxFps;
        DeviceInfoHelper::checkOption(option, format, resolution, minFps, maxFps);
        url = QString("%1|%2|%3|%4").arg(url).arg(resolution).arg(maxFps).arg(format);
    }

    return url;
}

QStringList DeviceInfoHelper::deviceOption = QStringList();

void DeviceInfoHelper::getDeviceOption(const QString &url) {
    //为空则重新获取
    if (DeviceInfoHelper::deviceOption.isEmpty()) {
#ifdef ffmpegdevice
        DeviceInfoHelper::deviceOption = FFmpegDevice::getDeviceOption(url);
#endif
    }
    //linux上还需要用命令行获取对应的参数
    if (DeviceInfoHelper::deviceOption.isEmpty()) {
        DeviceInfoHelper::deviceOption = DeviceInfoHelper::getDeviceOption2(url);
    }
}

//安装工具命令 sudo apt-get install v4l-utils
//获取所有设备 v4l2-ctl --list-devices
//获取所有格式 v4l2-ctl --list-formats -d /dev/video0
//获取所有参数 v4l2-ctl --list-formats-ext -d /dev/video0
QStringList DeviceInfoHelper::getDeviceOption2(const QString &url) {
    QStringList options;
#ifdef Q_OS_LINUX
#ifndef Q_OS_WASM
    QProcess process;
    QString devName = url;
    devName.replace("video=", "");
    process.start("v4l2-ctl", QStringList() << "--list-formats-ext" << "-d" << devName);
    process.waitForFinished();
    QString result = process.readAll();
    QStringList list = result.split("\t");

    static QString pixel_format;
    static QString video_size;
    static int min_fps = 200;
    static int max_fps = 0;
    foreach (QString line, list) {
        line = line.trimmed();
        line.replace("\r", "");
        line.replace("\n", "");
        if (line.startsWith("Pixel Format")) {
            QString format = "yuyv422";
            if (line.contains("MJPG")) {
                format = "mjpeg";
            } else if (line.contains("H264")) {
                format = "h264";
            }

            //不同的格式说明是新的/要
            if (format != pixel_format) {
                //存在格式说明当前是新的一个格式/需要把上一个的末尾的信息添加
                if (!pixel_format.isEmpty()) {
                    options << QString("pixel_format=%1 min s=%2 fps=%3 max s=%2 fps=%4").arg(pixel_format).arg(video_size).arg(min_fps).arg(max_fps);
                }

                //复位分辨率和帧率
                pixel_format = format;
                video_size.clear();
                min_fps = 200;
                max_fps = 0;
            }
        } else if (line.startsWith("Size")) {
            //新的分辨率则把上一个分辨率带帧率的信息添加
            QString size = line.split(" ").last();
            if (size != video_size) {
                if (!video_size.isEmpty()) {
                    options << QString("pixel_format=%1 min s=%2 fps=%3 max s=%2 fps=%4").arg(pixel_format).arg(video_size).arg(min_fps).arg(max_fps);
                }

                //复位帧率
                video_size = size;
                min_fps = 200;
                max_fps = 0;
            }
        } else if (line.startsWith("Interval")) {
            //取出当前分辨率分组中最大最小帧率
            line = line.split("(").last();
            line = line.split(".").first();
            int fps = line.toInt();
            if (fps > max_fps) {
                max_fps = fps;
            }
            if (fps < min_fps) {
                min_fps = fps;
            }
        }
    }

    //添加末尾结束的一个
    if (min_fps != 200 && max_fps != 0) {
        options << QString("pixel_format=%1 min s=%2 fps=%3 max s=%2 fps=%4").arg(pixel_format).arg(video_size).arg(min_fps).arg(max_fps);
    }

    //搞完后需要复位
    pixel_format.clear();
    video_size.clear();
    min_fps = 200;
    max_fps = 0;
#endif
#endif
    return options;
}

QStringList DeviceInfoHelper::getDeviceFormat(const QString &url) {
    QStringList formats;
    DeviceInfoHelper::getDeviceOption(url);
            foreach (QString option, DeviceInfoHelper::deviceOption) {
            QStringList list = option.split(" ");
            QString format = list.at(0).split("=").at(1);
            if (!formats.contains(format)) {
                formats << format;
            }
        }

    return formats;
}

QStringList DeviceInfoHelper::getDeviceResolution(const QString &url, const QString &format) {
    QStringList resolutions;
    DeviceInfoHelper::getDeviceOption(url);
            foreach (QString option, DeviceInfoHelper::deviceOption) {
            QStringList list = option.split(" ");
            QString formatx = list.at(0).split("=").at(1);
            if (format == formatx) {
                QString resolution = list.at(2).split("=").at(1);
                resolutions << resolution;
            }
        }

    return resolutions;
}

QStringList DeviceInfoHelper::getDeviceFrameRate(const QString &url, const QString &format, const QString &resolution) {
    QStringList frameRates;
    DeviceInfoHelper::getDeviceOption(url);
            foreach (QString option, DeviceInfoHelper::deviceOption) {
            QStringList list = option.split(" ");
            QString formatx = list.at(0).split("=").at(1);
            if (format == formatx) {
                QString resolutionx = list.at(2).split("=").at(1);
                if (resolution == resolutionx) {
                    QString minFps = list.at(3).split("=").at(1);
                    QString maxFps = list.at(6).split("=").at(1);
                    frameRates << minFps << maxFps;
                    break;
                }
            }
        }

    return frameRates;
}

QStringList DeviceInfoHelper::getAudioDevices() {
    QStringList names;
#ifdef multimedia
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QList<QAudioDevice> devices = QMediaDevices::audioInputs();
    foreach (QAudioDevice device, devices) {
        names << device.description();
    }
#else
    //目前发现Qt4.8会不全/尾部字符串被暴力裁剪/Qt4.7中文乱码/一般建议Qt5使用
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (QAudioDeviceInfo device, devices) {
        //部分版本会重复比如Qt5.9会输出两次/需要去重
        QString name = device.deviceName();
        if (!names.contains(name)) {
            names << name;
        }
    }
#endif
#endif
    return names;
}

QStringList DeviceInfoHelper::getVideoDevices() {
    QStringList names;
#ifdef multimedia
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QList<QCameraDevice>cameras = QMediaDevices::videoInputs();
    foreach (QCameraDevice camera, cameras) {
        names << camera.description();
    }
#elif (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (QCameraInfo camera, cameras) {
        names << camera.description();
    }
#endif
#endif
    return names;
}

QStringList DeviceInfoHelper::getVideoDevices2() {
    QStringList names;
#ifdef Q_OS_LINUX
#ifndef Q_OS_WASM
    QProcess process;
    process.start("ls", QStringList() << "/dev/");
    process.waitForFinished();
    QString result = process.readAll();
    QStringList list = result.split("\n");
    foreach (QString line, list) {
        if (line.startsWith("video")) {
            names << QString("/dev/%1").arg(line);
        }
    }
#endif
#endif
    return names;
}

void DeviceInfoHelper::getInputDevices(QStringList &audioDevices, QStringList &videoDevices, bool screen) {
    //优先用Qt自带的方式获取
    audioDevices << DeviceInfoHelper::getAudioDevices();
    videoDevices << DeviceInfoHelper::getVideoDevices();

    //这里假设如果存在虚拟屏幕则假设虚拟声卡
    if (!audioDevices.contains("virtual-audio-capturer") && videoDevices.contains("screen-capture-recorder")) {
        audioDevices << "virtual-audio-capturer";
    }

    //上面没有获取到则用ffmpeg内置函数再获取
#ifdef ffmpegdevice
    FFmpegDevice::getInputDevices(false, audioDevices);
    FFmpegDevice::getInputDevices(true, videoDevices);
#endif

    //如果为空还需要通过命令行获取
    if (videoDevices.isEmpty()) {
        videoDevices << DeviceInfoHelper::getVideoDevices2();
    }

    //将屏幕信息加到后面
    if (screen) {
        videoDevices << DeviceInfoHelper::getScreenInfo();
    }
}

QString DeviceInfoHelper::getDeviceUrl(const QString &audioDevice, const QString &videoDevice) {
    QString url;
    if (audioDevice.isEmpty() && videoDevice.isEmpty()) {
        return url;
    }

    //优先添加桌面采集
    if (videoDevice.startsWith("DISPLAY")) {
        url = DeviceInfoHelper::getScreenUrl(videoDevice);
        return url;
    }

    QString audioName;
    if (!audioDevice.isEmpty() && audioDevice != "none") {
        audioName = "audio=" + audioDevice;
    }

    QString videoName;
    if (!videoDevice.isEmpty() && videoDevice != "none") {
        videoName = "video=" + videoDevice;
    }

    if (audioName.isEmpty() && videoName.isEmpty()) {
        return url;
    }

    QStringList list;
    if (!videoName.isEmpty()) {
        list << videoName;
    }
    if (!audioName.isEmpty()) {
        list << audioName;
    }

    url = list.join(":");
    return url;
}

void
DeviceInfoHelper::initInputDevices(QComboBox *cboxAudioDevice, QComboBox *cboxVideoDevice, bool none, bool screen) {
    QStringList audioDevices, videoDevices;
    DeviceInfoHelper::getInputDevices(audioDevices, videoDevices, screen);

    cboxAudioDevice->clear();
    cboxVideoDevice->clear();

    if (none) {
        cboxAudioDevice->addItem("none");
        cboxVideoDevice->addItem("none");
    }

    cboxAudioDevice->addItems(audioDevices);
    cboxVideoDevice->addItems(videoDevices);
}

void DeviceInfoHelper::initInputOptions(QComboBox *cboxAudioDevice, QComboBox *cboxVideoDevice,
                                        QComboBox *cboxDeviceOption) {
    QString audioDevice = cboxAudioDevice->currentText();
    QString videoDevice = cboxVideoDevice->currentText();
    QStringList options = DeviceInfoHelper::getOptions(audioDevice, videoDevice);
    cboxDeviceOption->clear();
    cboxDeviceOption->addItems(options);
}

void DeviceInfoHelper::setPara(const QStringList &list, int count, int index, int &para) {
    if (count > index && !list.at(index).isEmpty()) {
        para = list.at(index).toInt();
    }
}

void DeviceInfoHelper::setPara(const QStringList &list, int count, int index, float &para) {
    if (count > index && !list.at(index).isEmpty()) {
        para = list.at(index).toFloat();
    }
}

void DeviceInfoHelper::setPara(const QStringList &list, int count, int index, QString &para) {
    if (count > index && !list.at(index).isEmpty()) {
        para = list.at(index);
    }
}

void DeviceInfoHelper::getDevicePara(const QString &url, QString &resolution, int &frameRate, QString &codecName) {
    int offsetX, offsetY;
    float encodeVideoRatio;
    QString encodeVideoScale;
    DeviceInfoHelper::getDevicePara(url, resolution, frameRate, codecName, offsetX, offsetY, encodeVideoRatio,
                                    encodeVideoScale);
}

void DeviceInfoHelper::getDevicePara(const QString &url, QString &resolution, int &frameRate, QString &codecName,
                                     int &offsetX, int &offsetY, float &encodeVideoRatio, QString &encodeVideoScale) {
    //无论是否带分隔符第一个约定是设备名称
    QStringList list = url.split("|");
    int count = list.count();

    //默认不指定屏幕索引
    int screenIndex = -1;
    //判断是否是屏幕采集
    bool isScreen = url.startsWith("screen=");

    //带分隔符说明还指定了分辨率或帧率
    if (count > 1) {
        QStringList sizes = DeviceInfoHelper::getSizes(list.at(1));
        if (sizes.count() == 2) {
            int width = sizes.at(0).toInt();
            int height = sizes.at(1).toInt();
            resolution = QString("%1x%2").arg(width).arg(height);
        } else {
            resolution = "0x0";
        }

        //第三个参数是帧率
        DeviceInfoHelper::setPara(list, count, 2, frameRate);

        //取出其他几个参数
        if (isScreen) {
            //XY坐标偏移值
            DeviceInfoHelper::setPara(list, count, 3, offsetX);
            DeviceInfoHelper::setPara(list, count, 4, offsetY);
            //屏幕索引
            DeviceInfoHelper::setPara(list, count, 5, screenIndex);
            //压缩比率
            DeviceInfoHelper::setPara(list, count, 6, encodeVideoRatio);
            //视频缩放
            DeviceInfoHelper::setPara(list, count, 7, encodeVideoScale);

            //约定屏幕索引255则取用户自定义的区域
            if (screenIndex != 255) {
                DeviceInfoHelper::checkRect(screenIndex, resolution, offsetX, offsetY);
            }
        } else {
            //指定解码器
            DeviceInfoHelper::setPara(list, count, 3, codecName);
            //压缩比率
            DeviceInfoHelper::setPara(list, count, 4, encodeVideoRatio);
            //视频缩放
            DeviceInfoHelper::setPara(list, count, 5, encodeVideoScale);
        }
    }

    //没有设置分辨率则重新处理
    if (resolution == "0x0") {
        if (!isScreen) {
            resolution = "640x480";
        } else {
            DeviceInfoHelper::checkRect(screenIndex, resolution, offsetX, offsetY);
        }
    }

    //采用第三方录屏只能主屏幕/不能设置分辨率/而且不能指定区域/优点就是效率高而且光标不闪烁
    if (url.contains("video=screen-capture-recorder")) {
        resolution = "0x0";
        codecName = "";
        if (frameRate <= 0 || frameRate >= 30) {
            frameRate = 30;
        }
    }
}

void DeviceInfoHelper::getNormalPara(const QString &url, QString &transport, int &decodeType, int &encodeVideo,
                                     int &encodeVideoFps, float &encodeVideoRatio, QString &encodeVideoScale) {
    QStringList list = url.split("|");
    int count = list.count();

    //通信协议
    DeviceInfoHelper::setPara(list, count, 1, transport);
    //解码策略
    DeviceInfoHelper::setPara(list, count, 2, decodeType);
    //编码视频格式
    DeviceInfoHelper::setPara(list, count, 3, encodeVideo);
    //编码视频帧率
    DeviceInfoHelper::setPara(list, count, 4, encodeVideoFps);
    //视频压缩比率
    DeviceInfoHelper::setPara(list, count, 5, encodeVideoRatio);
    //视频缩放比例
    DeviceInfoHelper::setPara(list, count, 6, encodeVideoScale);
}
