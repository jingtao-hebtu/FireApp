#ifndef FFMPEGSAVEHELPER_H
#define FFMPEGSAVEHELPER_H

#include "qfile.h"
#include "qstringlist.h"
#include "widgetstruct.h"
#include "ffmpeginclude.h"

class FFmpegSave;

class FFmpegSaveHelper {
public:
    //rtmp推流音频pcma标志位
    static bool rtmp_pcm;
    //支持的视音频格式
    static QStringList vnames_file;
    static QStringList anames_pcm;
    static QStringList anames_file;
    static QStringList anames_rtmp;
    static QStringList anames_rtsp;

    //校验是否需要编码
    static void
    checkEncode(FFmpegSave *thread, const QString &videoCodecName, const QString &audioCodecName, bool &videoEncode,
                bool &audioEncode, EncodeAudio &encodeAudio, bool &needAudio);

    //校验保存文件名
    static void checkFileName(SaveVideoType saveVideoType, EncodeVideo encodeVideo, const QString &videoCodecName,
                              QString &fileName);

    //获取保存的格式
    static const char *getFormat(const QString &url, bool mov = false);

    static const char *
    getFormat(AVDictionary **options, QString &url, bool mov = false, const QString &flag = QString());

    //根据分辨率获取码率
    static qint64 getBitRate(int width, int height);

    //分辨率字符串分割 640x480/640*480
    static QStringList getSizes(const QString &size);

    //根据缩放字符串获取最终宽高
    static void getVideoSize(const QString &encodeVideoScale, int videoWidth, int videoHeight, int &width, int &height);

    //初始化视频编码器
    static void initVideoCodec(AVCodecx **videoCodec, AVCodecID codecId, int encodeVideo, const QString &fileName);

    //初始化视频编码器上下文
    static void initVideoCodecCtx(AVCodecContext *videoCodecCtx, int mediaType, int width, int height, int frameRate,
                                  float encodeVideoRatio);

    //初始化视频转换相关
    static bool
    initVideoConvert(FFmpegSave *thread, AVFrame *videoFrame, SwsContext **videoSwsCtx, int videoWidth, int videoHeight,
                     int width, int height);

    //初始化音频编码器上下文
    static void
    initAudioCodecCtx(AVCodecContext *audioCodecCtx, AVSampleFormat sampleFormat, int sampleRate, int channelCount,
                      float encodeSpeed);

    //初始化音频转换相关
    static bool
    initAudioConvert(FFmpegSave *thread, AVFrame *audioFrame, SwrContext **audioSwrCtx, AVCodecContext *audioCodecCtx,
                     AVSampleFormat sampleFormat, int sampleRate, int channelCount);

    //封装格式(写入sps/pps信息)
    static void initBsfCtx(AVStream *stream, AVBSFContextx **bsfCtx, bool h264);

    static void freeBsfCtx(AVBSFContextx **bsfCtx);

    static void writeBsf(AVPacket *packet, AVStream *stream, AVBSFContextx *bsfCtx);

    static void freeBsf(AVPacket *packet);

    //转换时间基准
    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, qint64 &count, qreal fps);

    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut);

    static void rescalePacket(AVPacket *packet, qint64 &duration);

    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut, qint64 &duration);

    //通用软编码(支持音频视频)
    static int encode(FFmpegSave *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video);

    //打开超时和写入超时回调
    static int openAndWriteCallBack(void *ctx);
};

#endif // FFMPEGSAVEHELPER_H
