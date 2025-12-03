#include "abstractvideothread.h"
#include "deviceinfohelper.h"
#include "urlhelper.h"

#ifdef videosave
#include "savevideo.h"
#include "saveaudio.h"
#endif

#ifdef ffmpeg

#include "ffmpegsave.h"

#endif

int AbstractVideoThread::debugInfo = 2;
bool AbstractVideoThread::snapSource = false;

AbstractVideoThread::AbstractVideoThread(QObject *parent) : QThread(parent) {
    //注册数据类型
    qRegisterMetaType<RecorderState>("RecorderState");
    qRegisterMetaType<QList<int> >("QList<int>");

    stopped = false;
    isOk = false;
    isPause = false;
    isSnap = false;
    isRecord = false;

    errorCount = 0;
    lastTime = QDateTime::currentDateTime();
    hwndWidget = (QWidget *) parent;

    videoMode = VideoMode_Hwnd;
    videoSize = "0x0";
    videoWidth = 0;
    videoHeight = 0;
    rotate = -1;

    sampleRate = 8000;
    channelCount = 1;
    profile = 1;
    onlyAudio = false;

    brightness = 0;
    contrast = 0;
    hue = 0;
    saturation = 0;

    formatName = "";
    videoCodecName = "";
    audioCodecName = "";

    snapName = "";
    fileName = "";

    findFaceRect = false;
    findFaceOne = false;

    saveVideoType = SaveVideoType_None;
    saveAudioType = SaveAudioType_None;

    encodeSpeed = 1;
    encodeAudio = EncodeAudio_Auto;
    encodeVideo = EncodeVideo_None;
    encodeVideoFps = 0;
    encodeVideoRatio = 1;
    encodeVideoScale = "1";

#ifdef videosave
    saveVideo = new SaveVideo(this);
    saveAudio = new SaveAudio(this);
#endif
#ifdef ffmpeg
    saveFile = new FFmpegSave(this);
#endif
}

AbstractVideoThread::~AbstractVideoThread() {
    this->stop();
}

void AbstractVideoThread::setGeometry() {

}

void AbstractVideoThread::initFilter() {

}

bool AbstractVideoThread::getIsOk() const {
    return this->isOk;
}

bool AbstractVideoThread::getIsPause() const {
    return this->isPause;
}

bool AbstractVideoThread::getIsSnap() const {
    return this->isSnap;
}

bool AbstractVideoThread::getIsRecord() const {
    return this->isRecord;
}

bool AbstractVideoThread::getIsDetect() const {
    return this->isDetect;
}

void AbstractVideoThread::updateTime() {
    lastTime = QDateTime::currentDateTime();
}

QElapsedTimer *AbstractVideoThread::getTimer() {
    return &timer;
}

QString AbstractVideoThread::getAddr() const {
    return this->addr;
}

QString AbstractVideoThread::getFlag() const {
    return this->flag;
}

VideoMode AbstractVideoThread::getVideoMode() const {
    return this->videoMode;
}

void AbstractVideoThread::setVideoMode(const VideoMode &videoMode) {
    this->videoMode = videoMode;
}

int AbstractVideoThread::getVideoWidth() const {
    return this->videoWidth;
}

int AbstractVideoThread::getVideoHeight() const {
    return this->videoHeight;
}

void AbstractVideoThread::setVideoSize(const QString &videoSize) {
    this->videoSize = videoSize;
    QStringList sizes = DeviceInfoHelper::getSizes(videoSize);
    if (sizes.count() == 2) {
        videoWidth = sizes.at(0).toInt();
        videoHeight = sizes.at(1).toInt();
    }
}

void AbstractVideoThread::checkVideoSize(int width, int height) {
    if (width > 0 && height > 0 && videoWidth > 0 && videoHeight > 0) {
        if (videoWidth != width || videoHeight != height) {
            videoWidth = width;
            videoHeight = height;
            QMetaObject::invokeMethod(this, "receiveSizeChanged");
        }
    }
}

void AbstractVideoThread::addWidget(QWidget *widget) {
    //必须是视频而且句柄模式才需要加入视频控件
    if (onlyAudio || videoMode != VideoMode_Hwnd) {
        return;
    }

    //有时候可能只是线程在用/没有设置窗体展示/比如只需要后台解码的情况
    if (hwndWidget && hwndWidget->layout()) {
        hwndWidget->layout()->addWidget(widget);
    }

    QMetaObject::invokeMethod(widget, "show");
}

qreal AbstractVideoThread::getFrameRate() const {
    return this->frameRate;
}

void AbstractVideoThread::setFrameRate(qreal frameRate) {
    this->frameRate = frameRate;
}

int AbstractVideoThread::getRotate() const {
    return this->rotate;
}

void AbstractVideoThread::setRotate(int rotate) {
    this->rotate = rotate;
}

QString AbstractVideoThread::getFormatName() const {
    return this->formatName;
}

QString AbstractVideoThread::getVideoCodecName() const {
    return this->videoCodecName;
}

