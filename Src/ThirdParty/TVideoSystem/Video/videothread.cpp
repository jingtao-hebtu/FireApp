#include "videothread.h"
#include "videohelper.h"
#include "videotask.h"
#include "urlhelper.h"

QList<VideoThread *> VideoThread::videoThreads;

VideoThread *VideoThread::getVideoThread(const WidgetPara &widgetPara, const VideoPara &videoPara) {
    //句柄模式不能共用
    if (!widgetPara.sharedData || widgetPara.videoMode == VideoMode_Hwnd) {
        return NULL;
    }

    VideoThread *thread = NULL;
            foreach (VideoThread *videoThread, videoThreads) {
            //纯音频直接跳过
            if (videoThread->getOnlyAudio()) {
                continue;
            }
            //内核不一样直接跳过
            if (videoThread->getVideoCore() != videoPara.videoCore) {
                continue;
            }

            //桌面采集和非桌面采集比较方式不一样
            if (videoThread->getMediaType() != MediaType_Screen) {
                //地址不一样直接跳过
                if (videoThread->getMediaUrl() != videoPara.mediaUrl) {
                    continue;
                }
            } else {
                //分辨率或者帧率或者偏移值不一样则跳过/要完全一样才是同一个屏幕采集
                if (videoThread->getBufferSize() != videoPara.bufferSize) {
                    continue;
                }
                if (videoThread->getFrameRate() != videoPara.frameRate) {
                    continue;
                }
                if (videoThread->property("offsetX").toInt() != videoPara.offsetX) {
                    continue;
                }
                if (videoThread->property("offsetY").toInt() != videoPara.offsetY) {
                    continue;
                }
            }

            videoThread->refCount++;
            thread = videoThread;
            break;
        }

    return thread;
}

VideoThread::VideoThread(QObject *parent) : AbstractVideoThread(parent) {
    refCount = 0;
    audioIndex = -1;
    videoIndex = -1;
    audioTrack = -1;
    videoTrack = -1;
    duration = 0;
    position = 0;

    firstDuration = -1;
    playCount = 0;
    rotate = -1;

    videoCore = VideoCore_None;
    mediaUrl = "";
    mediaType = MediaType_Other;

    videoWidth = 0;
    videoHeight = 0;
    bufferSize = "0x0";

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

    openSleepTime = 3000;
    readTimeout = 0;
    connectTimeout = 500;

    //这里要过滤下只使用了解码线程而没有对应视频控件的时候
    videoWidget = (QWidget *) parent;
    if (videoWidget) {
        while (!videoWidget->inherits("VideoWidget")) {
            videoWidget = (QWidget *) videoWidget->parent();
            if (!videoWidget) {
                break;
            }
        }
    }

    //也有可能有父控件但是不是视频控件
    if (videoWidget && !videoWidget->inherits("VideoWidget")) {
        videoWidget = 0;
    }

    //启动任务处理线程做一些额外的处理
    VideoTask::Instance()->start();
}

void VideoThread::checkOpen() {
    //特意每次做个小延时每次都去判断标志位等可以大大加快关闭速度
    int count = 0;
    int maxCount = openSleepTime / 100;
    while (!stopped) {
        msleep(100);
        count++;
        //测试下来正常情况下基本上等待一次后 isOk=true
        if (count == maxCount || isOk) {
            break;
        }
    }
}

