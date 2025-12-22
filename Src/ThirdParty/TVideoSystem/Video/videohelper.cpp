#include "videohelper.h"
#include "videoutil.h"
#include "urlhelper.h"
#include "videowidgetx.h"
#include "widgethelper.h"
#include "deviceinfohelper.h"

QString VideoHelper::getVersion() {
    return VideoUtil::getVersion();
}

QString VideoHelper::getVersion(const VideoCore &videoCore) {
    return VideoUtil::getVersion(videoCore);
}

VideoCore VideoHelper::getVideoCore(int type) {
    return VideoUtil::getVideoCore(type);
}

bool VideoHelper::checkSaveFile(const QString &fullName, const QString &fileName, bool isffmpeg) {
    return VideoUtil::checkSaveFile(fullName, fileName, isffmpeg);
}

void VideoHelper::renameFile(const QString &fileName) {
    VideoUtil::renameFile(fileName);
}

void VideoHelper::resetCursor() {
    VideoUtil::resetCursor();
}

void VideoHelper::rotateImage(int rotate, QImage &image) {
    if (rotate > 0) {
        QTransform matrix;
        matrix.rotate(rotate);
        image = image.transformed(matrix, Qt::SmoothTransformation);
    }
}

void VideoHelper::rotateSize(int rotate, int &width, int &height) {
    WidgetHelper::rotateSize(rotate, width, height);
}

int VideoHelper::getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax) {
    return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
}

bool VideoHelper::checkUrl(VideoThread *videoThread, const MediaType &mediaType, const QString &mediaUrl, int timeout) {
    //因为每个视频采集子类都会执行这个方法/所以通用的处理放在这里

    //部分协议必须强制指定tcp还是udp通信
    if (mediaUrl.startsWith("udp://") || mediaUrl.startsWith("srt://")) {
        videoThread->setTransport("udp");
    } else if (mediaUrl.startsWith("tcp://")) {
        videoThread->setTransport("tcp");
    }

    //本地设备和桌面采集不支持硬解码
    if (mediaType == MediaType_Device || mediaType == MediaType_Screen) {
        if (videoThread->getVideoCodecName() != "h264") {
            videoThread->setHardware("none");
        }
    }

    //如果指定窗口采集则不能设置分辨率
    if (mediaUrl.contains("title=")) {
        videoThread->setBufferSize("0x0");
    }

    QString error;
    if (mediaUrl.isEmpty()) {
        error = "地址为空";
    } else if (mediaType == MediaType_Rtsp) {
        /*
        if (!UrlHelper::checkUrl(mediaUrl, timeout)) {
            error = "网络地址不可达";
        }
        */
    } else if (mediaType == MediaType_FileLocal) {
        if (!QFile(mediaUrl).exists()) {
            error = "文件不存在";
        }
    } else if (mediaType == MediaType_Other) {

    }

    if (error.isEmpty()) {
        return true;
    } else {
        videoThread->debug("地址有误", "原因: " + error);
        return false;
    }
}

QString VideoHelper::getFormatName(VideoThread *videoThread) {
    //文件封装格式可能是一个很长的字符串比如 mov,mp4,m4a,3gp,3g2,mj2
    QString name = videoThread->getFormatName();
    if (name.contains(",")) {
        QString url = videoThread->getMediaUrl();
        MediaType type = videoThread->getMediaType();
        if (type == MediaType_FileLocal) {
            name = url.split(".").last();
        } else if (type == MediaType_FileWeb) {
            name = url.split("://").first();
        } else if (name.contains("v4l2")) {
            name = "v4l2";
        } else {
            name = "file";
        }
    } else if (name.contains("-")) {
        name = name.split("-").first();
        name = name.trimmed();
    }

    return name;
}

