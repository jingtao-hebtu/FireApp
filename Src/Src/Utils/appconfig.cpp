#include "appconfig.h"
#include "qthelper.h"

QString AppConfig::ConfigFile = "config.ini";

int AppConfig::OpenGLType = 0;
bool AppConfig::Use96Dpi = false;
int AppConfig::IndexStart = 0;
int AppConfig::WorkMode = 0;
int AppConfig::NavStyle = 3;
QString AppConfig::StyleName = ":/qss/blackvideo.css";
QString AppConfig::LogoImage = ":/logo.svg";
QString AppConfig::CompanyHttp = "https://qtchina.blog.csdn.net/";
QString AppConfig::CompanyTel = "021-12345678";
QString AppConfig::Copyright = QString::fromUtf8("物联网技术研究中心");
QString AppConfig::TitleCn = QString::fromUtf8("安防视频监控管理平台");
QString AppConfig::TitleEn = "Security video management platform";

int AppConfig::RecordsPerpage = 28;
int AppConfig::PageButtonCount = 5;
int AppConfig::MsgListCount = 20;
int AppConfig::MsgTableCount = 0;
int AppConfig::TimeHideCursor = 0;
int AppConfig::TimeAutoFull = 0;
int AppConfig::AlarmSoundCount = 0;
int AppConfig::WindowOpacity = 70;
int AppConfig::TipInterval = 3;

bool AppConfig::AutoRun = false;
bool AppConfig::AutoLogin = false;
bool AppConfig::AutoPwd = false;
bool AppConfig::SaveLog = false;
bool AppConfig::SaveRunTime = false;
bool AppConfig::RightInfo = true;
bool AppConfig::SaveUrl = true;

bool AppConfig::FormMax = false;
bool AppConfig::FormFull = false;
QRect AppConfig::FormGeometry = QRect(0, 0, 1200, 850);
QRect AppConfig::PollGeometry = QRect(0, 0, 450, 60);
QString AppConfig::SelectDirName = ".";
QString AppConfig::DefaultChName = QString::fromUtf8("通道");
QString AppConfig::DefaultIpcName = QString::fromUtf8("摄像机#1");
QString AppConfig::DefaultNvrName = QString::fromUtf8("录像机#1");

QString AppConfig::LastLoginer = "admin";
QString AppConfig::LastFormMain = "btnView";
QString AppConfig::LastFormVideo = "btnVideoPlayLocal";
QString AppConfig::LastFormMap = "btnMapDevice";
QString AppConfig::LastFormData = "btnDataUser";
QString AppConfig::LastFormConfig = "btnConfigSystem";

bool AppConfig::FullScreen = false;
bool AppConfig::VisibleIpcSearch = true;
bool AppConfig::VisiblePollPlus = false;
bool AppConfig::VisiblePtzStep = false;

int AppConfig::ScaleMode = 2;
int AppConfig::VideoMode = 0;
int AppConfig::BorderWidth = 2;
int AppConfig::SoundValue = 100;
bool AppConfig::SoundMuted = true;
bool AppConfig::SharedData = true;
bool AppConfig::DebugInfo = true;

QString AppConfig::Hardware = "none";
QString AppConfig::Transport = "tcp";
int AppConfig::DecodeType = 0;
int AppConfig::Caching = 8192000;
bool AppConfig::PlayAudio = true;
int AppConfig::EncodeVideo = 0;
float AppConfig::EncodeVideoRatio = 1;
int AppConfig::ReadTimeout = 10000;
int AppConfig::ConnectTimeout = 0;

int AppConfig::VideoCount = 64;
QString AppConfig::LayoutType = "1_4";
bool AppConfig::VideoMax = false;
bool AppConfig::VideoDrag = true;
bool AppConfig::SaveVideo = false;
bool AppConfig::VideoSmart = false;
bool AppConfig::OnvifEvent = false;
bool AppConfig::OnvifNtp = true;

