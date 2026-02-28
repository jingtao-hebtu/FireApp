#include "videowidgetx.h"
#include "videohelper.h"
#include "videotask.h"
#include "bannerwidget.h"
#include "widgethelper.h"
#include "urlhelper.h"
#include "DetectionQueueManager.h"
#include "DetectorWorkerManager.h"
#include "TFMeaManager.h"


VideoWidget::VideoWidget(QWidget *parent) : AbstractVideoWidget(parent) {
    videoWidth = 0;
    videoHeight = 0;
    rotate = -1;
    videoThread = NULL;

    //关联按钮单击事件(对应功能可以直接处理掉)
    connect(this, SIGNAL(sig_btnClicked(QString)), this, SLOT(btnClicked(QString)));
    //关联鼠标按下用于电子放大
    connect(this, SIGNAL(sig_receivePoint(int, QPoint)), this, SLOT(receivePoint(int, QPoint)));
    //关联标签和图形信息变化
    connect(this, SIGNAL(sig_osdChanged()), this, SLOT(osdChanged()));
    connect(this, SIGNAL(sig_graphChanged()), this, SLOT(graphChanged()));
}

VideoWidget::~VideoWidget() {
    this->stop();
}

void VideoWidget::resizeEvent(QResizeEvent *e) {
    if (!videoThread) {
        AbstractVideoWidget::resizeEvent(NULL);
        return;
    }

    //主动调用resizeEvent(NULL)后产生的sender有对象也需要触发
    if (e || sender()) {
        //将尺寸等信息赋值给基类(基类那边需要用到这些变量)
        videoWidth = videoThread->getVideoWidth();
        videoHeight = videoThread->getVideoHeight();
        //频谱区域高度(easyplayer内核用于底部显示可视化频谱)
        if (videoPara.videoCore == VideoCore_EasyPlayer) {
            visualHeight = videoHeight * 0.1;
        }

        rotate = videoThread->getRotate();
        onlyAudio = videoThread->getOnlyAudio();
        hardware = videoThread->getHardware();

        //先执行父类的尺寸变化
        AbstractVideoWidget::resizeEvent(NULL);
        videoThread->debug("窗体拉伸", QString("宽高: %1x%2 角度: %3").arg(videoWidth).arg(videoHeight).arg(rotate));

        //在拉伸填充模式下可能还没获取到尺寸就会触发一次
        if (!onlyAudio && (videoWidth <= 0 || videoHeight <= 0)) {
            return;
        }

        //vlc核mpv内核句柄拉伸填充模式下需要主动设置拉伸比
        if (widgetPara.videoMode == VideoMode_Hwnd) {
            if (videoPara.videoCore == VideoCore_Mpv) {
                if (widgetPara.scaleMode == ScaleMode_Fill) {
                    this->resize2();
                }
            } else if (videoPara.videoCore == VideoCore_Vlc) {
                if (widgetPara.scaleMode == ScaleMode_Fill) {
                    this->resize2();
                    //vlc内核视频流有时候需要再执行一次
                    if (!videoThread->getIsFile()) {
                        QTimer::singleShot(100, this, SLOT(resize2()));
                    }
                } else if (videoThread->getMediaType() == MediaType_Device) {
                    this->setAspect(videoWidth, videoHeight);
                }
            }
        }
    }
}

VideoPara VideoWidget::getVideoPara() const {
    return this->videoPara;
}

void VideoWidget::setVideoPara(const VideoPara &videoPara) {
    this->videoPara = videoPara;
}