void VideoThread::run() {
    while (!stopped) {
        if (!isOk) {
            this->closeVideo();
            if (videoMode == VideoMode_Hwnd) {
                QMetaObject::invokeMethod(this, "openVideo");
            } else {
                this->openVideo();
            }

            this->checkOpen();
            continue;
        }

        if (videoCore == VideoCore_Vlc) {
            //存在视频索引但是还没获取到尺寸
            if (videoIndex >= 0 && videoWidth <= 0) {
                this->readMediaInfo();
            }
        } else if (videoCore == VideoCore_Mdk) {
            //实时读取播放进度
            this->getPosition();
        } else if (videoCore == VideoCore_HaiKang) {
            if (mediaType == MediaType_FileLocal) {
                //本地文件需要这里实时读取播放进度
                this->getPosition();
            } else if (isOk && videoMode == VideoMode_Hwnd) {
                //句柄模式下视频流如果打开正常了则sdk内部处理重连
                this->updateTime();
            }
        } else if (videoCore == VideoCore_EasyPlayer) {
            if (audioIndex < 0 && videoIndex < 0) {
                this->readMediaInfo();
            }

            this->getDuration();
            this->getPosition();
        }

        //启用了自动重连则通过判断最后的消息时间(超时则重新打开)
        if (readTimeout > 0) {
            qint64 offset = lastTime.msecsTo(QDateTime::currentDateTime());
            if (offset >= readTimeout) {
                isOk = false;
                debug("超时重连", "");
                continue;
            }
        }

        msleep(100);
    }

    //关闭视频
    this->closeVideo();

    //文件名为空才说明真正处理完可以彻底结束线程(否则一直等因为有可能文件还没保存完成)
    while (!fileName.isEmpty()) {
        debug("等待完成", "");
        msleep(5);
    }

    debug("线程结束", "");
}

bool VideoThread::eventFilter(QObject *watched, QEvent *event) {
    //对应内核中会有 installEventFilter
    //qDebug() << TIMEMS << watched << event;
    if (videoCore == VideoCore_QMedia) {
        //视频源尺寸发生变化需要重新读取信息
        if (event->type() == QEvent::LayoutRequest) {
            this->setGeometry();
        }
    } else if (videoCore == VideoCore_EasyPlayer) {
        //父窗体尺寸变了需要重新设置显示视频控件的尺寸
        if (event->type() == QEvent::Resize) {
            this->setGeometry();
        }
    }

    return QThread::eventFilter(watched, event);
}

bool VideoThread::openVideo() {
    return false;
}

void VideoThread::closeVideo() {
    audioIndex = -1;
    videoIndex = -1;

    duration = 0;
    position = 0;

    firstDuration = -1;
    playCount = 0;

    videoWidth = 0;
    videoHeight = 0;

    errorCount = 0;
    formatName = "";
    videoCodecName = "";
    audioCodecName = "";
    snapName = "";
    fileName = "";

    stopped = false;
    isOk = false;
    isPause = false;
    isSnap = false;
    isRecord = false;
    debug("关闭线程", "");
    emit receivePlayFinsh();
}

void VideoThread::replay() {
    //如果设置了重复循环播放则快到了文件末尾重新设置位置即可
    if (this->getPlayRepeat() && (this->getDuration() - position) < 500) {
        emit receiveReplay();
        QMetaObject::invokeMethod(this, "setPosition", Q_ARG(qint64, 0));
    }
}

void VideoThread::debug(const QString &head, const QString &msg) {
    AbstractVideoThread::debug(head, msg, mediaUrl);
}

qint64 VideoThread::getDuration2() const {
    return this->duration;
}

void VideoThread::setDuration2(qint64 duration) {
    this->duration = duration;
    //在mpv内核中如果连续多次时长不一致则判定为视频流而不是视频文件
    if (playCount < 2) {
        this->firstDuration = duration;
    }
}

void VideoThread::setPosition2(qint64 position) {
    this->position = position;
    this->playCount++;
}

VideoCore VideoThread::getVideoCore() const {
    return this->videoCore;
}

void VideoThread::setVideoCore(const VideoCore &videoCore) {
    this->videoCore = videoCore;
}

QString VideoThread::getMediaUrl() const {
    return this->mediaUrl;
}

void VideoThread::setMediaUrl(const QString &mediaUrl) {
    //海康大华等厂家sdk只支持rtsp和mp4
#if 0
    if (videoCore == VideoCore_HaiKang || videoCore == VideoCore_DaHua) {
        if (!mediaUrl.startsWith("rtsp://") && !mediaUrl.endsWith(".mp4")) {
            this->mediaUrl = "";
            return;
        }
    }
#endif

    this->mediaUrl = mediaUrl;
    UrlHelper::checkPrefix(this->mediaUrl);
    this->addr = UrlHelper::getAddr(this->mediaUrl);
    this->mediaType = VideoHelper::getMediaType(this->mediaUrl);
}

bool VideoThread::getIsFile() const {
    return (mediaType == MediaType_FileLocal || mediaType == MediaType_FileWeb);
}

