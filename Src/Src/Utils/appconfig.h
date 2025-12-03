#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QRect>


class AppConfig
{
public:
    static QString ConfigFile;      //配置文件文件路径及名称

    //基本配置参数1
    static int OpenGLType;          //opengl类型
    static bool Use96Dpi;           //开启后禁用分辨率缩放
    static int IndexStart;          //启动窗体索引
    static int WorkMode;            //软件工作模式
    static int NavStyle;            //导航按钮样式
    static QString StyleName;       //样式名称
    static QString LogoImage;       //软件图标
    static QString CompanyHttp;     //公司网址
    static QString CompanyTel;      //公司电话
    static QString Copyright;       //版权所有
    static QString TitleCn;         //软件中文名称
    static QString TitleEn;         //软件英文名称

    //基本配置参数2
    static int RecordsPerpage;      //表格分页时每页数据
    static int PageButtonCount;     //分页页码按钮个数
    static int MsgListCount;        //图文警情最大行数
    static int MsgTableCount;       //表格消息最大行数
    static int TimeHideCursor;      //程序多久未操作自动隐藏光标
    static int TimeAutoFull;        //程序多久未操作自动全屏界面
    static int AlarmSoundCount;     //报警声音次数/0-不启用
    static int WindowOpacity;       //停靠窗体透明度
    static int TipInterval;         //弹框显示的时间/单位秒/0-一直显示/除非手动关闭/10000-禁用

    //基本配置参数3
    static bool AutoRun;            //自动登录
    static bool AutoLogin;          //自动登录
    static bool AutoPwd;            //记住密码
    static bool SaveLog;            //日志输出
    static bool SaveRunTime;        //运行时间记录
    static bool RightInfo;          //右上角提示信息显示
    static bool SaveUrl;            //保存播放记录

    //基本配置参数4
    static bool FormMax;            //主界面最大化
    static bool FormFull;           //主界面全屏
    static QRect FormGeometry;      //最后窗体位置区域
    static QRect PollGeometry;      //轮询提示面板区域
    static QString SelectDirName;   //最后选择的目录    
    static QString DefaultChName;   //默认通道名称
    static QString DefaultIpcName;  //默认摄像机名称
    static QString DefaultNvrName;  //默认录像机名称

    //基本配置参数5
    static QString LastLoginer;     //最后登录用户
    static QString LastFormMain;    //最后的主界面
    static QString LastFormVideo;   //最后的视频回放界面
    static QString LastFormMap;     //最后的电子地图界面
    static QString LastFormData;    //最后的日志查询界面
    static QString LastFormConfig;  //最后的系统设置界面

    //可视化配置参数
    static bool FullScreen;         //全屏模式
    static bool VisibleIpcSearch;   //摄像机搜索界面显示
    static bool VisiblePollPlus;    //轮询点批量界面显示
    static bool VisiblePtzStep;     //云台控制步长下拉框

    //视频配置参数1
    static int ScaleMode;           //图片缩放模式
    static int VideoMode;           //视频显示模式
    static int BorderWidth;         //视频通道边框
    static int SoundValue;          //最后声音大小
    static bool SoundMuted;         //最后静音状态
    static bool SharedData;         //共享解码线程
    static bool DebugInfo;          //打印相关信息

    //视频配置参数2
    static QString Hardware;        //硬件加速名称
    static QString Transport;       //网络传输协议
    static int DecodeType;          //解码处理策略
    static int Caching;             //数据缓存大小
    static bool PlayAudio;          //解码播放声音
    static int EncodeVideo;         //编码处理策略
    static float EncodeVideoRatio;  //视频压缩比率
    static int ReadTimeout;         //采集超时时间
    static int ConnectTimeout;      //连接超时时间

    //视频配置参数3
    static int VideoCount;          //视频通道数
    static QString LayoutType;      //视频几画面
    static bool VideoMax;           //视频最大化
    static bool VideoDrag;          //通道交换
    static bool SaveVideo;          //保存视频
    static bool VideoSmart;         //人工智能
    static bool OnvifEvent;         //事件订阅
    static bool OnvifNtp;           //自动校时

