#include "deviceutil.h"
#include "qthelper.h"
#include "dbdata.h"
#include "playwav.h"
#include "urlhelper.h"
#include "videomanage.h"
#include "videohelper.h"
#include "frmvideowidgetpreview.h"
#include "frmvideowidgetslider.h"
#include "deviceurl.h"
#include "appconfig.h"
#include "enableconfig.h"
#include "otherconfig.h"


void DeviceUtil::getDeviceInfo(QStringList &names, QStringList &addrs, QStringList &points)
{
    names.clear();
    addrs.clear();
    points.clear();

#if 0
    //随机产生数据
    for (int i = 1; i <= 20; ++i) {
        names << QString("摄像头%1").arg(i);
        addrs << QString("测试地址%1").arg(i);
        points << QString("121.%1,31.%2").arg(rand() % 1000000).arg(rand() % 1000000);
    }
#elif 0
    //写死数据
    names << "摄像头1" << "摄像头2" << "摄像头3" << "摄像头4" << "摄像头5";
    addrs << "安波路533弄1号楼" << "北新路8号" << "康桥镇康桥路1200号(御青路)" << "闵行区诸翟镇纪翟南路" << "浦东新区广兰路1080号(紫薇路口)";
    points << "121.534942,31.307706" << "121.572075,31.188825" << "121.57987,31.155795" << "121.292628,31.215278" << "121.626992,31.211056";
#else
    //从数据库加载
    names = DbData::IpcInfo_IpcName;
    //纠正经纬度坐标,在数据库中 可能是 | 分隔符,方便导出数据
    foreach (QString ipcPosition, DbData::IpcInfo_IpcPosition) {
        addrs << "摄像机默认位置";
        points << ipcPosition.replace("|", ",");
    }
#endif
}

bool DeviceUtil::checkOnline(const QString &url)
{
    //rtsp除外的认为永远存在/可以根据需要自行约定规则
    if (!url.startsWith("rtsp://")) {
        return true;
    }

    QString host = UrlHelper::getUrlHost(url);
    int port = UrlHelper::getUrlPort(url);
    int timeout = QtHelper::getRandValue(500, 1000);
    return QtHelper::hostLive(host, port, timeout);
}

void DeviceUtil::showVideo(const QString &url, const QString &flag, int recordTime, const QString &fileName)
{
    if (url.isEmpty()) {
        return;
    }

    //提前检测下是否在线
    bool online = false;
    if (OtherConfig::CheckOffline) {
        for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
            if (DbData::IpcInfo_RtspMain.at(i) == url || DbData::IpcInfo_RtspSub.at(i) == url) {
                online = DbData::IpcInfo_IpcOnline.at(i);
                break;
            }
        }
    } else {
        online = checkOnline(url);
    }

    if (online) {
        frmVideoWidgetPreview *video = new frmVideoWidgetPreview;
        video->open(url, flag, recordTime, fileName);
    } else {
        QtHelper::showMessageBoxError("设备不在线, 请重新检查!");
    }
}

void DeviceUtil::showAlarmVideo(const QString &url)
{
    //创建视频播放控件
    VideoWidget *videoWidget = new VideoWidget;
    DeviceUtil::initVideoWidget(videoWidget);

    //放入到视频播放窗体(带播放进度条)
    frmVideoWidgetSlider *widget = new frmVideoWidgetSlider(videoWidget);
    widget->resize(850, 600);
    widget->setWindowTitle("报警视频回放");
    widget->layout()->setContentsMargins(6, 6, 6, 6);
    widget->setWindowFlags(widget->windowFlags() | Qt::WindowStaysOnTopHint);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->show();

    //设置图片拉伸策略并播放
    videoWidget->setScaleMode(ScaleMode_Auto);
    videoWidget->open(url);
}

void DeviceUtil::showAlarmImage(const QString &url)
{
    QLabel *labImage = new QLabel;
    labImage->setWindowTitle("报警图片预览");
    labImage->setAlignment(Qt::AlignCenter);
    labImage->setWindowFlags(labImage->windowFlags() | Qt::WindowStaysOnTopHint);
    labImage->setAttribute(Qt::WA_DeleteOnClose);

    //自适应图片分辨率并限制最大分辨率
    QImage image(url);
    QSize size = image.size();
    if (size.width() > 1280 || size.height() > 720) {
        size.scale(1280, 720, Qt::KeepAspectRatio);
        image = image.scaled(size);
    }

    labImage->resize(image.size());
    labImage->setPixmap(QPixmap::fromImage(image));
    labImage->show();
}

