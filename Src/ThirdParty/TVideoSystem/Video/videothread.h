#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "videohead.h"
#include "abstractvideothread.h"

class VideoThread : public AbstractVideoThread {
Q_OBJECT

public:
    //保存整个应用程序所有的解码线程集合/以便共享解码线程
    //引用计数(当前解码内核被几个窗体共用/当计数=1时在关闭的时候才需要真正关闭)
    int refCount;
    static QList<VideoThread *> videoThreads;

    static VideoThread *getVideoThread(const WidgetPara &widgetPara, const VideoPara &videoPara);

    explicit VideoThread(QObject *parent = 0);

protected:
    //打开后休息一下
    virtual void checkOpen();

    //线程执行内容
    virtual void run();

    //事件过滤器/识别尺寸变化
    bool eventFilter(QObject *watched, QEvent *event);

protected slots:

    //打开视频
    virtual bool openVideo();

    //关闭视频
    virtual void closeVideo();

    //重新播放(循环播放)
    virtual void replay();

protected:
    //音视频流索引(解码获取)
    int audioIndex;
    int videoIndex;

    //音视频轨道索引(用户设置)
    int audioTrack;
    int videoTrack;

    //音视频轨道集合
    QList<int> audioTracks;
    QList<int> videoTracks;

    //文件总时长
    qint64 duration;
    //当前播放位置
    qint64 position;

    //首次获取时长(mpv内核专用)
    qint64 firstDuration;
    //播放进度计数(mpv内核专用)
    qint64 playCount;

    //解析内核
    VideoCore videoCore;
    //媒体地址
    QString mediaUrl;
    //视频类型
    MediaType mediaType;
    //缓存分辨率
    QString bufferSize;

    //解码处理策略
    DecodeType decodeType;
    //硬件加速名称
    QString hardware;
    //通信协议(tcp/udp)
    QString transport;
    //缓存时间(默认500毫秒)
    int caching;

    //开启音频振幅
    bool audioLevel;
    //是否解码音频
    bool decodeAudio;
    //是否播放声音
    bool playAudio;
    //重复循环播放
    bool playRepeat;
    //是否逐帧播放
    bool playStep;
    //统计实时码率
    bool realBitRate;

    //加密秘钥字串
    QString encryptKey;
    //解密秘钥字串
    QString decryptKey;
    //音频输出设备
    QString audioDevice;

    //打开休息时间(最低1000/单位毫秒)
    int openSleepTime;
    //采集超时时间(0=不处理/单位毫秒)
    int readTimeout;
    //连接超时时间(0=不处理/单位毫秒)
    int connectTimeout;

    //解码线程对应的视频控件(可能为空)
    QWidget *videoWidget;

public:
    //统一格式打印信息
    void debug(const QString &head, const QString &msg);

    //事件回调那边主动设置获取到的值
    qint64 getDuration2() const;

    void setDuration2(qint64 duration);

    void setPosition2(qint64 position);

    //获取和设置解析内核
    VideoCore getVideoCore() const;

    void setVideoCore(const VideoCore &videoCore);

    //获取和设置媒体地址
    QString getMediaUrl() const;

    void setMediaUrl(const QString &mediaUrl);

    //获取是否是文件/有时长/可以是本地文件也可能是网络文件
    bool getIsFile() const;

    //获取是否是本地mp3
    bool getIsMp3() const;

    //校验是否只有音频
    void checkOnlyAudio();

    //获取是否只有音频
    bool getOnlyAudio() const;

    //校验视频类型
    void checkMediaType();

    void checkMediaType(bool web);

    //获取视频类型
    MediaType getMediaType() const;

    //获取和设置缓存分辨率
    QString getBufferSize() const;

    void setBufferSize(const QString &bufferSize);

    //获取和设置解码策略
    DecodeType getDecodeType() const;

    void setDecodeType(const DecodeType &decodeType);

    //获取和设置硬件加速名称
    QString getHardware() const;

    void setHardware(const QString &hardware);

    //获取和设置通信协议
    QString getTransport() const;

    void setTransport(const QString &transport);

    //获取和设置缓存时间
    int getCaching() const;

    void setCaching(int caching);

    //获取和设置音频振幅
    bool getAudioLevel() const;

    virtual void setAudioLevel(bool audioLevel);

    //获取和设置解码音频
    bool getDecodeAudio() const;

    void setDecodeAudio(bool decodeAudio);

    //获取和设置播放声音
    bool getPlayAudio() const;

    void setPlayAudio(bool playAudio);

    //获取和设置循环播放
    bool getPlayRepeat() const;

    void setPlayRepeat(bool playRepeat);

    //获取和设置统计码率
    bool getRealBitRate() const;

    virtual void setRealBitRate(bool realBitRate);

    //获取和设置加密秘钥字串
    QString getEncryptKey() const;

    void setEncryptKey(const QString &encryptKey);

    //获取和设置解密秘钥字串
    QString getDecryptKey() const;

    void setDecryptKey(const QString &decryptKey);

    //设置和清空秘钥字串
    void clearSecretKey();

    void setSecretKey(const QString &encryptKey, const QString &decryptKey);

    //获取和设置打开休息时间
    int getOpenSleepTime() const;

    void setOpenSleepTime(int openSleepTime);

    //获取和设置采集超时时间
    int getReadTimeout() const;

    void setReadTimeout(int readTimeout);

    //获取和设置连接超时时间
    int getConnectTimeout() const;

    void setConnectTimeout(int connectTimeout);

    // Detection
    void startDetect();

    void stopDetect();

public slots:

    //获取媒体信息
    virtual void readMediaInfo();

    //设置音视频轨道
    virtual void setAudioTrack(int audioTrack);

    virtual void setVideoTrack(int videoTrack);

    virtual void setTrack(int audioTrack, int videoTrack);

    virtual void setTrack();

    //获取和设置旋转角度
    virtual int getRotate();

    virtual void setRotate(int rotate);

    //设置视频宽高比例
    virtual void setAspect(double width, double height);

    //获取文件时长
    virtual qint64 getDuration();

    //获取和设置播放位置
    virtual qint64 getPosition();

    virtual void setPosition(qint64 position);

    //获取和设置播放速度
    virtual double getSpeed();

    virtual void setSpeed(double speed);

    //获取和设置音量大小
    virtual int getVolume();

    virtual void setVolume(int volume);

    //获取和设置静音状态
    virtual bool getMuted();

    virtual void setMuted(bool muted);

    //获取和设置逐帧播放
    virtual bool getPlayStep() const;

    virtual void setPlayStep(bool playStep);

    //获取和设置声音设备
    virtual QString getAudioDevice() const;

    virtual void setAudioDevice(const QString &audioDevice);

public slots:

    //调用线程对应窗体停止
    virtual void stop2();

    virtual void stop3();

    //截图完成
    virtual void snapFinsh();

    //打开完成
    virtual void openFinsh(bool open = false);

    //按帧播放
    virtual void step(bool backward = false);

    //开始录像完成
    virtual void recordStartFinsh();

    //停止录像完成
    virtual void recordStopFinsh();

signals:

    //循环播放
    void receiveReplay();

    //文件时长
    void receiveDuration(qint64 duration);

    //播放时长
    void receivePosition(qint64 position);

    //轨道索引
    void receiveTrack(const QList<int> &audioTracks, const QList<int> &videoTracks);
};

#endif // VIDEOTHREAD_H