bool VideoThread::getIsMp3() const {
    return (mediaType == MediaType_FileLocal && mediaUrl.endsWith(".mp3"));
}

void VideoThread::checkOnlyAudio() {
    //没有视频尺寸以及mp3文件都是纯音频/可能存在封面导致有尺寸
    if (videoWidth <= 0) {
        onlyAudio = true;
    } else if (mediaUrl.endsWith(".mp3")) {
        onlyAudio = true;
    } else {
        onlyAudio = false;
    }

    //mpv内核本地音频文件会默认生成一个 75x75 的封面
    if (videoCore == VideoCore_Mpv && mediaType == MediaType_FileLocal) {
        if (videoWidth == 75 && videoHeight == 75) {
            onlyAudio = true;
        }
    }

    emit receivePlayStart(timer.elapsed());
}

bool VideoThread::getOnlyAudio() const {
    return this->onlyAudio;
}

void VideoThread::checkMediaType() {
    //特别提示 有些可能带了 cache: 开头/这个是ffmpeg独有的写法
    QString url = mediaUrl;
    url.replace("cache:", "");
    bool http = (url.startsWith("http://") || url.startsWith("https://"));
    bool web = (http || url.startsWith("rtsp://") || url.startsWith("rtsps://"));
    if (duration > 0) {
        if (videoCore == VideoCore_Vlc) {
            //经过测试发现实时视频流会发送个固定90000=90s时长的数据
            if (duration == 90000) {
                if (http) {
                    mediaType = MediaType_Http;
                }
            } else {
                this->checkMediaType(web);
            }
        } else if (videoCore == VideoCore_Mpv) {
            //经过测试发现实时视频流也会不断触发时长和播放进度变化
            //如果是单纯的视频文件(本地视频/网络视频)只会触发1-2次时长变化和播放进度变化
            if (playCount == 5) {
                if (duration == firstDuration) {
                    QMetaObject::invokeMethod(this, "receiveDuration", Q_ARG(qint64, duration));
                    if (web) {
                        mediaType = MediaType_FileWeb;
                    }
                } else {
                    if (http) {
                        mediaType = MediaType_Http;
                    }
                }
            }
        } else if (videoCore == VideoCore_Mdk) {
            //限定低于3s时长的为视频流
            if (duration < 3000) {
                if (http) {
                    mediaType = MediaType_Http;
                }
            } else {
                this->checkMediaType(web);
            }
        } else if (videoCore == VideoCore_EasyPlayer) {
            //经过测试发现实时视频流会发送个固定1s时长的数据
            if (duration == 1) {
                if (http) {
                    mediaType = MediaType_Http;
                }
            } else {
                this->checkMediaType(web);
            }
        } else {
            this->checkMediaType(web);
        }
    } else {
        if (http) {
            mediaType = MediaType_Http;
        } else if (getIsFile()) {
            //有一些裸流本地文件没有时长
            mediaType = MediaType_Other;
        }
    }

    //突然发现网络地址是文件则再次设置倍速
    if (mediaType == MediaType_FileWeb && (videoCore == VideoCore_Vlc || videoCore == VideoCore_Mpv)) {
        this->setSpeed(this->property("speed").toDouble());
    }
}

void VideoThread::checkMediaType(bool web) {
    if (web) {
        mediaType = MediaType_FileWeb;
    } else if (QFile(mediaUrl).exists()) {
        mediaType = MediaType_FileLocal;
    }
}

MediaType VideoThread::getMediaType() const {
    return this->mediaType;
}

QString VideoThread::getBufferSize() const {
    return this->bufferSize;
}

void VideoThread::setBufferSize(const QString &bufferSize) {
    this->bufferSize = bufferSize;
}

DecodeType VideoThread::getDecodeType() const {
    return this->decodeType;
}

void VideoThread::setDecodeType(const DecodeType &decodeType) {
    this->decodeType = decodeType;
}

QString VideoThread::getHardware() const {
    return this->hardware;
}

void VideoThread::setHardware(const QString &hardware) {
    this->hardware = hardware;
}

QString VideoThread::getTransport() const {
    return this->transport;
}