QString VideoWidget::getBannerText() const {
    if (!getIsRunning()) {
        return QString();
    }

    //悬浮条如果宽度不够则不显示文字信息
    int width = bannerWidget->getLabelWidth();
    if (width < 100) {
        return QString();
    }

    //线程已经开启并处于打开中/还未打开完成
    if (!videoThread->getIsOk()) {
        return QString("打开中...");
    }

    QStringList list;
    if (bannerInfo.resolution) {
        if (videoWidth > 0) {
            list << QString("%1%2 x %3").arg(width > 150 ? "分辨率: " : "").arg(videoWidth).arg(videoHeight);
        }
    }

    if (bannerInfo.frameRate) {
        int fps = videoThread->getFrameRate();
        if (fps > 0) {
            list << QString("帧率: %1").arg(fps);
        }
    }

    if (bannerInfo.mediaUrl) {
        QString addr = videoThread->getAddr();
        if (!addr.isEmpty() && UrlHelper::isIP(addr)) {
            list << QString("地址: %1").arg(addr);
        }
    }

    if (bannerInfo.mediaName) {
        QString name = this->property("mediaName").toString();
        if (!name.isEmpty()) {
            list << QString("名称: %1").arg(name);
        }
    }

    if (bannerInfo.formatName) {
        QString name = VideoHelper::getFormatName(videoThread);
        if (!name.isEmpty()) {
            list << QString("格式: %1").arg(name);
        }
    }

    if (bannerInfo.videoCodecName) {
        QString name = videoThread->getVideoCodecName();
        if (!name.isEmpty()) {
            list << QString("视频: %1").arg(name);
        }
    }

    if (bannerInfo.audioCodecName) {
        QString name = videoThread->getAudioCodecName();
        if (!name.isEmpty()) {
            list << QString("音频: %1").arg(name);
        }
    }

    if (bannerInfo.realBitRate) {
        if (kbps > 0) {
            list << QString("码率: %1kbps").arg((int) kbps);
        }
    }

    if (bannerInfo.hardware && !this->onlyAudio) {
        QString hardware = videoThread->getHardware();
        list << QString("硬解: %1").arg(hardware);
    }

    if (bannerInfo.videoMode && !this->onlyAudio) {
        QString videoMode = "句柄";
        if (widgetPara.videoMode == VideoMode_Opengl) {
            videoMode = "GPU";
        } else if (widgetPara.videoMode == VideoMode_Painter) {
            videoMode = "绘制";
        }
        list << QString("模式: %1").arg(videoMode);
    }

    return list.join("  ");
}

void VideoWidget::setAudioLevel(bool audioLevel) {
    if (videoThread) {
        videoThread->setAudioLevel(audioLevel);
    }
}

void VideoWidget::setRealBitRate(bool realBitRate) {
    if (videoThread) {
        videoThread->setRealBitRate(realBitRate);
    }
}

bool VideoWidget::getIsPause() const {
    if (videoThread) {
        return videoThread->getIsPause();
    } else {
        return false;
    }
}

bool VideoWidget::getIsRecord() const {
    return bannerWidget->getIsRecord();
}

bool VideoWidget::getIsCrop() const {
    return bannerWidget->getIsCrop();
}

QImage VideoWidget::getImage() const {
    QImage image;
    if (videoThread) {
        //优先取本地绘制的图片
        if (!this->image.isNull()) {
            image = this->image;
        } else {
            image = videoThread->getImage();
        }
    }

    return image;
}

VideoThread *VideoWidget::getVideoThread() const {
    return this->videoThread;
}

void VideoWidget::setPlayRepeat(bool playRepeat) {
    if (videoThread) {
        videoPara.playRepeat = playRepeat;
        videoThread->setPlayRepeat(playRepeat);
    }
}

void VideoWidget::setReadTimeout(int readTimeout) {
    if (videoThread) {
        videoPara.readTimeout = readTimeout;
        videoThread->setReadTimeout(readTimeout);
    }
}

void VideoWidget::setAspect(double width, double height) {
    if (videoThread) {
        videoThread->setAspect(width, height);
    }
}

qint64 VideoWidget::getPosition() {
    if (videoThread) {
        return videoThread->getPosition();
    } else {
        return 0;
    }
}

void VideoWidget::setPosition(qint64 position) {
    if (videoThread) {
        videoThread->setPosition(position);
    }
}

double VideoWidget::getSpeed() {
    if (videoThread) {
        return videoThread->getSpeed();
    } else {
        return 1;
    }
}

void VideoWidget::setSpeed(double speed) {
    if (videoThread) {
        videoThread->setProperty("speed", speed);
        videoThread->setSpeed(speed);
    }
}

