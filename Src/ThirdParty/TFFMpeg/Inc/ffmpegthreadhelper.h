#ifndef FFMPEGTHREADHELPER_H
#define FFMPEGTHREADHELPER_H

#include "ffmpeginclude.h"
#include "widgetstruct.h"
#include "videostruct.h"

class FFmpegThread;

class FFmpegThreadHelper {
public:
    //根据解码策略获取解码参数
    static int getDecodeFlags(DecodeType decodeType);

    //校验硬解码类型
    static void checkHardware(const QString &formatName, QString &hardware);

    //校验音视频同步标志
    static void checkUseSync(FFmpegThread *thread, int videoIndex, int audioIndex, bool &useSync);

    //矫正保存音频类型
    static void checkSaveAudioType(MediaType mediaType, int audioIndex, const QString &audioCodecName, int bitrate,
                                   SaveAudioType &saveAudioType);

    //校验是否需要视频转码保存
    static void
    checkVideoEncode(MediaType mediaType, const QString &mediaUrl, SaveVideoType saveType, const QString &formatName,
                     bool isFile, bool &mp4ToAnnexB, bool &videoEncode);

    //检验滤镜相关
    static bool checkFilter(bool graph, const QString &videoCodecName, const QString &hardware, MediaType mediaType,
                            const QString &mediaUrl);

    //读取封面
    static QImage getAttachedPic(AVFormatContext *formatCtx);

    //转换图片
    static QImage frameToImage(SwsContext *swsCtx, AVFrame *srcFrame, AVFrame *dstFrame, quint8 *data);

    //初始化视频解码器
    static void initVideoCodec(AVCodecx **videoCodec, AVCodecID codecId, QString &videoCodecName, QString &hardware);

    //初始化硬解相关
    static bool
    initHardware(FFmpegThread *thread, AVCodecx *videoCodec, AVCodecContext *videoCodecCtx, const QString &hardware);

    //初始化解码视频相关数据
    static bool initVideoData(FFmpegThread *thread, AVFrame *yuvFrame, AVFrame *imageFrame, SwsContext **yuvSwsCtx,
                              SwsContext **imageSwsCtx, quint8 **imageData,
                              AVPixelFormat srcFormat, AVPixelFormat dstFormat, int videoWidth, int videoHeight,
                              const QString &hardware, int flags);

    //初始化解码音频相关数据
    static bool
    initAudioData(FFmpegThread *thread, AVCodecContext *audioCodecCtx, AVFrame *pcmFrame, SwrContext **pcmSwrCtx,
                  AVSampleFormat &pcmSampleFormat, int &pcmSampleRate, int &pcmChannels, const QString &audioCodecName);

    //统计I帧(关键帧)间隔
    static void
    checkKeyPacketCount(FFmpegThread *thread, AVPacket *packet, qint64 &keyPacketIndex, qint64 &keyPacketCount);

    //计算实时码率
    static void checkRealPacketSize(FFmpegThread *thread, AVPacket *packet, int maxCount, qint64 &realPacketSize,
                                    qint64 &realPacketCount);

    //硬解码格式
    static enum AVPixelFormat hw_pix_fmt;

    //获取硬解码格式回调函数
    static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const AVPixelFormat *pix_fmts);

    //根据硬解码类型找到对应的硬解码格式
    static enum AVPixelFormat find_fmt_by_hw_type(const AVHWDeviceType &type, const AVCodec *codec);

    //通用解码包
    static void decode(FFmpegThread *thread, AVPacket *packet, bool video);

    //通用硬解码(音频没有硬解码)
    static int
    decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frameSrc, AVFrame *frameDst);

    //通用软解码(支持音频视频)
    static int decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video);

    //打开超时和读取超时回调
    static int openAndReadCallBack(void *ctx);
};

#endif // FFMPEGTHREADHELPER_H