int AppConfig::AlarmSaveTime = 30;
int AppConfig::OpenInterval = 100;
int AppConfig::TableDataPolicy = 0;
int AppConfig::ChannelBgText = 1;
int AppConfig::BannerPosition = 0;
int AppConfig::BannerBgAlpha = 200;
int AppConfig::VolumePosition = 0;
int AppConfig::VolumeBgAlpha = 0;
int AppConfig::VolumeSize = 20;

bool AppConfig::AutoPoll = false;
bool AppConfig::Polling = false;
int AppConfig::PollInterval = 30;
int AppConfig::PollType = 4;
int AppConfig::PollRtsp = 1;
QString AppConfig::GroupLast = QString::fromUtf8("默认分组");
QStringList AppConfig::GroupNames = QString::fromUtf8("默认分组").split("|");

int AppConfig::PlusType = 1;
int AppConfig::PlusNumber = 16;
bool AppConfig::PlusNvr = false;
QString AppConfig::PlusAddr = "192.168.1.10";
QString AppConfig::PlusMain = "rtsp://admin:12345@[Addr]:554/Streaming/Channels/101?transportmode=unicast";
QString AppConfig::PlusSub = "rtsp://admin:12345@[Addr]:554/Streaming/Channels/102?transportmode=unicast";

int AppConfig::NvrCompanyType = 1;
int AppConfig::NvrVideoType = 1;
QString AppConfig::NvrUserName = "admin";
QString AppConfig::NvrUserPwd = "admin";
QString AppConfig::NvrDeviceIP = "192.168.0.64";
bool AppConfig::NvrSyncPosition = false;
bool AppConfig::NvrAutoCh = false;
int AppConfig::NvrChannel = 0;
int AppConfig::NvrStreamType = 0;
QString AppConfig::NvrDateTimeFormat = "yyyy-MM-dd HH:mm:00";
QString AppConfig::NvrDateTimeStart = QDateTime::currentDateTime().addDays(-1).toString(AppConfig::NvrDateTimeFormat);
QString AppConfig::NvrDateTimeEnd = QDateTime::currentDateTime().addDays(0).toString(AppConfig::NvrDateTimeFormat);

QString AppConfig::SearchUserName = "admin";
QString AppConfig::SearchUserPwd = "Admin123456";
QString AppConfig::SearchLocalIP = "192.168.1.2";
QString AppConfig::SearchDeviceIP = "192.168.1.128";
QString AppConfig::SearchFilter = "none";
QString AppConfig::SearchFilters = "none|:80|:8000|:2000|.1.";
int AppConfig::SearchInterval = 300;
int AppConfig::SearchTimeout = 1500;
bool AppConfig::SearchClear = false;

bool AppConfig::LocalAutoInfo = true;
int AppConfig::LocalConnMode = 0;
QString AppConfig::LocalDbType = "Sqlite";
QString AppConfig::LocalDbName = "video_system";
QString AppConfig::LocalHostName = "127.0.0.1";
int AppConfig::LocalHostPort = 3306;
QString AppConfig::LocalUserName = "root";
QString AppConfig::LocalUserPwd = "root";

QString AppConfig::ColorTextNormal = "white";
QString AppConfig::ColorTextWarn = "gold";
QString AppConfig::ColorTextAlarm = "darkorange";
QString AppConfig::ColorIconAlarm = "tomato";
QString AppConfig::ColorMsgWait = "goldenrod";
QString AppConfig::ColorMsgSolved = "skyblue";