int VideoWidget::getVolume() {
    if (videoThread) {
        return videoThread->getVolume();
    } else {
        return 100;
    }
}

void VideoWidget::setVolume(int volume) {
    //同时限定音量范围
    if (volume >= 0 && volume <= 100) {
        widgetPara.soundValue = volume;
    }
    if (videoThread) {
        videoThread->setVolume(widgetPara.soundValue);
    }
}

bool VideoWidget::getMuted() {
    if (videoThread) {
        return videoThread->getMuted();
    } else {
        return false;
    }
}

void VideoWidget::setMuted(bool muted) {
    widgetPara.soundMuted = muted;
    if (videoThread) {
        videoThread->setMuted(muted);
    }
}

void VideoWidget::setPlayStep(bool playStep) {
    if (videoThread) {
        videoPara.playStep = playStep;
        videoThread->setPlayStep(playStep);
    }
}

void VideoWidget::startDetect() {
    if (!videoThread) {
        return;
    }

    if (detectionEnabled) {
        return;
    }

    detectionEnabled = true;
    detectionFlag = videoThread->getFlag();

    TF::DetectionQueueManager::instance().start();
    auto &manager = TF::DetectorWorkerManager::instance();
    connect(&manager, &TF::DetectorWorkerManager::frameProcessed,
        this, &VideoWidget::receiveDetectedImage,
            Qt::UniqueConnection);
    manager.start();

    videoThread->startDetect();
}

void VideoWidget::stopDetect() {
    if (!detectionEnabled) {
        return;
    }

    detectionEnabled = false;
    detectionFlag.clear();

    if (videoThread) {
        videoThread->stopDetect();
    }
    TF::DetectionQueueManager::instance().stop();
    TF::DetectorWorkerManager::instance().stop();
}

void VideoWidget::resize2() {
    //如果有旋转角度则宽高对调
    if (rotate == 90) {
        this->setAspect(height(), width());
    } else {
        this->setAspect(width(), height());
    }
}

void VideoWidget::started() {
    isRunning = true;
    this->update();
}

void VideoWidget::finished() {
    isRunning = false;
    this->update();
}

void VideoWidget::receivePlayStart(int time) {
    //vlc内核句柄模式下在打开后鼠标打圈圈需要复位下
    if (widgetPara.videoMode == VideoMode_Hwnd) {
        VideoTask::Instance()->append("resetCursor", "");
    }

    //句柄模式下句柄控件要禁用绘制防止闪烁(尤其是海康大华厂家sdk内核)
    bool isQMedia = (videoPara.videoCore == VideoCore_QMedia);
    bool isMdk = (videoPara.videoCore == VideoCore_Mdk);
    bool isQtav = (videoPara.videoCore == VideoCore_Qtav);
    if (widgetPara.videoMode == VideoMode_Hwnd && !isQMedia && !isMdk && !isQtav) {
        hwndWidget->setUpdatesEnabled(false);
    }

    //重置码率
    kbps = 0;
    //执行父类处理
    AbstractVideoWidget::receivePlayStart(time);

    //设置默认声音大小和静音状态
    this->setVolume(widgetPara.soundValue);
    this->setMuted(widgetPara.soundMuted);

    //如果逐帧播放则先暂停并切换一次
    if (videoPara.playStep) {
        this->pause();
        this->step();
    }
}

void VideoWidget::receivePlayFinsh() {
    //mpv内核句柄模式下在关闭后鼠标打圈圈需要复位下
    if (widgetPara.videoMode == VideoMode_Hwnd) {
        VideoTask::Instance()->append("resetCursor", "");
    }

    //句柄模式下句柄控件要禁用绘制防止闪烁(尤其是海康大华厂家sdk内核)
    bool isQMedia = (videoPara.videoCore == VideoCore_QMedia);
    bool isMdk = (videoPara.videoCore == VideoCore_Mdk);
    bool isQtav = (videoPara.videoCore == VideoCore_Qtav);
    if (widgetPara.videoMode == VideoMode_Hwnd && !isQMedia && !isMdk && !isQtav) {
        hwndWidget->setUpdatesEnabled(true);
    }

    //执行父类处理
    AbstractVideoWidget::receivePlayFinsh();
}

