#ifndef FFMPEGHELPER_H
#define FFMPEGHELPER_H

#ifdef videoffmpeg

#include "ffmpegthread.h"

#else
class FFmpegThread;
#include "ffmpeginclude.h"
#endif

class FFmpegHelper {
public:
    //获取版本
    static QString getVersion();

    //将枚举值错误代号转成字符串
    static QString getError(int errnum);

    //初始化库(一个软件中只需要初始化一次就行)
    static void initLib();

    //初始化各种参数
    static void initRtspFast(AVFormatContext *formatCtx);

    static void initDecryption(AVDictionary **options, const QByteArray &cryptoKey);

    static void initEncryption(AVDictionary **options, const QByteArray &cryptoKey);

    static void initOption(AVDictionary **options, int caching, const QString &transport);

    static void initOption(AVDictionary **options, const QString &mediaUrl);

    static void initOption(AVDictionary **options, const QString &bufferSize, int frameRate);

    static void initOption(AVDictionary **options, int offsetX, int offsetY, QString &mediaUrl);

    //设置本地设备采集格式
    static void setVideoCodecName(AVFormatContext *formatCtx, const QString &videoCodecName);

    //格式枚举值转字符串
    static QString getFormatString(int format, bool video);

    static QString getPixFormatString(int format);

    static QString getSampleFormatString(int format);

    //获取pts值(带矫正)
    static qint64 getPts(AVPacket *packet);

    //播放时刻值(单位秒)
    static double getPtsTime(AVFormatContext *formatCtx, AVPacket *packet);

    //播放时长值(单位秒)
    static double getDurationTime(AVFormatContext *formatCtx, AVPacket *packet);

    //延时时间值(单位微秒)
    static qint64 getDelayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime);

    //根据时间差延时
    static void delayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime);

    //获取视频帧率
    static qreal getFrameRate(AVStream *stream, const QString &formatName = QString());

    //初始化音频转换上下文
    static int initSwrContext(SwrContext **swrCtx, int out_channels, AVSampleFormat out_sample_fmt, int out_sample_rate,
                              int in_channels, AVSampleFormat in_sample_fmt, int in_sample_rate);

    //初始化音频帧
    static void initFrame(AVFrame *frame, AVCodecContext *codecCtx);

    //初始化和获取音频通道相关
    static int getChannel(AVCodecContext *codecCtx);

    static void initChannel(AVFrame *frame, int channels);

    static void initChannel(AVCodecContext *codecCtx, int channels);

    static void initChannel(AVFrame *frame, AVCodecContext *codecCtx);

    //获取和设置旋转角度
    static int getRotate(AVStream *stream);

    static void setRotate(AVStream *stream, int rotate);

    //获取解码器枚举值和名称
    static AVCodecID getCodecId(AVStream *stream);

    static QString getCodecName(AVStream *stream);

    //获取码率
    static qint64 getBitRate(AVStream *stream);

    //获取格式
    static int getFormat(AVStream *stream);

    //获取类型
    static AVMediaType getMediaType(AVStream *stream);

    //获取分辨率
    static void getResolution(AVStream *stream, int &width, int &height);

    //获取采样等
    static void getAudioInfo(AVStream *stream, int &sampleRate, int &channelCount, int &profile);

    //获取流信息
    static void
    getStreamInfo(AVStream *stream, int &id, int &type, int &format, qint64 &bitrate, int &width, int &height,
                  int &sampleRate, int &channelCount, int &profile);

    //获取地址
    static QString getPlayUrl(const QString &mediaUrl);

    static QString getUrl(AVFormatContext *formatCtx);

    //获取视音频轨道
    static void getTracks(AVFormatContext *formatCtx, QList<int> &audioTracks, QList<int> &videoTracks);

    //拷贝上下文参数
    static int copyContext(AVStream *streamIn, AVStream *streamOut);

    static int copyContext(AVCodecContext *avctx, AVStream *stream, bool from);

    //判断是否是关键帧
    static bool checkPacketKey(AVPacket *packet);

    //生成一个数据包对象
    static AVPacket *creatPacket(AVPacket *packet);

    //释放数据帧数据包/2结尾的是释放后同时指针复位
    static void freeFrame(AVFrame *frame);

    static void freeFrame2(AVFrame **frame);

    static void freePacket(AVPacket *packet);

    static void freePacket2(AVPacket **packet);
};

#endif // FFMPEGHELPER_H
