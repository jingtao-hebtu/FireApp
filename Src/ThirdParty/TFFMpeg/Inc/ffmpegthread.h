#ifndef FFMPEGTHREAD_H
#define FFMPEGTHREAD_H

#include "ffmpeginclude.h"
#include "ffmpegstruct.h"
#include "videothread.h"

class FFmpegSync;

class FFmpegAvio;

class AudioPlayer;

class FFmpegThread : public VideoThread {
Q_OBJECT

public:
    //解码同步线程定义成友元类(这样可以直接使用主解码线程的变量)
    friend class FFmpegSync;

    explicit FFmpegThread(QObject *parent = 0);

    ~FFmpegThread();

protected:
    void run();

    void checkData();

    bool checkRun();

private:
    //正在切换进度
    volatile bool changePosition;
    //开启推流预览(推流用)
    volatile bool pushPreview;
    //清空缓存标志位(推流用)
    volatile bool needClear;
    //是否收到关键帧(推流用)
    volatile bool keyFrame;

    //转发数据包或数据帧
    bool sendPacket;
    bool sendFrame;

    //播放速度
    double speed;
    //是否已经尝试打开过
    bool tryOpen;
    //是否已经尝试读取过
    bool tryRead;

    //统计多久一个关键帧
    qint64 keyPacketIndex;
    qint64 keyPacketCount;

    //实时码率(帧数总包大小)
    qint64 realPacketSize;
    qint64 realPacketCount;

    //视频首个包的 pts start_time
    qint64 videoFirstPts;
    //音频首个包的 pts start_time
    qint64 audioFirstPts;
    //解码开始时间
    qint64 startTime;

    //待解码数据包
    AVPacket *packet;

    //视频帧对象
    AVFrame *videoFrame;
    //视频帧对象(中转用)
    AVFrame *tempFrame;
    //视频帧对象(转yuv420)
    AVFrame *yuvFrame;
    //视频帧对象(转rgb)
    AVFrame *imageFrame;
    //视频帧数据(转rgb)
    quint8 *imageData;

    //音频帧对象
    AVFrame *audioFrame;
    //音频帧对象(转pcm)
    AVFrame *pcmFrame;
    //音频转换格式
    AVSampleFormat pcmSampleFormat;
    //音频转换采样率
    int pcmSampleRate;
    //音频转换通道数
    int pcmChannels;

    //视频图像转换上下文(转yuv420)
    SwsContext *yuvSwsCtx;
    //视频图像转换上下文(转rgb)
    SwsContext *imageSwsCtx;
    //音频数据转换上下文(转pcm)
    SwrContext *pcmSwrCtx;

    //参数字典
    AVDictionary *options;
    //输入格式
    AVInputFormatx *ifmt;
    //描述一个多媒体文件的构成及其基本信息
    AVFormatContext *formatCtx;

    //视频解码器
    AVCodecx *videoCodec;
    //音频解码器
    AVCodecx *audioCodec;

    //视频解码器上下文
    AVCodecContext *videoCodecCtx;
    //音频解码器上下文
    AVCodecContext *audioCodecCtx;

    //启用音视频同步
    bool useSync;
    //视频解码同步线程
    FFmpegSync *videoSync;
    //音频解码同步线程
    FFmpegSync *audioSync;

    //恢复暂停定时器
    QTimer *timerPause;
    //切换进度定时器
    QTimer *timerPosition;

    //自定义流数据类
    FFmpegAvio *avio;
    //音频播放类
    AudioPlayer *audioPlayer;

    //裁剪滤镜标志位
    bool isCrop;
    //取消裁剪状态的时间
    QDateTime cropTime;
    //视频滤镜数据结构体
    FilterData videoFilter;

    //最大滤镜帧数量
    int maxFilterFrame;
    //当前用的哪个帧用来输出滤镜帧
    int filterFrameIndex;
    //滤镜帧队列/用完排队释放
    QList<AVFrame *> filterFrames;
    //已经使用的滤镜帧队列
    QList<AVFrame *> filterFramex;

    //视频帧锁/防止获取图片的时候数据冲突
    bool lockData;
    QMutex mutexFrame;

private:
    //读取并清空(视频流暂停期间)
    void readAndClear();

    //重新播放(循环播放)
    void replay();