bool VideoWidget::checkReceive(bool clear) {
    //可能在结束前的瞬间也发送过来了但是这边还没来得及绘制那边又释放了帧
    //所以要加个线程是否在运行的判断不然停止播放后可能会崩溃
    if (!isRunning) {
        //线程停止后设置一张空白图片
        if (clear) {
            AbstractVideoWidget::clear();
        }
        return false;
    }

    return true;
}

void VideoWidget::receiveImage(const QImage &image, int time) {
    if (!this->checkReceive(true)) {
        return;
    }

    if (detectionEnabled && videoThread && videoThread->getIsDetect()) {
        TF::DetectionQueueManager::instance().enqueue(detectionFlag, image, time);
        return;
    }

    AbstractVideoWidget::receiveImage(image, time);
}

void VideoWidget::receiveDetectedImage(const QString& flag, const QImage& image, double meanValue, int time) {
    if (!this->checkReceive(true)) {
        return;
    }

    if (!detectionEnabled || detectionFlag != flag) {
        return;
    }

    TF::TFMeaManager::instance().receiveNewValue(meanValue);

    Q_UNUSED(meanValue);
    AbstractVideoWidget::receiveImage(image, time);
}

void VideoWidget::receiveFrame(int width, int height, quint8 *dataRGB, int type) {
    if (this->checkReceive()) {
        AbstractVideoWidget::receiveFrame(width, height, dataRGB, type);
    }
}

void VideoWidget::receiveFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY,
                               quint32 linesizeU, quint32 linesizeV) {
    if (this->checkReceive()) {
        AbstractVideoWidget::receiveFrame(width, height, dataY, dataU, dataV, linesizeY, linesizeU, linesizeV);
    }
}

void
VideoWidget::receiveFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV) {
    if (this->checkReceive()) {
        AbstractVideoWidget::receiveFrame(width, height, dataY, dataUV, linesizeY, linesizeUV);
    }
}

void VideoWidget::receivePoint(int type, const QPoint &point) {
    if (!videoThread) {
        return;
    }

    //处于电子放大阶段才处理(悬浮条按钮切换)
    if (bannerWidget->getIsCrop()) {
        //限定只有一个电子放大滤镜
                foreach (GraphInfo graph, listGraph) {
                if (graph.name == "crop") {
                    return;
                }
            }

        //第一步: 鼠标按下开始记住坐标
        //第二步: 鼠标移动绘制选取区域
        //第三步: 鼠标松开发送裁剪滤镜
        if (type == QEvent::MouseButtonPress) {
            lastPoint = point;
        } else if (type == QEvent::MouseMove) {
            GraphInfo graph;
            graph.name = "croptemp";
            graph.rect = WidgetHelper::getRect(lastPoint, point);
            graph.borderWidth = WidgetHelper::getBorderWidth(this);
            this->setGraph(graph);
        } else if (type == QEvent::MouseButtonRelease) {
            GraphInfo graph;
            graph.name = "crop";
            graph.rect = WidgetHelper::getRect(lastPoint, point);
            this->removeGraph("croptemp");
            this->setGraph(graph);
        }
    }
}

void VideoWidget::btnClicked(const QString &btnName) {
    QString flag = widgetPara.videoFlag;
    QString name = STRDATETIMEMS;
    if (!flag.isEmpty()) {
        name = QString("%1_%2").arg(flag).arg(name);
    }

    if (btnName.endsWith("btnRecord")) {
        QString fileName = QString("%1/%2.mp4").arg(recordPath).arg(name);
        this->recordStart(fileName);
    } else if (btnName.endsWith("btnStop")) {
        this->recordStop();
    } else if (btnName.endsWith("btnSound")) {
        this->setMuted(true);
    } else if (btnName.endsWith("btnMuted")) {
        this->setMuted(false);
    } else if (btnName.endsWith("btnSnap")) {
        QString snapName = QString("%1/%2.jpg").arg(snapPath).arg(name);
        this->snap(snapName, false);
    } else if (btnName.endsWith("btnCrop")) {
        if (videoThread) {
            if (videoPara.videoCore == VideoCore_FFmpeg) {
                QMetaObject::invokeMethod(videoThread, "setCrop", Q_ARG(bool, true));
            }
        }
    } else if (btnName.endsWith("btnReset")) {
        if (videoThread) {
            this->removeGraph("crop");
            if (videoPara.videoCore == VideoCore_FFmpeg) {
                this->removeGraph("croptemp");
                QMetaObject::invokeMethod(videoThread, "setCrop", Q_ARG(bool, false));
            }
        }
    } else if (btnName.endsWith("btnAlarm")) {

    } else if (btnName.endsWith("btnClose")) {
        this->stop();
    }
}