void VideoThread::setTransport(const QString &transport) {
    this->transport = transport;
}

int VideoThread::getCaching() const {
    return this->caching;
}

void VideoThread::setCaching(int caching) {
    this->caching = caching;
}

bool VideoThread::getAudioLevel() const {
    return this->audioLevel;
}

void VideoThread::setAudioLevel(bool audioLevel) {
    this->audioLevel = audioLevel;
}

bool VideoThread::getDecodeAudio() const {
    return this->decodeAudio;
}

void VideoThread::setDecodeAudio(bool decodeAudio) {
    this->decodeAudio = decodeAudio;
}

bool VideoThread::getPlayAudio() const {
    return this->playAudio;
}

void VideoThread::setPlayAudio(bool playAudio) {
    //切换播放声音期间需要清空音频振幅数据
    this->playAudio = playAudio;
    emit receiveLevel(0, 0);
}

bool VideoThread::getPlayRepeat() const {
    return this->playRepeat;
}

void VideoThread::setPlayRepeat(bool playRepeat) {
    this->playRepeat = playRepeat;
}

bool VideoThread::getRealBitRate() const {
    return this->realBitRate;
}

void VideoThread::setRealBitRate(bool realBitRate) {
    this->realBitRate = realBitRate;
}

QString VideoThread::getEncryptKey() const {
    return this->encryptKey;
}

void VideoThread::setEncryptKey(const QString &encryptKey) {
    //将原始秘钥按照MD5加密转换成最终秘钥字符串
    this->encryptKey = encryptKey;
    if (!encryptKey.isEmpty()) {
        QByteArray cryptoKey = QCryptographicHash::hash(encryptKey.toUtf8(), QCryptographicHash::Md5).toHex();
        this->setProperty("encryptKey", cryptoKey);
    }
}

QString VideoThread::getDecryptKey() const {
    return this->decryptKey;
}

void VideoThread::setDecryptKey(const QString &decryptKey) {
    //将原始秘钥按照MD5加密转换成最终秘钥字符串
    this->decryptKey = decryptKey;
    if (!decryptKey.isEmpty()) {
        QByteArray cryptoKey = QCryptographicHash::hash(decryptKey.toUtf8(), QCryptographicHash::Md5).toHex();
        this->setProperty("decryptKey", cryptoKey);
    }
}

void VideoThread::clearSecretKey() {
    this->setProperty("encryptKey", "");
    this->setProperty("decryptKey", "");
}

void VideoThread::setSecretKey(const QString &encryptKey, const QString &decryptKey) {
    this->setEncryptKey(encryptKey);
    this->setDecryptKey(decryptKey);
}

int VideoThread::getOpenSleepTime() const {
    return this->openSleepTime;
}

void VideoThread::setOpenSleepTime(int openSleepTime) {
    this->openSleepTime = openSleepTime;
}

int VideoThread::getReadTimeout() const {
    return this->readTimeout;
}

void VideoThread::setReadTimeout(int readTimeout) {
    this->readTimeout = readTimeout;
}

int VideoThread::getConnectTimeout() const {
    return this->connectTimeout;
}

void VideoThread::setConnectTimeout(int connectTimeout) {
    this->connectTimeout = connectTimeout;
}

void VideoThread::startDetect() {
    isDetect = true;
}

void VideoThread::stopDetect() {
    isDetect = false;
}

void VideoThread::readMediaInfo() {

}

void VideoThread::setAudioTrack(int audioTrack) {
    this->audioTrack = audioTrack;
}

void VideoThread::setVideoTrack(int videoTrack) {
    this->videoTrack = videoTrack;
}

void VideoThread::setTrack(int audioTrack, int videoTrack) {
    this->audioTrack = audioTrack;
    this->videoTrack = videoTrack;
    this->setAudioTrack(audioTrack);
    this->setVideoTrack(videoTrack);
}

void VideoThread::setTrack() {
    //如果设置过了则立即切换轨道
    if (audioTrack >= 0 && audioTracks.contains(audioTrack)) {
        this->setAudioTrack(audioTrack);
    }
    if (videoTrack >= 0 && videoTracks.contains(videoTrack)) {
        this->setVideoTrack(videoTrack);
    }

    if (audioTracks.count() > 0 || videoTracks.count() > 0) {
        emit receiveTrack(audioTracks, videoTracks);
    }
}