void AbstractVideoThread::setVideoCodecName(const QString &videoCodecName) {
    this->videoCodecName = videoCodecName.toLower();
}

QString AbstractVideoThread::getAudioCodecName() const {
    return this->audioCodecName;
}

void AbstractVideoThread::setAudioCodecName(const QString &audioCodecName) {
    this->audioCodecName = audioCodecName.toLower();
}

bool AbstractVideoThread::isPush(const QString &fileName) {
    static QStringList flags;
    if (flags.count() == 0) {
        //可以自行增加其他种类的推流前缀
        flags << "rtmp://" << "rtmps://" << "rtsp://" << "rtsps://" << "srt://" << "udp://" << "tcp://";
    }

            foreach (QString flag, flags) {
            if (fileName.startsWith(flag)) {
                return true;
            }
        }

    return false;
}

void AbstractVideoThread::checkPath(const QString &fileName) {
    //推流则不用继续
    if (isPush(fileName)) {
        return;
    }

    //补全完整路径
    QString path = QFileInfo(fileName).path();
    if (path.startsWith("./")) {
        path.replace("./", qApp->applicationDirPath() + "/");
    }

    //目录不存在则新建
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }
}

QString AbstractVideoThread::getSnapName() const {
    return this->snapName;
}

void AbstractVideoThread::setSnapName(const QString &snapName) {
    this->snapName = snapName;
    this->checkPath(snapName);
}

QString AbstractVideoThread::getFileName() const {
    return this->fileName;
}

void AbstractVideoThread::setFileName(const QString &fileName) {
    this->fileName = fileName;
    UrlHelper::checkPrefix(this->fileName);
    this->checkPath(this->fileName);
}

void AbstractVideoThread::setFindFaceRect(bool findFaceRect) {
    this->findFaceRect = findFaceRect;
}

void AbstractVideoThread::setFindFaceOne(bool findFaceOne) {
    this->findFaceOne = findFaceOne;
}

SaveVideoType AbstractVideoThread::getSaveVideoType() const {
    return this->saveVideoType;
}

void AbstractVideoThread::setSaveVideoType(const SaveVideoType &saveVideoType) {
    this->saveVideoType = saveVideoType;
}

SaveAudioType AbstractVideoThread::getSaveAudioType() const {
    return this->saveAudioType;
}

void AbstractVideoThread::setSaveAudioType(const SaveAudioType &saveAudioType) {
    this->saveAudioType = saveAudioType;
}

double AbstractVideoThread::getEncodeSpeed() const {
    return this->encodeSpeed;
}

void AbstractVideoThread::setEncodeSpeed(double encodeSpeed) {
    this->encodeSpeed = encodeSpeed;
}

EncodeAudio AbstractVideoThread::getEncodeAudio() const {
    return this->encodeAudio;
}

void AbstractVideoThread::setEncodeAudio(const EncodeAudio &encodeAudio) {
    this->encodeAudio = encodeAudio;
}

EncodeVideo AbstractVideoThread::getEncodeVideo() const {
    return this->encodeVideo;
}

void AbstractVideoThread::setEncodeVideo(const EncodeVideo &encodeVideo) {
    this->encodeVideo = encodeVideo;
}

int AbstractVideoThread::getEncodeVideoFps() const {
    return this->encodeVideoFps;
}

void AbstractVideoThread::setEncodeVideoFps(int encodeVideoFps) {
    if (encodeVideoFps >= 1 && encodeVideoRatio <= 30) {
        this->encodeVideoFps = encodeVideoFps;
    }
}

float AbstractVideoThread::getEncodeVideoRatio() const {
    return this->encodeVideoRatio;
}

void AbstractVideoThread::setEncodeVideoRatio(float encodeVideoRatio) {
    if (encodeVideoRatio >= 0) {
        this->encodeVideoRatio = encodeVideoRatio;
    }
}

QString AbstractVideoThread::getEncodeVideoScale() const {
    return this->encodeVideoScale;
}

void AbstractVideoThread::setEncodeVideoScale(const QString &encodeVideoScale) {
    this->encodeVideoScale = encodeVideoScale;
}

void AbstractVideoThread::setFlag(const QString &flag) {
    this->flag = flag;
#ifdef videosave
    saveVideo->setFlag(flag);
    saveAudio->setFlag(flag);
#endif
#ifdef ffmpeg
    saveFile->setFlag(flag);
#endif
}

void AbstractVideoThread::debug(const QString &head, const QString &msg, const QString &url) {
    if (debugInfo == 0) {
        return;
    }

    //如果设置了唯一标识则放在打印前面
    QString text = head;
    if (!flag.isEmpty()) {
        text = QString("标识[%1] %2").arg(flag).arg(text);
    }

    QString addr = (debugInfo == 2 ? this->addr : url);
    if (msg.isEmpty()) {
        qDebug() << TIMEMS << QString("%1 -> 地址: %2").arg(text).arg(addr);
    } else {
        qDebug() << TIMEMS << QString("%1 -> %2 地址: %3").arg(text).arg(msg).arg(addr);
    }
}