void VideoWidget::osdChanged() {
    if (videoThread && widgetPara.osdDrawMode == DrawMode_Source) {
        videoThread->setOsdInfo(listOsd);
    }
}

void VideoWidget::graphChanged() {
    if (videoThread && widgetPara.graphDrawMode == DrawMode_Source) {
        videoThread->setGraphInfo(listGraph);
    }
}

void VideoWidget::connectThreadSignal() {
    if (!videoThread) {
        return;
    }

    //后面带个参数指定信号唯一(如果多次连接信号会自动去重)
    connect(videoThread, SIGNAL(started()), this, SLOT(started()), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(finished()), this, SLOT(finished()), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receivePlayStart(int)), this, SLOT(receivePlayStart(int)), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receivePlayFinsh()), this, SLOT(receivePlayFinsh()), Qt::UniqueConnection);

    connect(videoThread, SIGNAL(receiveImage(QImage, int)), this, SLOT(receiveImage(QImage, int)),
            Qt::UniqueConnection);
    connect(videoThread, SIGNAL(snapImage(QImage, QString)), this, SLOT(snapImage(QImage, QString)),
            Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , int)), this,
            SLOT(receiveFrame(int, int, quint8 * , int)), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , quint8 * , quint8 * , quint32, quint32, quint32)),
            this, SLOT(receiveFrame(int, int, quint8 * , quint8 * , quint8 * , quint32, quint32, quint32)),
            Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , quint8 * , quint32, quint32)),
            this, SLOT(receiveFrame(int, int, quint8 * , quint8 * , quint32, quint32)), Qt::UniqueConnection);

    connect(videoThread, SIGNAL(receiveLevel(qreal, qreal)), this, SIGNAL(sig_receiveLevel(qreal, qreal)),
            Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveKbps(qreal, int)), this, SIGNAL(sig_receiveKbps(qreal, int)));
    connect(videoThread, SIGNAL(receivePlayStart(int)), this, SIGNAL(sig_receivePlayStart(int)), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receivePlayFinsh()), this, SIGNAL(sig_receivePlayFinsh()), Qt::UniqueConnection);

    connect(videoThread, SIGNAL(receivePlayFinsh()), bannerWidget, SLOT(receivePlayFinsh()), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveMuted(bool)), bannerWidget, SLOT(receiveMuted(bool)), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(recorderStateChanged(RecorderState, QString)), bannerWidget,
            SLOT(recorderStateChanged(RecorderState, QString)), Qt::UniqueConnection);
    connect(videoThread, SIGNAL(receiveSizeChanged()), this, SLOT(receiveSizeChanged()));

    //根据默认音量大小和静音状态触发下信号
    if (videoPara.videoCore == VideoCore_FFmpeg) {
        QMetaObject::invokeMethod(videoThread, "receiveVolume", Q_ARG(int, widgetPara.soundValue));
        QMetaObject::invokeMethod(videoThread, "receiveMuted", Q_ARG(bool, widgetPara.soundMuted));
    }
}