void AppConfig::readConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    OpenGLType = set.value("OpenGLType", OpenGLType).toInt();
    Use96Dpi = set.value("Use96Dpi", Use96Dpi).toBool();
    //IndexStart = set.value("IndexStart", IndexStart).toInt();
    WorkMode = set.value("WorkMode", WorkMode).toInt();
    NavStyle = set.value("NavStyle", NavStyle).toInt();
    StyleName = set.value("StyleName", StyleName).toString();
    LogoImage = set.value("LogoImage", LogoImage).toString();
    CompanyHttp = set.value("CompanyHttp", CompanyHttp).toString();
    CompanyTel = set.value("CompanyTel", CompanyTel).toString();
    Copyright = set.value("Copyright", Copyright).toString();
    TitleCn = set.value("TitleCn", TitleCn).toString();
    TitleEn = set.value("TitleEn", TitleEn).toString();
    set.endGroup();

    set.beginGroup("AppConfig2");
    RecordsPerpage = set.value("RecordsPerpage", RecordsPerpage).toInt();
    PageButtonCount = set.value("PageButtonCount", PageButtonCount).toInt();
    MsgListCount = set.value("MsgListCount", MsgListCount).toInt();
    MsgTableCount = set.value("MsgTableCount", MsgTableCount).toInt();
    TimeHideCursor = set.value("TimeHideCursor", TimeHideCursor).toInt();
    TimeAutoFull = set.value("TimeAutoFull", TimeAutoFull).toInt();
    AlarmSoundCount = set.value("AlarmSoundCount", AlarmSoundCount).toInt();
    WindowOpacity = set.value("WindowOpacity", WindowOpacity).toInt();
    TipInterval = set.value("TipInterval", TipInterval).toInt();
    set.endGroup();

    set.beginGroup("AppConfig3");
    AutoRun = set.value("AutoRun", AutoRun).toBool();
    AutoLogin = set.value("AutoLogin", AutoLogin).toBool();
    AutoPwd = set.value("AutoPwd", AutoPwd).toBool();
    SaveLog = set.value("SaveLog", SaveLog).toBool();
    SaveRunTime = set.value("SaveRunTime", SaveRunTime).toBool();
    RightInfo = set.value("RightInfo", RightInfo).toBool();
    SaveUrl = set.value("SaveUrl", SaveUrl).toBool();
    set.endGroup();

    set.beginGroup("AppConfig4");
    FormMax = set.value("FormMax", FormMax).toBool();
    FormFull = set.value("FormFull", FormFull).toBool();
    FormGeometry = set.value("FormGeometry", FormGeometry).toRect();
    PollGeometry = set.value("PollGeometry", PollGeometry).toRect();
    SelectDirName = set.value("SelectDirName", SelectDirName).toString();    
    DefaultChName = set.value("DefaultChName", DefaultChName).toString();
    DefaultIpcName = set.value("DefaultIpcName", DefaultIpcName).toString();
    DefaultNvrName = set.value("DefaultNvrName", DefaultNvrName).toString();
    set.endGroup();

    set.beginGroup("AppConfig5");
    LastLoginer = set.value("LastLoginer", LastLoginer).toString();
    LastFormMain = set.value("LastFormMain", LastFormMain).toString();
    LastFormVideo = set.value("LastFormVideo", LastFormVideo).toString();
    LastFormMap = set.value("LastFormMap", LastFormMap).toString();
    LastFormData = set.value("LastFormData", LastFormData).toString();
    LastFormConfig = set.value("LastFormConfig", LastFormConfig).toString();
    set.endGroup();

    set.beginGroup("VisibleConfig");
    FullScreen = set.value("FullScreen", FullScreen).toBool();
    VisibleIpcSearch = set.value("VisibleIpcSearch", VisibleIpcSearch).toBool();
    VisiblePollPlus = set.value("VisiblePollPlus", VisiblePollPlus).toBool();
    VisiblePtzStep = set.value("VisiblePtzStep", VisiblePtzStep).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig1");
    ScaleMode = set.value("ScaleMode", ScaleMode).toInt();
    VideoMode = set.value("VideoMode", VideoMode).toInt();
    BorderWidth = set.value("BorderWidth", BorderWidth).toInt();
    SoundValue = set.value("SoundValue", SoundValue).toInt();
    SoundMuted = set.value("SoundMuted", SoundMuted).toBool();
    SharedData = set.value("SharedData", SharedData).toBool();
    DebugInfo = set.value("DebugInfo", DebugInfo).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig2");
    Hardware = set.value("Hardware", Hardware).toString();
    Transport = set.value("Transport", Transport).toString();
    DecodeType = set.value("DecodeType", DecodeType).toInt();
    Caching = set.value("Caching", Caching).toInt();
    PlayAudio = set.value("PlayAudio", PlayAudio).toBool();

    EncodeVideo = set.value("EncodeVideo", EncodeVideo).toInt();
    EncodeVideoRatio = set.value("EncodeVideoRatio", EncodeVideoRatio).toFloat();
    ReadTimeout = set.value("ReadTimeout", ReadTimeout).toInt();
    ConnectTimeout = set.value("ConnectTimeout", ConnectTimeout).toInt();
    set.endGroup();

    set.beginGroup("VideoConfig3");
    VideoCount = set.value("VideoCount", VideoCount).toInt();
    LayoutType = set.value("LayoutType", LayoutType).toString();
    VideoMax = set.value("VideoMax", VideoMax).toBool();
    VideoDrag = set.value("VideoDrag", VideoDrag).toBool();
    SaveVideo = set.value("SaveVideo", SaveVideo).toBool();
    VideoSmart = set.value("VideoSmart", VideoSmart).toBool();
    OnvifEvent = set.value("OnvifEvent", OnvifEvent).toBool();
    OnvifNtp = set.value("OnvifNtp", OnvifNtp).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig4");
    AlarmSaveTime = set.value("AlarmSaveTime", AlarmSaveTime).toInt();
    OpenInterval = set.value("OpenInterval", OpenInterval).toInt();
    TableDataPolicy = set.value("TableDataPolicy", TableDataPolicy).toInt();
    ChannelBgText = set.value("ChannelBgText", ChannelBgText).toInt();
    BannerPosition = set.value("BannerPosition", BannerPosition).toInt();
    BannerBgAlpha = set.value("BannerBgAlpha", BannerBgAlpha).toInt();
    VolumePosition = set.value("VolumePosition", VolumePosition).toInt();
    VolumeBgAlpha = set.value("VolumeBgAlpha", VolumeBgAlpha).toInt();
    VolumeSize = set.value("VolumeSize", VolumeSize).toInt();
    set.endGroup();

    set.beginGroup("PollConfig");
    AutoPoll = set.value("AutoPoll", AutoPoll).toBool();
    Polling = set.value("Polling", Polling).toBool();
    PollInterval = set.value("PollInterval", PollInterval).toInt();
    PollType = set.value("PollType", PollType).toInt();
    PollRtsp = set.value("PollRtsp", PollRtsp).toInt();
    GroupLast = set.value("GroupLast", GroupLast).toString();
    GroupNames = set.value("GroupNames", GroupNames).toStringList();
    set.endGroup();

    set.beginGroup("PlusConfig");
    PlusType = set.value("PlusType", PlusType).toInt();
    PlusNumber = set.value("PlusNumber", PlusNumber).toInt();
    PlusNvr = set.value("PlusNvr", PlusNvr).toBool();
    PlusAddr = set.value("PlusAddr", PlusAddr).toString();
    PlusMain = set.value("PlusMain", PlusMain).toString();
    PlusSub = set.value("PlusSub", PlusSub).toString();
    set.endGroup();

    set.beginGroup("NvrConfig");
    NvrCompanyType = set.value("NvrCompanyType", NvrCompanyType).toInt();
    NvrVideoType = set.value("NvrVideoType", NvrVideoType).toInt();
    NvrUserName = set.value("NvrUserName", NvrUserName).toString();
    NvrUserPwd = set.value("NvrUserPwd", NvrUserPwd).toString();
    NvrDeviceIP = set.value("NvrDeviceIP", NvrDeviceIP).toString();
    NvrSyncPosition = set.value("NvrSyncPosition", NvrSyncPosition).toBool();
    NvrAutoCh = set.value("NvrAutoCh", NvrAutoCh).toBool();
    NvrChannel = set.value("NvrChannel", NvrChannel).toInt();
    NvrStreamType = set.value("NvrStreamType", NvrStreamType).toInt();
    NvrDateTimeFormat = set.value("NvrDateTimeFormat", NvrDateTimeFormat).toString();
    NvrDateTimeStart = set.value("NvrDateTimeStart", NvrDateTimeStart).toString();
    NvrDateTimeEnd = set.value("NvrDateTimeEnd", NvrDateTimeEnd).toString();
    set.endGroup();

    set.beginGroup("SearchConfig");
    SearchUserName = set.value("SearchUserName", SearchUserName).toString();
    SearchUserPwd = set.value("SearchUserPwd", SearchUserPwd).toString();
    SearchLocalIP = set.value("SearchLocalIP", SearchLocalIP).toString();
    SearchDeviceIP = set.value("SearchDeviceIP", SearchDeviceIP).toString();
    SearchFilter = set.value("SearchFilter", SearchFilter).toString();
    SearchFilters = set.value("SearchFilters", SearchFilters).toString();
    SearchInterval = set.value("SearchInterval", SearchInterval).toInt();
    SearchTimeout = set.value("SearchTimeout", SearchTimeout).toInt();
    SearchClear = set.value("SearchClear", SearchClear).toBool();
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    LocalAutoInfo = set.value("LocalAutoInfo", LocalAutoInfo).toBool();
    LocalConnMode = set.value("LocalConnMode", LocalConnMode).toInt();
    LocalDbType = set.value("LocalDbType", LocalDbType).toString();
    LocalDbName = set.value("LocalDbName", LocalDbName).toString();
    LocalHostName = set.value("LocalHostName", LocalHostName).toString();
    LocalHostPort = set.value("LocalHostPort", LocalHostPort).toInt();
    LocalUserName = set.value("LocalUserName", LocalUserName).toString();
    LocalUserPwd = set.value("LocalUserPwd", LocalUserPwd).toString();
    set.endGroup();

    set.beginGroup("ColorConfig");
    ColorTextNormal = set.value("ColorTextNormal", ColorTextNormal).toString();
    ColorTextWarn = set.value("ColorTextWarn", ColorTextWarn).toString();
    ColorTextAlarm = set.value("ColorTextAlarm", ColorTextAlarm).toString();
    ColorIconAlarm = set.value("ColorIconAlarm", ColorIconAlarm).toString();
    ColorMsgWait = set.value("ColorMsgWait", ColorMsgWait).toString();
    ColorMsgSolved = set.value("ColorMsgSolved", ColorMsgSolved).toString();
    set.endGroup();

    AppConfig::checkConfig();
}