    //重新打开(重连打开)
    void reopen();

    //动态监测尺寸变化
    bool checkFrameSize(AVFrame *frame);

    //转换和保存视频
    bool scaleAndSaveVideo(bool &needScale, AVFrame *frame);

    //处理和显示视频
    void checkAndShowVideo(bool needScale, AVFrame *frame);

public:
    //解码视频
    void decodeVideo0(AVPacket *packet);

    void decodeVideo1(AVPacket *packet);

    void decodeVideo2(AVPacket *packet);

    //解码音频
    void decodeAudio0(AVPacket *packet);

    void decodeAudio1(AVPacket *packet);

    void decodeAudio2(AVPacket *packet);

private:
    //初始化保存类的属性
    void initSaveFile();

    //写入视音频数据到文件
    void writeFile(AVPacket *packet, bool video);

    void writeFile(AVFrame *frame, bool video);

private slots:

    //初始化通信
    void initAvio();

    //初始化参数
    void initOption();

    //初始化输入
    bool initInput();

    //初始化视频
    bool initVideo();

    //初始化音频
    bool initAudio();

    //初始化相关数据
    void initData();

    //初始化音频播放
    void initAudioPlayer();

    //初始化音频输出
    void initAudioOutput();

    //初始化音效库
    void initAudioEffect();

    //初始化专辑
    void initMetadata();

    //初始化滤镜
    void initFilter();

    //重置滤镜
    void resetFilter();

    //打开视频
    bool openVideo();

    //关闭视频
    void closeVideo();

public:
    //锁住视频帧和释放视频帧锁
    void lockFrame();

    void unlockFrame();

    Q_INVOKABLE void setLockData(bool lockData);

    Q_INVOKABLE bool setValue(const char *name, const QVariant &value);

    //获取图片
    QImage getImage();

    //获取初始化的时间
    qint64 getStartTime();

    //获取最后的活动时间
    QDateTime getLastTime();

    //获取视频保存类
    FFmpegSave *getSaveFile();

    //获取视频流对象
    AVStream *getVideoStream();

    //获取音频流对象
    AVStream *getAudioStream();

public slots:

    //获取和设置开启推流预览
    bool getPushPreview();

    void setPushPreview(bool pushPreview);

    //设置是否转发数据包或数据帧
    void setSendPacket(bool sendPacket);

    void setSendFrame(bool sendFrame);

    //获取是否已经尝试打开过
    bool getTryOpen();

    //获取是否已经尝试读取过
    bool getTryRead();

    //获取是否尝试停止线程
    bool getTryStop();

    //清空缓存数据
    void clearBuffer(bool direct);

    //清空复位同步线程
    void clearAndReset();

    //获取和设置旋转角度
    int getRotate();

    void setRotate(int rotate);

    //获取文件时长
    qint64 getDuration();

    //获取和设置播放位置
    qint64 getPosition();

    void setPosition(qint64 position);

    void setPosition();

    //获取和设置播放速度
    double getSpeed();

    void setSpeed(double speed);

    //获取和设置音量大小
    int getVolume();

    void setVolume(int volume);

    //获取和设置静音状态
    bool getMuted();

    void setMuted(bool muted);

    //开启音频振幅和统计实时码率
    void setAudioLevel(bool audioLevel);

    void setRealBitRate(bool realBitRate);

    //切换声卡
    void setAudioDevice(const QString &audioDevice);

    //设置裁剪状态
    void setCrop(bool isCrop);

    //设置唯一标识
    void setFlag(const QString &flag);

    //打印信息
    void debug(int result, const QString &head, const QString &msg);

public slots:

    //暂停播放
    void pause();

    //继续播放
    void next();

    //按帧播放
    void step(bool backward = false);

    //开始录制
    void recordStart(const QString &fileName);

    //暂停录制
    void recordPause();

    //停止录制
    void recordStop();

    //设置标签信息集合
    void setOsdInfo(const QList<OsdInfo> &listOsd);

    //设置图形信息集合
    void setGraphInfo(const QList<GraphInfo> &listGraph);

signals:

    //转发数据包或数据帧(用于单独取出解码数据处理)
    void writePacket(AVPacket *packet, int index);

    void writeFrame(AVFrame *frame, int index);
};

#endif // FFMPEGTHREAD_H