void VideoWidget::disconnectThreadSignal() {
    if (!videoThread) {
        return;
    }

    disconnect(videoThread, SIGNAL(started()), this, SLOT(started()));
    disconnect(videoThread, SIGNAL(finished()), this, SLOT(finished()));
    disconnect(videoThread, SIGNAL(receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
    disconnect(videoThread, SIGNAL(receivePlayFinsh()), this, SLOT(receivePlayFinsh()));

    disconnect(videoThread, SIGNAL(receiveImage(QImage, int)), this, SLOT(receiveImage(QImage, int)));
    disconnect(videoThread, SIGNAL(snapImage(QImage, QString)), this, SLOT(snapImage(QImage, QString)));
    disconnect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , int)), this,
               SLOT(receiveFrame(int, int, quint8 * , int)));
    disconnect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , quint8 * , quint8 * , quint32, quint32, quint32)),
               this, SLOT(receiveFrame(int, int, quint8 * , quint8 * , quint8 * , quint32, quint32, quint32)));
    disconnect(videoThread, SIGNAL(receiveFrame(int, int, quint8 * , quint8 * , quint32, quint32)),
               this, SLOT(receiveFrame(int, int, quint8 * , quint8 * , quint32, quint32)));

    disconnect(videoThread, SIGNAL(receiveLevel(qreal, qreal)), this, SIGNAL(sig_receiveLevel(qreal, qreal)));
    disconnect(videoThread, SIGNAL(receiveKbps(qreal, int)), this, SIGNAL(sig_receiveKbps(qreal, int)));
    disconnect(videoThread, SIGNAL(receivePlayStart(int)), this, SIGNAL(sig_receivePlayStart(int)));
    disconnect(videoThread, SIGNAL(receivePlayFinsh()), this, SIGNAL(sig_receivePlayFinsh()));

    disconnect(videoThread, SIGNAL(receivePlayFinsh()), bannerWidget, SLOT(receivePlayFinsh()));
    disconnect(videoThread, SIGNAL(receiveMuted(bool)), bannerWidget, SLOT(receiveMuted(bool)));
    disconnect(videoThread, SIGNAL(recorderStateChanged(RecorderState, QString)), bannerWidget,
               SLOT(recorderStateChanged(RecorderState, QString)));
    disconnect(videoThread, SIGNAL(receiveSizeChanged()), this, SLOT(receiveSizeChanged()));
}

bool VideoWidget::init() {
    //媒体地址不能为空
    if (videoPara.mediaUrl.isEmpty()) {
        return false;
    }

    //如果没有解码内核则不用继续
    if (videoPara.videoCore == VideoCore_None) {
        return false;
    }

    //初始化参数
    VideoHelper::initPara(widgetPara, videoPara, encodePara);

    //线程正在运行不用继续
    if (isRunning) {
        return false;
    }

    //句柄模式则句柄控件在前否则遮罩控件在前
    if (widgetPara.videoMode == VideoMode_Hwnd) {
        coverWidget->stackUnder(hwndWidget);
    } else {
        hwndWidget->stackUnder(coverWidget);
    }

    //已经存在同名的线程则取同名线程
    VideoThread *thread = VideoThread::getVideoThread(widgetPara, videoPara);
    if (thread) {
        isShared = true;
        videoThread = thread;
        //默认音量大小和静音状态取共享线程的
        widgetPara.soundValue = thread->getVolume();
        widgetPara.soundMuted = thread->getMuted();
        //硬件加速也要取共享线程的
        hardware = thread->getHardware();
        videoPara.hardware = thread->getHardware();
    } else {
        //创建新的采集线程
        videoThread = VideoHelper::newVideoThread(hwndWidget, videoPara.videoCore, widgetPara.videoMode);
        //设置视频通道唯一标识
        videoThread->setFlag(widgetPara.videoFlag);
        //加入到采集线程队列
        if (widgetPara.sharedData) {
            VideoThread::videoThreads << videoThread;
        }

        //设置对应参数
        videoThread->setVideoMode(widgetPara.videoMode);
        videoThread->setProperty("scaleMode", widgetPara.scaleMode);
        videoThread->setProperty("sharedData", widgetPara.sharedData);
        VideoHelper::initVideoThread(videoThread, videoPara, encodePara);
    }

    //绑定信号槽
    connectThreadSignal();
    return true;
}

bool VideoWidget::open(const QString &mediaUrl) {
    //线程正常说明还在运行需要先停止
    if (videoThread) {
        this->stop();
        qApp->processEvents();
    }

    //重新初始化和播放
    videoPara.mediaUrl = mediaUrl;
    if (this->init()) {
        this->play();
        return true;
    } else {
        return false;
    }
}