QString VideoHelper::getPlayUrl(const VideoCore &videoCore, const MediaType &mediaType, const QString &mediaUrl) {
    //本地设备有时候设备名居然带空格
    QString url = (mediaType == MediaType_Device ? mediaUrl : mediaUrl.trimmed());
    if (mediaType == MediaType_Rtsp) {
        //视频流中的特殊字符比如rtsp中的用户密码部分需要转义
        int index = mediaUrl.lastIndexOf("@");
        if (index > 0) {
            QString userName, userPwd;
            UrlHelper::getUserInfo(url, userName, userPwd);
            QString otherInfo = mediaUrl.mid(index + 1, mediaUrl.length() - index);
            userName = userName.toUtf8().toPercentEncoding();
            userPwd = userPwd.toUtf8().toPercentEncoding();

            //不能用字符串拼接(又会把转义字符转回去)
            //url = QString("rtsp://%1:%2@%3").arg(userName).arg(userPwd).arg(otherInfo);
            url.clear();
            url.append("rtsp://");
            url.append(userName);
            url.append(":");
            url.append(userPwd);
            url.append("@");
            url.append(otherInfo);
        }
    } else if (mediaType == MediaType_Http) {
        //视频流中带空格要转换
        if (url.contains(" ")) {
            url = QUrl(url).toEncoded();
        }
    } else if (mediaType == MediaType_Device) {
        bool isAudio = url.startsWith("audio=");
        QString flag = (isAudio ? Device_Audio : Device_Video);
        //linux上直接采集设备文件
#ifndef Q_OS_WIN
        url.replace("video=", "");
        url.replace("audio=", "");
#endif
        if (videoCore == VideoCore_Vlc) {
            url = QString("%1://").arg(Device_Video);
        } else if (videoCore == VideoCore_Mpv) {
            url = QString("av://%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_Mdk) {
            url = QString("avdevice://%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_Qtav) {
            url = QString("avdevice:%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_EasyPlayer) {
            url = QString("%1://%2").arg(flag).arg(url);
        }
    } else if (mediaType == MediaType_Screen) {
        QString flag = Device_Screen;
        //要去掉前缀
        url.replace("screen=", "");
        if (videoCore == VideoCore_Vlc) {
            url = "screen://";
        } else if (videoCore == VideoCore_Mpv) {
            url = QString("av://%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_Mdk) {
            url = QString("avdevice://%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_Qtav) {
            url = QString("avdevice:%1:%2").arg(flag).arg(url);
        } else if (videoCore == VideoCore_EasyPlayer) {
            url = QString("%1://%2").arg(flag).arg(url);
        }
    }

    //QString msg = QString("地址校验 -> 类型: %1 地址: %2").arg(mediaType).arg(url);
    //qDebug() << TIMEMS << msg;
    return url;
}

QUrl VideoHelper::getPlayUrl(const MediaType &mediaType, const QString &mediaUrl) {
    QUrl url;
    if (mediaType == MediaType_FileLocal) {
        url = QUrl::fromLocalFile(mediaUrl);
    } else {
        url = QUrl(VideoHelper::getPlayUrl(VideoCore_QMedia, mediaType, mediaUrl));
    }
    return url;
}

QByteArray VideoHelper::getUrlData(const MediaType &mediaType, const QString &mediaUrl, bool gbk) {
    if (gbk && mediaType == MediaType_Screen) {
        //return mediaUrl.toLocal8Bit();
        return QTextCodec::codecForName("gbk")->fromUnicode(mediaUrl);
    } else {
        return mediaUrl.toUtf8();
    }
}

MediaType VideoHelper::getMediaType(const QString &mediaUrl) {
    //http开头的文件地址这里没法判断(需要打开后获取时长判断)
    MediaType mediaType;
    if (mediaUrl.startsWith("rtsp://") || mediaUrl.startsWith("rtsps://")) {
        mediaType = MediaType_Rtsp;
    } else if (mediaUrl.startsWith("rtmp://") || mediaUrl.startsWith("rtmps://")) {
        mediaType = MediaType_Rtmp;
    } else if (mediaUrl.startsWith("http://") || mediaUrl.startsWith("https://")) {
        mediaType = MediaType_Http;
    } else if (mediaUrl.startsWith("udp://")) {
        mediaType = MediaType_Udp;
    } else if (mediaUrl.startsWith("tcp://")) {
        mediaType = MediaType_Tcp;
    } else if (mediaUrl.startsWith("srt://")) {
        mediaType = MediaType_Srt;
    } else if (mediaUrl.startsWith("ws://") || mediaUrl.startsWith("wss://")) {
        mediaType = MediaType_WebSocket;
    } else if (mediaUrl.startsWith("webrtc://")) {
        mediaType = MediaType_Webrtc;
    } else if (QFile(mediaUrl).exists() && !mediaUrl.startsWith("/dev/")) {
        //在linux系统万物皆文件所以设备文件 /dev/video0 这种会认为文件存在
        mediaType = MediaType_FileLocal;
    } else if (mediaUrl.startsWith("video=") || mediaUrl.startsWith("audio=")) {
        mediaType = MediaType_Device;
    } else if (mediaUrl.startsWith("screen=")) {
        mediaType = MediaType_Screen;
    } else {
        mediaType = MediaType_Other;
    }

    return mediaType;
}

void VideoHelper::getDeviceName(const QString &mediaUrl, QString &audioDevice, QString &videoDevice) {
    videoDevice = "none";
    audioDevice = "none";
    QStringList list = mediaUrl.split(":");
            foreach (QString s, list) {
            QStringList l = s.split("=");
            QString flag = l.first();
            QString name = l.last();
            if (flag == "audio") {
                audioDevice = name;
            } else if (flag == "video") {
                videoDevice = name;
            }
        }
}

//格式要求: url|bufferSize|frameRate|offsetX|offsetY|screenIndex|encodeVideoRatio|encodeVideoScale
void
VideoHelper::getDevicePara(QString &mediaUrl, QString &bufferSize, int &frameRate, QString &codecName, int &offsetX,
                           int &offsetY, float &encodeVideoRatio, QString &encodeVideoScale) {
    //ffmpeg -f gdigrab -s 800x600 -r 30 -i desktop out.mp4
    //ffmpeg -f x11grab -s 800x600 -r 30 -i :0.0+0,0 out.mp4
    //ffmpeg -f avfoundation -s 800x600 -r 30 -i 0:0 out.mp4

    //本地设备 video=USB2.0 PC CAMERA|1920x1080|30|mjpeg
    //桌面采集 screen=desktop|800x600|25
    //窗口采集 screen=title=MainWindow

    DeviceInfoHelper::getDevicePara(mediaUrl, bufferSize, frameRate, codecName, offsetX, offsetY, encodeVideoRatio,
                                    encodeVideoScale);
    mediaUrl = mediaUrl.split("|").first();
}

//格式要求: url|transport|decodeType|encodeVideo|encodeVideoFps|encodeVideoRatio|encodeVideoScale
void VideoHelper::getNormalPara(QString &mediaUrl, QString &transport, DecodeType &decodeType, EncodeVideo &encodeVideo,
                                int &encodeVideoFps, float &encodeVideoRatio, QString &encodeVideoScale) {
    int decodeType2 = -1, encodeVideo2 = -1;
    DeviceInfoHelper::getNormalPara(mediaUrl, transport, decodeType2, encodeVideo2, encodeVideoFps, encodeVideoRatio,
                                    encodeVideoScale);
    mediaUrl = mediaUrl.split("|").first();

    if (decodeType2 >= 0) {
        decodeType = (DecodeType) decodeType2;
    }
    if (encodeVideo2 >= 0) {
        encodeVideo = (EncodeVideo) encodeVideo2;
    }
}

MediaType VideoHelper::initPara(WidgetPara &widgetPara, VideoPara &videoPara, EncodePara &encodePara) {
    //获取视频类型
    QString url = videoPara.mediaUrl;
    MediaType mediaType = VideoHelper::getMediaType(url);

    //媒体地址不能为空
    if (url.isEmpty()) {
        return mediaType;
    }

    //标签和图形绘制模式默认值
    widgetPara.osdDrawMode = DrawMode_Cover;
    widgetPara.graphDrawMode = DrawMode_Cover;

    //本地mp3文件需要绘制封面
    if (mediaType == MediaType_FileLocal && url.endsWith(".mp3")) {
        widgetPara.videoMode = VideoMode_Painter;
    }

    //下面的纠正仅仅是目前已经实现的(如果后面有增加对应处理需要调整)
    VideoCore videoCore = videoPara.videoCore;
    if (videoCore == VideoCore_QMedia) {
        //qmedia内核下的GPU模式对应要用rgbwidget窗体
        if (widgetPara.videoMode == VideoMode_Opengl) {
            videoPara.hardware = "rgb";
            widgetPara.osdDrawMode = DrawMode_Source;
            widgetPara.graphDrawMode = DrawMode_Source;
        }

        //Qt4中的多媒体只有句柄模式(Qt5.6以下绘制模式用QAbstractVideoSurface获取不到图片也只能用句柄模式)
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
        widgetPara.videoMode = VideoMode_Hwnd;
#endif
    } else if (videoCore == VideoCore_FFmpeg) {
        //ffmpeg内核没有句柄模式
        if (widgetPara.videoMode == VideoMode_Hwnd) {
            widgetPara.videoMode = VideoMode_Opengl;
        }

        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_Vlc) {
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;

        //vlc内核下的GPU模式对应要用rgbwidget窗体
        if (widgetPara.videoMode == VideoMode_Opengl) {
            videoPara.hardware = "rgb";
        }

        //非句柄模式下非本地文件必须指定分辨率
        if (videoPara.bufferSize == "0x0" && mediaType != MediaType_FileLocal) {
            videoPara.bufferSize = "640x480";
        }
    } else if (videoCore == VideoCore_Mpv) {
        //mpv内核只有句柄模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_Mdk || videoCore == VideoCore_Qtav) {
        //内核只有GPU模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_HaiKang || videoCore == VideoCore_DaHua) {
        //目前不支持硬件加速
        videoPara.hardware = "none";
    } else if (videoCore == VideoCore_EasyPlayer) {
        //easyplayer内核只有句柄模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    }

    //纠正缓存分辨率
    if (!videoPara.bufferSize.contains("x")) {
        videoPara.bufferSize = "0x0";
    }

    //纠正缓存大小和其他
    if (videoCore == VideoCore_FFmpeg || videoCore == VideoCore_Mdk || videoCore == VideoCore_Qtav) {
        if ((videoPara.caching < 1024000) || (videoPara.caching > 102400000)) {
            videoPara.caching = 8192000;
        }
    } else if (videoCore == VideoCore_Vlc) {
        if ((videoPara.caching < 200) || (videoPara.caching > 8000)) {
            videoPara.caching = 500;
        }
    }

    //如果地址带了本地设备参数或者桌面参数则需要取出对应的参数
    if (mediaType == MediaType_Device || mediaType == MediaType_Screen) {
        VideoHelper::getDevicePara(videoPara.mediaUrl, videoPara.bufferSize, videoPara.frameRate, videoPara.codecName,
                                   videoPara.offsetX, videoPara.offsetY, encodePara.encodeVideoRatio,
                                   encodePara.encodeVideoScale);
    } else {
        VideoHelper::getNormalPara(videoPara.mediaUrl, videoPara.transport, videoPara.decodeType,
                                   encodePara.encodeVideo, encodePara.encodeVideoFps, encodePara.encodeVideoRatio,
                                   encodePara.encodeVideoScale);
    }

    //本地文件以及本地摄像头不需要连接超时时间
    if (mediaType == MediaType_FileLocal || mediaType == MediaType_Device) {
        videoPara.connectTimeout = 0;
    }

    //打开休息时间不宜过短(建议最低1秒)
    if (videoPara.openSleepTime < 1000) {
        videoPara.openSleepTime = 1000;
    }

    //读取超时时间不宜过短(建议最低5秒)
    if (videoPara.readTimeout > 0 && videoPara.readTimeout < 5000) {
        videoPara.readTimeout = 5000;
    }

    //连接超时时间=0表示对网络流不做提前连接测试处理
    if (videoPara.connectTimeout > 0) {
        //连接超时时间过长则自动调整打开休息时间(至少要2秒)
        if (videoPara.openSleepTime > 0 && (videoPara.openSleepTime - videoPara.connectTimeout) < 2000) {
            videoPara.openSleepTime = videoPara.connectTimeout + 2000;
        }

        //连接超时时间过长则自动调整读取超时时间(至少要3轮)
        if (videoPara.readTimeout > 0 && (videoPara.readTimeout / videoPara.connectTimeout) < 3) {
            videoPara.readTimeout = videoPara.connectTimeout * 3;
        }
    }

#ifndef openglx
    //如果没有opengl则强制改成绘制模式
    if (widgetPara.videoMode == VideoMode_Opengl) {
        widgetPara.videoMode = VideoMode_Painter;
    }
#else
    //OpenGLES模式下不能用硬解码
    if (QCoreApplication::testAttribute(Qt::AA_UseOpenGLES)) {
        videoPara.hardware = "none";
    }
#endif

    //句柄模式不能共享解码线程
    if (widgetPara.videoMode == VideoMode_Hwnd) {
        widgetPara.sharedData = false;
    }

    //绘制模式下标签和图形绘制模式强制改成图片
    if (widgetPara.videoMode == VideoMode_Painter) {
        widgetPara.osdDrawMode = DrawMode_Image;
        widgetPara.graphDrawMode = DrawMode_Image;
    }

    //如果启用了视频编码则绘制采用源头绘制
    if (videoCore == VideoCore_FFmpeg && encodePara.encodeVideo != EncodeVideo_None) {
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    }

    return mediaType;
}

VideoThread *VideoHelper::newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode) {
    return VideoUtil::newVideoThread(parent, videoCore, videoMode);
}

void VideoHelper::initVideoThread(VideoThread *videoThread, const VideoPara &videoPara, const EncodePara &encodePara) {
    //过滤下结尾空格/有些本地设备名居然有空格/不能过滤掉
    QString url = videoPara.mediaUrl;
    if (!url.startsWith("video=") && !url.startsWith("audio=")) {
        url = url.trimmed();
    }

    //设置一堆参数
    videoThread->setVideoCore(videoPara.videoCore);
    videoThread->setMediaUrl(url);
    videoThread->setBufferSize(videoPara.bufferSize);
    videoThread->setFrameRate(videoPara.frameRate);
    videoThread->setVideoCodecName(videoPara.codecName);
    videoThread->setProperty("offsetX", videoPara.offsetX);
    videoThread->setProperty("offsetY", videoPara.offsetY);

    videoThread->setDecodeType(videoPara.decodeType);
    videoThread->setHardware(videoPara.hardware);
    videoThread->setTransport(videoPara.transport.toLower());
    videoThread->setCaching(videoPara.caching);

    videoThread->setAudioLevel(videoPara.audioLevel);
    videoThread->setDecodeAudio(videoPara.decodeAudio);
    videoThread->setPlayAudio(videoPara.playAudio);
    videoThread->setPlayRepeat(videoPara.playRepeat);
    videoThread->setPlayStep(videoPara.playStep);
    videoThread->setRealBitRate(videoPara.realBitRate);

    videoThread->setEncryptKey(videoPara.encryptKey);
    videoThread->setDecryptKey(videoPara.decryptKey);
    videoThread->setAudioDevice(videoPara.audioDevice);

    videoThread->setOpenSleepTime(videoPara.openSleepTime);
    videoThread->setReadTimeout(videoPara.readTimeout);
    videoThread->setConnectTimeout(videoPara.connectTimeout);

    videoThread->setEncodeAudio(encodePara.encodeAudio);
    videoThread->setEncodeVideo(encodePara.encodeVideo);
    videoThread->setEncodeVideoFps(encodePara.encodeVideoFps);
    videoThread->setEncodeVideoRatio(encodePara.encodeVideoRatio);
    videoThread->setEncodeVideoScale(encodePara.encodeVideoScale);
}

void VideoHelper::initVideoPara(VideoThread *videoThread, const QString &mediaUrl) {
    float encodeVideoRatio = 1;
    QString encodeVideoScale = "1";
    VideoHelper::initVideoPara(videoThread, mediaUrl, encodeVideoRatio, encodeVideoScale);
}

void VideoHelper::initVideoPara(VideoThread *videoThread, const QString &mediaUrl, float &encodeVideoRatio,
                                QString &encodeVideoScale) {
    //如果是本地设备或者桌面录屏要取出其他参数
    MediaType mediaType = VideoHelper::getMediaType(mediaUrl);
    if (mediaType == MediaType_Device || mediaType == MediaType_Screen) {
        QString deviceName = mediaUrl;
        QString resolution = "0x0";
        int frameRate = 0;
        QString codecName;
        int offsetX = 0;
        int offsetY = 0;
        VideoHelper::getDevicePara(deviceName, resolution, frameRate, codecName, offsetX, offsetY, encodeVideoRatio,
                                   encodeVideoScale);

        videoThread->setMediaUrl(deviceName);
        videoThread->setBufferSize(resolution);
        videoThread->setFrameRate(frameRate);
        videoThread->setVideoCodecName(codecName);
        videoThread->setProperty("offsetX", offsetX);
        videoThread->setProperty("offsetY", offsetY);
    } else {
        QString url = mediaUrl;
        QString transport = "none";
        DecodeType decodeType = DecodeType_Fast;
        EncodeVideo encodeVideo = EncodeVideo_None;
        int encodeVideoFps = 0;
        float encode_video_ratio = 1;
        VideoHelper::getNormalPara(url, transport, decodeType, encodeVideo, encodeVideoFps, encode_video_ratio,
                                   encodeVideoScale);

        videoThread->setMediaUrl(url);
        videoThread->setTransport(transport);
        videoThread->setDecodeType(decodeType);
        videoThread->setEncodeVideo(encodeVideo);
        videoThread->setEncodeVideoFps(encodeVideoFps);
        videoThread->setEncodeVideoRatio(encode_video_ratio);
        videoThread->setEncodeVideoScale(encodeVideoScale);
    }
}

void VideoHelper::initVideoWidget(VideoWidget *videoWidget, const QString &mediaUrl, int videoCore, int scaleMode,
                                  int videoMode, const QString &hardware, const QString &audioDevice) {
    //设置悬浮条信息
    BannerInfo bannerInfo = videoWidget->getBannerInfo();
    bannerInfo.enableAll();

    //设置窗体参数
    WidgetPara widgetPara = videoWidget->getWidgetPara();
    widgetPara.scaleMode = (ScaleMode) scaleMode;
    widgetPara.videoMode = (VideoMode) videoMode;
    widgetPara.bannerEnable = true;

    //设置视频参数
    VideoPara videoPara = videoWidget->getVideoPara();
    videoPara.videoCore = (VideoCore) videoCore;
    videoPara.mediaUrl = mediaUrl;
    videoPara.hardware = hardware;
    videoPara.audioDevice = audioDevice;
    videoPara.playRepeat = true;

    //要先初始化参数再设置
    EncodePara encodePara = videoWidget->getEncodePara();
    VideoHelper::initPara(widgetPara, videoPara, encodePara);
    videoWidget->setWidgetPara(widgetPara);
    videoWidget->setVideoPara(videoPara);
    videoWidget->setBannerInfo(bannerInfo);
}