    //视频配置参数4
    static int AlarmSaveTime;       //报警视频保存时间/单位秒
    static int OpenInterval;        //通道打开延时间隔/单位毫秒
    static int TableDataPolicy;     //表格数据编辑策略/0-单选按下编辑/1-多选双击编辑
    static int ChannelBgText;       //通道背景文字类型/0-通道1/1-通道01/2-设备名称
    static int BannerPosition;      //悬浮位置/0-顶部/1-底部/2-左侧/3-右侧
    static int BannerBgAlpha;       //悬浮条背景透明度/0-255
    static int VolumePosition;      //音柱位置/0-禁用/1-左侧/2-上侧/3-右侧/4-下侧/5-左右/6-悬浮
    static int VolumeBgAlpha;       //音柱条背景透明度/0-255
    static int VolumeSize;          //音柱尺寸/0-自动/其他值为最终尺寸

    //视频轮询配置参数
    static bool AutoPoll;           //启动后自动识别最后轮询状态
    static bool Polling;            //是否处于轮询状态
    static int PollInterval;        //轮询间隔/单位秒
    static int PollType;            //轮询画面
    static int PollRtsp;            //码流类型/0-主码流/1-子码流
    static QString GroupLast;       //最后选中轮询分组
    static QStringList GroupNames;  //轮询分组名称集合

    //批量生成配置参数
    static int PlusType;            //格式类型
    static int PlusNumber;          //添加数量
    static bool PlusNvr;            //通道递增(nvr才有)
    static QString PlusAddr;        //起始地址
    static QString PlusMain;        //主码流格式
    static QString PlusSub;         //子码流格式

    //远程回放配置参数
    static int NvrCompanyType;      //厂家类型
    static int NvrVideoType;        //视频类型
    static QString NvrUserName;     //用户名称
    static QString NvrUserPwd;      //用户密码
    static QString NvrDeviceIP;     //设备地址
    static bool NvrSyncPosition;    //同步播放进度
    static bool NvrAutoCh;          //通道递增
    static int NvrChannel;          //通道索引
    static int NvrStreamType;       //码流类型
    static QString NvrDateTimeFormat;//时间格式
    static QString NvrDateTimeStart;//开始时间
    static QString NvrDateTimeEnd;  //结束时间

    //设备搜索配置参数
    static QString SearchUserName;  //用户名称
    static QString SearchUserPwd;   //用户密码
    static QString SearchLocalIP;   //指定网卡
    static QString SearchDeviceIP;  //指定设备
    static QString SearchFilter;    //条件过滤
    static QString SearchFilters;   //条件集合
    static int SearchInterval;      //搜索间隔(单位毫秒)
    static int SearchTimeout;       //超时时间(单位毫秒)
    static bool SearchClear;        //自动清空(每次都重新搜索)

    //本地数据库配置参数
    static bool LocalAutoInfo;      //自动填入端口和用户信息
    static int LocalConnMode;       //本地数据库连接方式
    static QString LocalDbType;     //本地数据库主机类型
    static QString LocalDbName;     //本地数据库数据库名
    static QString LocalHostName;   //本地数据库主机地址
    static int LocalHostPort;       //本地数据库通信端口
    static QString LocalUserName;   //本地数据库用户名称
    static QString LocalUserPwd;    //本地数据库用户密码

    //各种颜色配置参数
    static QString ColorTextNormal; //常规文字颜色
    static QString ColorTextWarn;   //警告文字颜色
    static QString ColorTextAlarm;  //报警文字颜色
    static QString ColorIconAlarm;  //报警图标颜色
    static QString ColorMsgWait;    //待处理颜色
    static QString ColorMsgSolved;  //已处理颜色

    //读写校验配置文件
    static void readConfig();       //读取配置文件
    static void checkConfig();      //检验配置参数
    static void writeConfig();      //写入配置文件

    //读写布局配置文件
    static QByteArray readLayout(const QString &file);
    static void writeLayout(const QString &file, const QByteArray &data);
};

#endif // APPCONFIG_H