void VideoWidget::play() {
    //如果是图片则只显示图片就行
    image = QImage(videoPara.mediaUrl);
    if (!image.isNull()) {
        videoThread->setVideoSize(QString("%1x%2").arg(image.width()).arg(image.height()));
        this->setImage(image);
        return;
    }

    //采用已经存在的采集线程/如果处于解码正常阶段则还要发几个信号通知
    if (videoThread->getIsOk()) {
        QMetaObject::invokeMethod(this, "receivePlayStart", Qt::DirectConnection, Q_ARG(int, 0));
        QMetaObject::invokeMethod(videoThread, "receiveSizeChanged", Qt::DirectConnection);
        QMetaObject::invokeMethod(videoThread, "receiveDuration", Qt::DirectConnection,
                                  Q_ARG(qint64, videoThread->getDuration()));
    }

    //已经在运行阶段还要发送已经开始的信号
    if (videoThread->isRunning()) {
        isRunning = true;
        QMetaObject::invokeMethod(this, "sig_receivePlayStart", Qt::DirectConnection, Q_ARG(int, 0));
    }

    //启动播放线程
    videoThread->play();
    this->update();
    //初始化标签信息和图形信息
    this->osdChanged();
    this->graphChanged();
}

void VideoWidget::stop() {
    //立即隐藏悬浮条
    bannerWidget->setVisible(false);
    //关闭的时候将遮罩控件移到最前
    hwndWidget->stackUnder(coverWidget);

    //处于运行状态才可以停止
    if (videoThread && videoThread->isRunning()) {
        //先判断当前线程的引用计数是0才需要真正停止
        if (videoThread->refCount > 0) {
            isRunning = false;
            //减少引用计数
            videoThread->refCount--;
            //执行停止信号
            QMetaObject::invokeMethod(this, "receivePlayFinsh", Qt::DirectConnection);
            QMetaObject::invokeMethod(this, "sig_receivePlayFinsh", Qt::DirectConnection);
            //取消信号关联
            disconnectThreadSignal();
        } else {
            //停止播放
            videoThread->stop();
            //取消信号关联
            disconnectThreadSignal();
            //从队列中移除
            VideoThread::videoThreads.removeOne(videoThread);
            //释放线程
            videoThread->debug("删除线程", "");
            videoThread->deleteLater();
        }
    }

    //复位标志位并将线程置空(没有这个的话可能会出现野指针的情况)
    isRunning = false;
    isShared = false;
    videoThread = NULL;
    videoPara.reset();
    encodePara.reset();
    AbstractVideoWidget::clear();
}

void VideoWidget::pause() {
    if (videoThread) {
        videoThread->pause();
    }
}

void VideoWidget::next() {
    if (videoThread) {
        videoThread->next();
    }
}

void VideoWidget::step(bool backward) {
    if (videoThread && !onlyAudio) {
        videoThread->step(backward);
    }
}

void VideoWidget::snap(const QString &snapName, bool preview) {
    if (videoThread && !onlyAudio) {
        this->preview = preview;
        if (!VideoThread::snapSource && widgetPara.videoMode == VideoMode_Painter && !image.isNull()) {
            //绘制方式直接取本地绘制图片对象
            videoThread->setSnapName(snapName);
            qDebug() << "888" << preview;
            image.save(videoThread->getSnapName(), "jpg");
            QMetaObject::invokeMethod(videoThread, "snapFinsh");
        } else {
            //这种方式直接用对应解码线程的抓图/是干净的原图
            videoThread->snap(snapName);
        }
    }
}

void VideoWidget::recordStart(const QString &fileName) {
    if (videoThread && videoThread->getIsOk()) {
        videoThread->recordStart(fileName);
    }
}

void VideoWidget::recordPause() {
    if (videoThread && videoThread->getIsOk()) {
        videoThread->recordPause();
    }
}

void VideoWidget::recordStop() {
    if (videoThread && videoThread->getIsOk()) {
        videoThread->recordStop();
    }
}