int VideoThread::getRotate() {
    return rotate;
}

void VideoThread::setRotate(int rotate) {
    this->rotate = rotate;
}

void VideoThread::setAspect(double width, double height) {

}

qint64 VideoThread::getDuration() {
    return duration;
}

qint64 VideoThread::getPosition() {
    return position;
}

void VideoThread::setPosition(qint64 position) {
    emit receivePosition(position);
}

double VideoThread::getSpeed() {
    return 1;
}

void VideoThread::setSpeed(double speed) {

}

int VideoThread::getVolume() {
    return 100;
}

void VideoThread::setVolume(int volume) {
    emit receiveVolume(volume);
}

bool VideoThread::getMuted() {
    return false;
}

void VideoThread::setMuted(bool muted) {
    emit receiveMuted(muted);
}

bool VideoThread::getPlayStep() const {
    return this->playStep;
}

void VideoThread::setPlayStep(bool playStep) {
    if (this->playStep != playStep) {
        playStep ? this->pause() : this->next();
        this->playStep = playStep;
    }
}

QString VideoThread::getAudioDevice() const {
    return this->audioDevice;
}

void VideoThread::setAudioDevice(const QString &audioDevice) {
    this->audioDevice = audioDevice;
}

void VideoThread::stop2() {
    if (readTimeout > 0 && isOk && isRunning()) {
        debug("重新打开", "");
        isOk = false;
        return;
    }

    if (videoWidget) {
        //this->stop(false);
        //如果在系统非常繁忙且CPU资源很紧张的情况下/执行下面这行可能出现崩溃/找遍全宇宙原因未知/请用上面这行代码
        QMetaObject::invokeMethod(videoWidget, "stop");
    } else {
        this->stop(false);
    }
}

void VideoThread::stop3() {
    //先发送打开错误信号
    emit receivePlayError(VideoError_Open);
    //如果没有启用重连则停止线程
    if (readTimeout == 0) {
        this->stop2();
        emit receivePlayFinsh();
    }
}

void VideoThread::snapFinsh() {
    //文件已经存在不用重新保存(png格式重新保存为jpg以减少体积)
    QImage image(snapName);
    if (snapName.endsWith(".png")) {
        QFile(snapName).remove();
        snapName = snapName.replace(".png", ".jpg");
        //vlc内核需要重新旋转
        if (videoCore == VideoCore_Vlc) {
            VideoHelper::rotateImage(rotate, image);
        }
        image.save(snapName, "jpg");
    }

    isSnap = false;
    emit snapImage(image, snapName);
}

void VideoThread::openFinsh(bool open) {
    isOk = true;
    this->updateTime();
    emit recorderStateChanged(RecorderState_Stopped, fileName);
    int time = timer.elapsed();
    debug(QString("打开%1").arg(open ? "成功" : "完成"), QString("用时: %1 毫秒").arg(time));
}

void VideoThread::step(bool backward) {
    //本身没有逐帧播放接口/按照帧率计算的间隔来跳转播放进度实现
    //如果子类实现了step函数则会执行子类的代码
    if (getIsFile() && playStep) {
        int offset = 1000 / 25;
        if (frameRate > 0) {
            offset = 1000 / frameRate;
        }

        qint64 position = this->getPosition();
        position = (backward ? (position - offset) : (position + offset));
        this->setPosition(position);
    }
}

void VideoThread::recordStartFinsh() {
    isRecord = true;
    emit recorderStateChanged(RecorderState_Recording, fileName);
    if (isPush(fileName)) {
        debug("开始推流", QString("地址: %1").arg(fileName));
    } else {
        debug("开始录制", QString("文件: %1").arg(fileName));
    }
}

void VideoThread::recordStopFinsh() {
    isRecord = false;
    emit recorderStateChanged(RecorderState_Stopped, fileName);
    if (isPush(fileName)) {
        debug("结束推流", QString("地址: %1").arg(fileName));
    } else {
        debug("结束录制", QString("文件: %1").arg(fileName));
    }
}