void AppConfig::checkConfig()
{
    //过滤通道数量
    AppConfig::VideoCount = (AppConfig::VideoCount < 16 ? 16 : AppConfig::VideoCount);

    //配置文件不存在或者有问题则重新生成
    if (!QtHelper::checkIniFile(ConfigFile)) {
        //首次启动将窗体位置移到中间
        QRect deskRect = QtHelper::checkCenterRect(FormGeometry);
        //小分辨率重新调整
        if (QtHelper::deskWidth() <= 1366) {
            FormMax = true;
            FormGeometry = deskRect;
        }

        //重新设置默认选择目录
        AppConfig::SelectDirName = QtHelper::appPath() + "/data";
        AppConfig::writeConfig();
        return;
    }

    //加密过的密码在这里解密
    NvrUserPwd = QtHelper::getXorEncryptDecrypt(NvrUserPwd, 100);
    SearchUserPwd = QtHelper::getXorEncryptDecrypt(SearchUserPwd, 100);
    LocalUserPwd = QtHelper::getXorEncryptDecrypt(LocalUserPwd, 100);
}

void AppConfig::writeConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    set.setValue("OpenGLType", OpenGLType);
    set.setValue("Use96Dpi", Use96Dpi);
    set.setValue("IndexStart", IndexStart);
    set.setValue("WorkMode", WorkMode);
    set.setValue("NavStyle", NavStyle);
    set.setValue("StyleName", StyleName);
    set.setValue("LogoImage", LogoImage);
    set.setValue("CompanyHttp", CompanyHttp);
    set.setValue("CompanyTel", CompanyTel);
    set.setValue("Copyright", Copyright);
    set.setValue("TitleCn", TitleCn);
    set.setValue("TitleEn", TitleEn);
    set.endGroup();

    set.beginGroup("AppConfig2");
    set.setValue("RecordsPerpage", RecordsPerpage);
    set.setValue("PageButtonCount", PageButtonCount);
    set.setValue("MsgListCount", MsgListCount);
    set.setValue("MsgTableCount", MsgTableCount);
    set.setValue("TimeHideCursor", TimeHideCursor);
    set.setValue("TimeAutoFull", TimeAutoFull);
    set.setValue("AlarmSoundCount", AlarmSoundCount);
    set.setValue("WindowOpacity", WindowOpacity);
    set.setValue("TipInterval", TipInterval);
    set.endGroup();

    set.beginGroup("AppConfig3");
    set.setValue("AutoRun", AutoRun);
    set.setValue("AutoLogin", AutoLogin);
    set.setValue("AutoPwd", AutoPwd);
    set.setValue("SaveLog", SaveLog);
    set.setValue("SaveRunTime", SaveRunTime);
    set.setValue("RightInfo", RightInfo);
    set.setValue("SaveUrl", SaveUrl);
    set.endGroup();

    set.beginGroup("AppConfig4");
    set.setValue("FormMax", FormMax);
    set.setValue("FormFull", FormFull);
    set.setValue("FormGeometry", FormGeometry);
    set.setValue("PollGeometry", PollGeometry);
    set.setValue("SelectDirName", SelectDirName);    
    set.setValue("DefaultChName", DefaultChName);
    set.setValue("DefaultIpcName", DefaultIpcName);
    set.setValue("DefaultNvrName", DefaultNvrName);
    set.endGroup();

    set.beginGroup("AppConfig5");
    set.setValue("LastLoginer", LastLoginer);
    set.setValue("LastFormMain", LastFormMain);
    set.setValue("LastFormVideo", LastFormVideo);
    set.setValue("LastFormMap", LastFormMap);
    set.setValue("LastFormData", LastFormData);
    set.setValue("LastFormConfig", LastFormConfig);
    set.endGroup();

    set.beginGroup("VisibleConfig");
    set.setValue("FullScreen", FullScreen);
    set.setValue("VisibleIpcSearch", VisibleIpcSearch);
    set.setValue("VisiblePollPlus", VisiblePollPlus);
    set.setValue("VisiblePtzStep", VisiblePtzStep);
    set.endGroup();

    set.beginGroup("VideoConfig1");
    set.setValue("ScaleMode", ScaleMode);
    set.setValue("VideoMode", VideoMode);
    set.setValue("BorderWidth", BorderWidth);
    set.setValue("SoundValue", SoundValue);
    set.setValue("SoundMuted", SoundMuted);
    set.setValue("SharedData", SharedData);
    set.setValue("DebugInfo", DebugInfo);
    set.endGroup();

    set.beginGroup("VideoConfig2");
    set.setValue("Hardware", Hardware);
    set.setValue("Transport", Transport);
    set.setValue("DecodeType", DecodeType);
    set.setValue("Caching", Caching);
    set.setValue("PlayAudio", PlayAudio);
    set.setValue("EncodeVideo", EncodeVideo);
    set.setValue("EncodeVideoRatio", (double)EncodeVideoRatio);
    set.setValue("ReadTimeout", ReadTimeout);
    set.setValue("ConnectTimeout", ConnectTimeout);
    set.endGroup();

    set.beginGroup("VideoConfig3");
    set.setValue("VideoCount", VideoCount);
    set.setValue("LayoutType", LayoutType);
    set.setValue("VideoMax", VideoMax);
    set.setValue("VideoDrag", VideoDrag);
    set.setValue("SaveVideo", SaveVideo);
    set.setValue("VideoSmart", VideoSmart);
    set.setValue("OnvifEvent", OnvifEvent);
    set.setValue("OnvifNtp", OnvifNtp);
    set.endGroup();

    set.beginGroup("VideoConfig4");
    set.setValue("AlarmSaveTime", AlarmSaveTime);
    set.setValue("OpenInterval", OpenInterval);
    set.setValue("TableDataPolicy", TableDataPolicy);
    set.setValue("ChannelBgText", ChannelBgText);
    set.setValue("BannerPosition", BannerPosition);
    set.setValue("BannerBgAlpha", BannerBgAlpha);
    set.setValue("VolumePosition", VolumePosition);
    set.setValue("VolumeBgAlpha", VolumeBgAlpha);
    set.setValue("VolumeSize", VolumeSize);
    set.endGroup();

    set.beginGroup("PollConfig");
    set.setValue("AutoPoll", AutoPoll);
    set.setValue("Polling", Polling);
    set.setValue("PollInterval", PollInterval);
    set.setValue("PollType", PollType);
    set.setValue("PollRtsp", PollRtsp);
    set.setValue("GroupLast", GroupLast);
    set.setValue("GroupNames", GroupNames);
    set.endGroup();

    set.beginGroup("PlusConfig");
    set.setValue("PlusType", PlusType);
    set.setValue("PlusNumber", PlusNumber);
    set.setValue("PlusNvr", PlusNvr);
    set.setValue("PlusAddr", PlusAddr);
    set.setValue("PlusMain", PlusMain);
    set.setValue("PlusSub", PlusSub);
    set.endGroup();

    set.beginGroup("NvrConfig");
    set.setValue("NvrCompanyType", NvrCompanyType);
    set.setValue("NvrVideoType", NvrVideoType);
    set.setValue("NvrUserName", NvrUserName);
    set.setValue("NvrUserPwd", QtHelper::getXorEncryptDecrypt(NvrUserPwd, 100));
    set.setValue("NvrDeviceIP", NvrDeviceIP);
    set.setValue("NvrSyncPosition", NvrSyncPosition);
    set.setValue("NvrAutoCh", NvrAutoCh);
    set.setValue("NvrChannel", NvrChannel);
    set.setValue("NvrStreamType", NvrStreamType);
    set.setValue("NvrDateTimeFormat", NvrDateTimeFormat);
    set.setValue("NvrDateTimeStart", NvrDateTimeStart);
    set.setValue("NvrDateTimeEnd", NvrDateTimeEnd);
    set.endGroup();

    set.beginGroup("SearchConfig");
    set.setValue("SearchUserName", SearchUserName);
    set.setValue("SearchUserPwd", QtHelper::getXorEncryptDecrypt(SearchUserPwd, 100));
    set.setValue("SearchLocalIP", SearchLocalIP);
    set.setValue("SearchDeviceIP", SearchDeviceIP);
    set.setValue("SearchFilter", SearchFilter);
    set.setValue("SearchFilters", SearchFilters);
    set.setValue("SearchInterval", SearchInterval);
    set.setValue("SearchTimeout", SearchTimeout);
    set.setValue("SearchClear", SearchClear);
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    set.setValue("LocalAutoInfo", LocalAutoInfo);
    set.setValue("LocalConnMode", LocalConnMode);
    set.setValue("LocalDbType", LocalDbType);
    set.setValue("LocalDbName", LocalDbName);
    set.setValue("LocalHostName", LocalHostName);
    set.setValue("LocalHostPort", LocalHostPort);
    set.setValue("LocalUserName", LocalUserName);
    set.setValue("LocalUserPwd", QtHelper::getXorEncryptDecrypt(LocalUserPwd, 100));
    set.endGroup();

    set.beginGroup("ColorConfig");
    set.setValue("ColorTextNormal", ColorTextNormal);
    set.setValue("ColorTextWarn", ColorTextWarn);
    set.setValue("ColorTextAlarm", ColorTextAlarm);
    set.setValue("ColorIconAlarm", ColorIconAlarm);
    set.setValue("ColorMsgWait", ColorMsgWait);
    set.setValue("ColorMsgSolved", ColorMsgSolved);
    set.endGroup();
}

QByteArray AppConfig::readLayout(const QString &file)
{
    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("MainWindow");
    QByteArray data = set.value("State").toByteArray();
    set.endGroup();
    return data;
}

void AppConfig::writeLayout(const QString &file, const QByteArray &data)
{
    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("MainWindow");
    set.setValue("State", data);
    set.endGroup();
}