void DeviceUtil::initVideoWidget(VideoWidget *videoWidget, int decodeType, int soundValue, bool soundMuted, bool bannerEnable, int readTimeout)
{
    //设置悬浮条哪些信息需要显示
    BannerInfo bannerInfo = videoWidget->getBannerInfo();
    bannerInfo.resolution = EnableConfig::EnableResolution;
    bannerInfo.frameRate = EnableConfig::EnableFrameRate;
    bannerInfo.mediaUrl = EnableConfig::EnableMediaUrl;
    bannerInfo.mediaName = EnableConfig::EnableMediaName;
    bannerInfo.formatName = EnableConfig::EnableFormatName;
    bannerInfo.videoCodecName = EnableConfig::EnableVideoCodecName;
    bannerInfo.audioCodecName = EnableConfig::EnableAudioCodecName;
    bannerInfo.realBitRate = EnableConfig::EnableRealBitRate;
    bannerInfo.hardware = EnableConfig::EnableHardware;
    bannerInfo.videoMode = EnableConfig::EnableVideoMode;
    videoWidget->setBannerInfo(bannerInfo);

    //设置窗体参数
    WidgetPara widgetPara = videoWidget->getWidgetPara();
    widgetPara.borderWidth = AppConfig::BorderWidth * 1;
    widgetPara.bgImage = QImage(QString("%1/config/bg_novideo.png").arg(QtHelper::appPath()));
    widgetPara.videoFlag = videoWidget->objectName();
    widgetPara.scaleMode = (ScaleMode)AppConfig::ScaleMode;
    widgetPara.videoMode = (VideoMode)AppConfig::VideoMode;
    widgetPara.bannerPosition = (BannerPosition)AppConfig::BannerPosition;
    widgetPara.soundValue = soundValue;
    widgetPara.soundMuted = soundMuted;
    widgetPara.sharedData = AppConfig::SharedData;
    widgetPara.bannerEnable = bannerEnable;
    //默认黑色背景(在等比例的时候填充空隙颜色)
    widgetPara.fillColor = QColor(0, 0, 0);

    //设置解码参数
    VideoPara videoPara = videoWidget->getVideoPara();
    QVariant videoCore = videoWidget->property("videoCore");
    videoPara.videoCore = videoCore.isNull() ? VideoHelper::getVideoCore() : (VideoCore)videoCore.toInt();
    videoPara.decodeType = (DecodeType)decodeType;
    videoPara.hardware = AppConfig::Hardware;
    videoPara.transport = AppConfig::Transport;
    videoPara.audioLevel = (AppConfig::VolumePosition > 0);
    videoPara.playAudio = AppConfig::PlayAudio;
    videoPara.readTimeout = readTimeout;
    videoPara.playRepeat = (readTimeout > 0);
    videoPara.connectTimeout = AppConfig::ConnectTimeout;

    //设置编码参数
    EncodePara encodePara = videoWidget->getEncodePara();
    encodePara.encodeVideo = (EncodeVideo)AppConfig::EncodeVideo;
    encodePara.encodeVideoRatio = AppConfig::EncodeVideoRatio;

    //初始化参数
    VideoHelper::initPara(widgetPara, videoPara, encodePara);
    videoWidget->setWidgetPara(widgetPara);
    videoWidget->setEncodePara(encodePara);
    videoWidget->setBannerBgAlpha(AppConfig::BannerBgAlpha);

    //如果视频控件正在播放则不设置解码参数
    if (!videoWidget->getIsRunning()) {
        videoWidget->setVideoPara(videoPara);
    } else {
        //有一些参数可以立即设置
        VideoThread *videoThread = videoWidget->getVideoThread();
        videoThread->setAudioLevel(videoPara.audioLevel);
        videoThread->setPlayAudio(videoPara.playAudio);
    }
}

void DeviceUtil::initVideoWidget2(VideoWidget *videoWidget)
{
    DeviceUtil::initVideoWidget(videoWidget, AppConfig::DecodeType, AppConfig::SoundValue, AppConfig::SoundMuted, true, AppConfig::ReadTimeout);
}

void DeviceUtil::playAlarm(const QString &soundName, bool stop)
{
    if (AppConfig::AlarmSoundCount > 0) {
        playSound(soundName, AppConfig::AlarmSoundCount, stop);
    }
}

void DeviceUtil::playSound(const QString &soundName, int playCount, bool stop)
{
    //名称为空或者设置无则不播放
    if (soundName.isEmpty() || soundName.contains("无")) {
        return;
    }

    //设置了先停止
    if (stop) {
        stopSound();
    }

    //重新设置文件全路径
    QString fileName = QString("%1/sound/%2").arg(QtHelper::appPath()).arg(soundName);
    PlayWav::play(fileName, playCount);
}

void DeviceUtil::stopSound()
{
    PlayWav::stop();
}

void DeviceUtil::preview()
{
    QStringList mediaUrls;
    foreach (QString url, DeviceUrl::MediaUrls) {
        if (!url.isEmpty()) {
            mediaUrls << url;
        }
    }

    DeviceUtil::preview(0, "127.0.0.1", 9999, mediaUrls);
}

#ifdef netpush
#include "videopushurl.h"
#include "netpushserver.h"
#include "videopushserver.h"
#endif
void DeviceUtil::preview(int serverType, const QString &hostName, int hostPort, const QStringList &mediaUrls)
{
#ifdef netpush
    static VideoPushServer *server = NULL;
    VideoPushServer::ServerType type = (VideoPushServer::ServerType)serverType;
    if (!server) {
        //读取对应的推拉流端口配置信息
        VideoPushUrl::initServerInfo(QtHelper::appPath() + "/config/video_push_port.txt");
        //启动流媒体服务
        QtHelper::start(QtHelper::appPath() + "/server", "mediamtx");
        QtHelper::start(QtHelper::appPath() + "/server", "MediaServer");

        //实例化推流服务
        server = new VideoPushServer(type, hostPort);
        //开始推流服务 mediamtx/ZLMediaKit webrtc/ws-flv
        QString pushUrl = QString("rtsp://%1:%2/stream").arg(hostName).arg(8554);
        server->start(pushUrl, "mediamtx", "webrtc");
    }

    if (mediaUrls.isEmpty()) {
        return;
    }

    //传入视频地址启动推流
    QStringList urls = VideoPushServer::startPush(type, hostName, hostPort, mediaUrls);
    if (!urls.isEmpty()) {
        bool liveplayer = false;
        QString httpUrl = QString("http://%1").arg(hostName);
        QString htmlName = QtHelper::appPath() + "/config/video_push_url.html";
        VideoPushUrl::preview(httpUrl, htmlName, 1, urls, liveplayer);
    }
#else
    QtHelper::showMessageBoxError("当前版本不含推流服务, 请联系作者购买推流版本!");
#endif
}