void AbstractVideoThread::setImage(const QImage &image) {
    this->updateTime();
    emit receiveImage(image, 0);
}

void AbstractVideoThread::setRgb(int width, int height, quint8 *dataRGB, int type) {
    this->updateTime();
    emit receiveFrame(width, height, dataRGB, type);
}

void AbstractVideoThread::setYuv(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV) {
    this->updateTime();
    emit receiveFrame(width, height, dataY, dataU, dataV, width, width / 2, width / 2);
}

void AbstractVideoThread::play() {
    //没有运行才需要启动
    if (!this->isRunning()) {
        stopped = false;
        isPause = false;
        isSnap = false;
        this->start();
    }
}

void AbstractVideoThread::stop(bool wait) {
    //处于运行状态才可以停止
    if (this->isRunning()) {
        stopped = true;
        isPause = false;
        isSnap = false;
        if (wait) {
            this->wait();
        }
    }
}

void AbstractVideoThread::pause() {
    if (this->isRunning()) {
        isPause = true;
    }
}

void AbstractVideoThread::next() {
    if (this->isRunning()) {
        isPause = false;
    }
}

void AbstractVideoThread::snap(const QString &snapName) {
    if (this->isRunning()) {
        isSnap = true;
        this->setSnapName(snapName);
    }
}

void AbstractVideoThread::snapFinsh(const QImage &image) {
    //如果填了截图文件名称则先保存图片
    if (!snapName.isEmpty() && !image.isNull()) {
        //取出拓展名根据拓展名保存格式
        QString suffix = snapName.split(".").last();
        image.save(snapName, suffix.toLatin1().constData());
    }

    //发送截图完成信号
    emit snapImage(image, snapName);
}

QImage AbstractVideoThread::getImage() {
    return QImage();
}

void AbstractVideoThread::recordStart(const QString &fileName) {
#ifdef videosave
    this->setFileName(fileName);
    if (saveAudioType > 0) {
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveAudio->getIsPause()) {
            saveAudio->pause();
            emit recorderStateChanged(RecorderState_Recording, saveAudio->getFileName());
        } else {
            saveAudio->setPara(saveAudioType, sampleRate, channelCount, profile);
            saveAudio->open(fileName);
            if (saveAudio->getIsOk()) {
                emit recorderStateChanged(RecorderState_Recording, saveAudio->getFileName());
            }
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveVideo->getIsPause()) {
            isRecord = true;
            saveVideo->pause();
            emit recorderStateChanged(RecorderState_Recording, saveVideo->getFileName());
        } else {
            saveVideo->setPara(saveVideoType, videoWidth, videoHeight, frameRate);
            saveVideo->open(fileName);
            if (saveVideo->getIsOk()) {
                isRecord = true;
                emit recorderStateChanged(RecorderState_Recording, saveVideo->getFileName());
            }
        }
    }
#endif
}

void AbstractVideoThread::recordPause() {
#ifdef videosave
    if (saveAudioType > 0) {
        if (saveAudio->getIsOk()) {
            saveAudio->pause();
            emit recorderStateChanged(RecorderState_Paused, saveAudio->getFileName());
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        if (saveVideo->getIsOk()) {
            isRecord = false;
            saveVideo->pause();
            emit recorderStateChanged(RecorderState_Paused, saveVideo->getFileName());
        }
    }
#endif
}

void AbstractVideoThread::recordStop() {
#ifdef videosave
    if (saveAudioType > 0) {
        if (saveAudio->getIsOk()) {
            saveAudio->stop();
            emit recorderStateChanged(RecorderState_Stopped, saveAudio->getFileName());
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        if (saveVideo->getIsOk()) {
            isRecord = false;
            saveVideo->stop();
            emit recorderStateChanged(RecorderState_Stopped, saveVideo->getFileName());
        }
    }
#endif
}

void AbstractVideoThread::writeVideoData(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV) {
#ifdef videosave
    if (saveVideoType == SaveVideoType_Yuv && isRecord) {
        saveVideo->setPara(SaveVideoType_Yuv, width, height, frameRate);
        saveVideo->write(dataY, dataU, dataV);
    }
#endif
}

void AbstractVideoThread::writeAudioData(const char *data, qint64 len) {
#ifdef videosave
    if (saveAudioType > 0 && saveAudio->getIsOk()) {
        saveAudio->write(data, len);
    }
#endif
}

void AbstractVideoThread::writeAudioData(const QByteArray &data) {
#ifdef videosave
    if (saveAudioType > 0 && saveAudio->getIsOk()) {
        this->writeAudioData(data.constData(), data.length());
    }
#endif
}

void AbstractVideoThread::setOsdInfo(const QList<OsdInfo> &listOsd) {
    this->listOsd = listOsd;
    this->initFilter();
}

void AbstractVideoThread::setGraphInfo(const QList<GraphInfo> &listGraph) {
    this->listGraph = listGraph;
    this->initFilter();
}
