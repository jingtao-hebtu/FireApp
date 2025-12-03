#include "ffmpegthreadhelper.h"
#include "ffmpegthread.h"
#include "ffmpeghelper.h"

int FFmpegThreadHelper::getDecodeFlags(DecodeType decodeType) {
    //默认速度优先的解码采用的SWS_FAST_BILINEAR参数(可能会丢失部分图片数据)
    int flags = SWS_FAST_BILINEAR;
    if (decodeType == DecodeType_Fast) {
        flags = SWS_FAST_BILINEAR;
    } else if (decodeType == DecodeType_Full) {
        flags = SWS_BICUBIC;
    } else if (decodeType == DecodeType_Even) {
        flags = SWS_BILINEAR;
    }

    return flags;
}

void FFmpegThreadHelper::checkHardware(const QString &formatName, QString &hardware) {
    //有些格式不支持硬解码/或者该格式的部分文件/后面改成了创建硬解失败则转成软解码
    static QStringList formats;
    if (formats.count() == 0) {
        //formats << "rm" << "avi" << "webm";
    }

            foreach (QString format, formats) {
            if (formatName.contains(format)) {
                hardware = "none";
                break;
            }
        }

    //过低版本不支持硬解码
#if (FFMPEG_VERSION_MAJOR < 3)
    hardware = "none";
#endif

    //安卓上默认提供的库不带硬解码(需要自行编译带硬解的库)
#ifdef Q_OS_ANDROID
    hardware = "none";
#endif
}

void FFmpegThreadHelper::checkUseSync(FFmpegThread *thread, int videoIndex, int audioIndex, bool &useSync) {
    //默认需要同步
    useSync = true;

    //某些格式比如视频流不做音视频同步(响应速度快)
    if (thread->getFormatName() == "rtsp" || thread->getMediaUrl().endsWith(".sdp")) {
        useSync = false;
    }

    //本地或者网络文件必须同步
    if (thread->getIsFile()) {
        useSync = true;
    }

    //本地设备采集不需要同步/文件必须同步
    MediaType mediaType = thread->getMediaType();
    if (mediaType == MediaType_Device || mediaType == MediaType_Screen || mediaType == MediaType_Srt) {
        useSync = false;
    }

    //设置了最快速度则不启用音视频同步(质量优先需要同步)
    DecodeType decodeType = thread->getDecodeType();
    if (decodeType == DecodeType_Fastest) {
        useSync = false;
    } else if (decodeType == DecodeType_Full) {
        useSync = true;
    }

    //纯音频永远都要同步(有些rtsp是纯音频需要同步声音才正常/否则有些情况可能不正确)
    if (videoIndex < 0 && audioIndex >= 0) {
        useSync = true;
    }

    thread->debug(0, "同步策略", QString("同步: %1 类型: %2").arg(useSync ? "启用" : "禁用").arg(mediaType));
}

void
FFmpegThreadHelper::checkSaveAudioType(MediaType mediaType, int audioIndex, const QString &audioCodecName, int bitrate,
                                       SaveAudioType &saveAudioType) {
    //没有音频则不用录制音频
    if (audioIndex < 0) {
        saveAudioType = SaveAudioType_None;
    }

    //本地摄像头强制采用wav格式
    if (mediaType == MediaType_Device) {
        if (saveAudioType > 1) {
            saveAudioType = SaveAudioType_Wav;
        }
#ifndef Q_OS_WIN
        saveAudioType = SaveAudioType_None;
#endif
    }

    //设置了保存声音并且比特=0则强制改成wav格式
    if (saveAudioType == SaveAudioType_Aac && bitrate == 0) {
        saveAudioType = SaveAudioType_Wav;
    }

    //如果解码不是aac且设置了保存为aac则强制改成保存为wav
    //有些文件就算音频解码是aac然后用aac保存输出可能也有错(只需要改成wav格式100%正确)
    if (audioCodecName != "aac" && saveAudioType == SaveAudioType_Aac) {
        saveAudioType = SaveAudioType_Wav;
    }
}

void FFmpegThreadHelper::checkVideoEncode(MediaType mediaType, const QString &mediaUrl, SaveVideoType saveType,
                                          const QString &formatName, bool isFile, bool &mp4ToAnnexB,
                                          bool &videoEncode) {
    //测试发现文件在保存裸流的时候必须写入/否则无法播放
    if (isFile && saveType == SaveVideoType_Stream) {
        mp4ToAnnexB = true;
    }

    //部分厂家比如安迅士的需要封装格式保存才正常
    if (mediaType == MediaType_Rtsp && mediaUrl.contains("/axis-media/")) {
        mp4ToAnnexB = true;
    }

    if (formatName == "mpjpeg") {
        videoEncode = true;
    }
}

bool
FFmpegThreadHelper::checkFilter(bool graph, const QString &videoCodecName, const QString &hardware, MediaType mediaType,
                                const QString &mediaUrl) {
    //虚拟桌面的摄像头其实也是屏幕
    bool isScreen = (mediaType == MediaType_Screen || mediaUrl.contains("video=screen-capture-recorder"));
    //低分辨率的本地摄像头禁用滤镜/很奇怪会变成左右对称的两个画面
    if (videoCodecName == "rawvideo" && !isScreen) {
        return false;
    }

    //硬解码不支持图形滤镜
    //采集桌面不支持图形滤镜/ffmpeg5才开始支持
    if (graph) {
        if (hardware != "none") {
            return false;
        }
#if (FFMPEG_VERSION_MAJOR < 5)
        if (isScreen) {
            return false;
        }
#endif
    }

    return true;
}

QImage FFmpegThreadHelper::getAttachedPic(AVFormatContext *formatCtx) {
    QImage image;
    int count = formatCtx->nb_streams;
    for (int i = 0; i < count; ++i) {
        if (formatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
            AVPacket pkt = formatCtx->streams[i]->attached_pic;
            image = QImage::fromData((quint8 *) pkt.data, pkt.size);
            break;
        }
    }
    return image;
}

QImage FFmpegThreadHelper::frameToImage(SwsContext *swsCtx, AVFrame *srcFrame, AVFrame *dstFrame, quint8 *data) {
    //将数据转成图片
    int result = sws_scale(swsCtx, (const quint8 **) srcFrame->data, srcFrame->linesize, 0, srcFrame->height,
                           dstFrame->data, dstFrame->linesize);
    if (result < 0) {
        return QImage();
    } else {
        return QImage(data, srcFrame->width, srcFrame->height, QImage::Format_RGB888);
    }
}

void FFmpegThreadHelper::initVideoCodec(AVCodecx **videoCodec, AVCodecID codecId, QString &videoCodecName,
                                        QString &hardware) {
    //获取默认的解码器
    (*videoCodec) = avcodec_find_decoder(codecId);
    videoCodecName = (*videoCodec)->name;

    bool otherHardware = (hardware != "none" && hardware != "dxva2" && hardware != "d3d11va");
    if (!otherHardware) {
        return;
    }

    //264/265才能去启用系统层以外的硬解码
    if (videoCodecName != "h264" && videoCodecName != "hevc") {
        hardware = "none";
        return;
    }

    //指定硬解码器名称 h264_qsv/h264_cuvid/h264_vaapi hevc_qsv/hevc_cuvid/hevc_vaapi/h264_mediacodec/h264_rkmpp
    QString name = QString("%1_%2").arg(videoCodecName).arg(hardware);
    (*videoCodec) = avcodec_find_decoder_by_name(name.toUtf8().constData());
    //如果硬解码器分配失败则立即切换到软解码
    if (!(*videoCodec)) {
        (*videoCodec) = avcodec_find_decoder(codecId);
    }
}

bool FFmpegThreadHelper::initHardware(FFmpegThread *thread, AVCodecx *videoCodec, AVCodecContext *videoCodecCtx,
                                      const QString &hardware) {
#if (FFMPEG_VERSION_MAJOR > 2)
    //根据名称自动寻找硬解码
    enum AVHWDeviceType type;
    QByteArray hwData = hardware.toUtf8();
    const char *hwName = hwData.constData();
#ifdef __arm__
    //发现嵌入式上低版本的库没有av_hwdevice_find_type_by_name函数
#if (FFMPEG_VERSION_MAJOR < 4)
    return false;
#else
    type = av_hwdevice_find_type_by_name(hwName);
#endif
#else
    type = av_hwdevice_find_type_by_name(hwName);
#endif

    //找到对应的硬解码格式
    thread->debug(0, "硬件加速", QString("名称: %1 数值: %2").arg(hardware).arg(type));
    FFmpegThreadHelper::hw_pix_fmt = FFmpegThreadHelper::find_fmt_by_hw_type(type, videoCodec);
    if (FFmpegThreadHelper::hw_pix_fmt == -1) {
        thread->debug(0, "加速失败", "");
        return false;
    }

    int result = -1;
    //创建硬解码设备
    AVBufferRef *hw_device_ref;
    result = av_hwdevice_ctx_create(&hw_device_ref, type, NULL, NULL, 0);
    if (result < 0) {
        thread->debug(result, "创建硬解", "av_hwdevice_ctx_create");
        return false;
    }

    //解码器格式赋值为硬解码
    videoCodecCtx->get_format = FFmpegThreadHelper::get_hw_format;
    videoCodecCtx->hw_device_ctx = av_buffer_ref(hw_device_ref);
    av_buffer_unref(&hw_device_ref);
    thread->debug(result, "初始硬解", QString("成功: %1").arg(hardware));
    return true;
#else
    return false;
#endif
}

bool
FFmpegThreadHelper::initVideoData(FFmpegThread *thread, AVFrame *yuvFrame, AVFrame *imageFrame, SwsContext **yuvSwsCtx,
                                  SwsContext **imageSwsCtx, quint8 **imageData,
                                  AVPixelFormat srcFormat, AVPixelFormat dstFormat, int videoWidth, int videoHeight,
                                  const QString &hardware, int flags) {
    //设置属性以便该帧对象正常
    yuvFrame->format = AV_PIX_FMT_YUV420P;
    yuvFrame->width = videoWidth;
    yuvFrame->height = videoHeight;

    //如果发现画面斜了或者条纹状可以考虑修改这里的对齐值
    int align = (hardware == "none" ? 4 : 16);

    //分配视频数据(转yuv420)
    int result = av_frame_get_buffer(yuvFrame, align);
    if (result < 0) {
        thread->debug(result, "视频转换", "av_frame_get_buffer");
        return false;
    }

    //视频图像转换(转yuv420)
    (*yuvSwsCtx) = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, AV_PIX_FMT_YUV420P,
                                  flags, NULL, NULL, NULL);
    if (!yuvSwsCtx) {
        thread->debug(result, "视频转换", "sws_getContext");
        return false;
    }

    //视频图像转换(转rgb)
    AVPixelFormat imageFormat = AV_PIX_FMT_RGB24;
    (*imageSwsCtx) = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, imageFormat, flags,
                                    NULL, NULL, NULL);
    if (!imageSwsCtx) {
        thread->debug(result, "视频转换", "sws_getContext");
        return false;
    }

    //分配视频帧数据(转rgb)
    align = 4;
    int imageSize = av_image_get_buffer_size(imageFormat, videoWidth, videoHeight, align);
    (*imageData) = (quint8 *) av_malloc(imageSize * sizeof(quint8));
    result = av_image_fill_arrays(imageFrame->data, imageFrame->linesize, (*imageData), imageFormat, videoWidth,
                                  videoHeight, align);
    if (result < 0) {
        thread->debug(result, "视频转换", "av_image_fill_arrays");
        return false;
    }

    QString srcFormatString = FFmpegHelper::getPixFormatString(srcFormat);
    QString dstFormatString = FFmpegHelper::getPixFormatString(dstFormat);
    QString msg = QString("源头: %1 目标: %2 硬解: %3").arg(srcFormatString).arg(dstFormatString).arg(hardware);
    thread->debug(0, "视频转换", msg);
    return true;
}

bool FFmpegThreadHelper::initAudioData(FFmpegThread *thread, AVCodecContext *audioCodecCtx, AVFrame *pcmFrame,
                                       SwrContext **pcmSwrCtx,
                                       AVSampleFormat &pcmSampleFormat, int &pcmSampleRate, int &pcmChannels,
                                       const QString &audioCodecName) {
    AVSampleFormat sampleFormat = audioCodecCtx->sample_fmt;
    int sampleRate = audioCodecCtx->sample_rate;
    int channels = FFmpegHelper::getChannel(audioCodecCtx);

    pcmSampleFormat = sampleFormat;
    pcmSampleRate = sampleRate;
    pcmChannels = channels;

    //源头采集到的是pcm可以不用转
    bool convert = !audioCodecName.startsWith("pcm");
    if (convert) {
        //可以自行指定目标转换参数/也可用源头采样率和通道作为目标参数
        pcmSampleFormat = AV_SAMPLE_FMT_S16;
        //pcmSampleRate = 48000;
        //pcmChannels = 1;

        //有些帧大小居然是0/需要主动设置个值
        int frameSize = audioCodecCtx->frame_size;
        if (frameSize == 0) {
            frameSize = 9600;
        }

        //设置属性以便该帧对象正常
        pcmFrame->format = pcmSampleFormat;
        pcmFrame->sample_rate = pcmSampleRate;
        pcmFrame->nb_samples = frameSize;
        FFmpegHelper::initChannel(pcmFrame, pcmChannels);

        //分配音频数据(转pcm)
        int result = av_frame_get_buffer(pcmFrame, 0);
        if (result < 0) {
            thread->debug(result, "音频转换", "av_frame_get_buffer");
            return false;
        }

        //音频采样转换
        result = FFmpegHelper::initSwrContext(pcmSwrCtx, pcmChannels, pcmSampleFormat, pcmSampleRate, channels,
                                              sampleFormat, sampleRate);
        if (result < 0) {
            thread->debug(result, "音频转换", "initSwrContext");
            return false;
        }
    }

    QString srcFormatString = FFmpegHelper::getSampleFormatString(sampleFormat);
    QString dstFormatString = FFmpegHelper::getSampleFormatString(pcmSampleFormat);
    QString msg1 = QString("源头: %1/%2/%3").arg(srcFormatString).arg(sampleRate).arg(channels);
    QString msg2 = QString("目标: %1/%2/%3").arg(dstFormatString).arg(pcmSampleRate).arg(pcmChannels);
    thread->debug(0, "音频转换", QString("转换: %1").arg(convert ? "启用" : "禁用") + " " + msg1 + " " + msg2);
    return true;
}

void FFmpegThreadHelper::checkKeyPacketCount(FFmpegThread *thread, AVPacket *packet, qint64 &keyPacketIndex,
                                             qint64 &keyPacketCount) {
    keyPacketIndex++;
    keyPacketCount++;
    if (FFmpegHelper::checkPacketKey(packet)) {
        thread->debug(0, "关键帧", QString("间隔: %1 当前: %2").arg(keyPacketCount).arg(keyPacketIndex - 1));
        keyPacketCount = 0;
    }
}

void
FFmpegThreadHelper::checkRealPacketSize(FFmpegThread *thread, AVPacket *packet, int maxCount, qint64 &realPacketSize,
                                        qint64 &realPacketCount) {
    realPacketCount++;
    realPacketSize += packet->size;
    if (realPacketCount == maxCount) {
        qreal kbps = (qreal) realPacketSize / 100;
        //thread->debug(0, "实时码率", QString("大小: %1 kbps").arg(QString::number(kbps, 'f', 0)));
        QMetaObject::invokeMethod(thread, "receiveKbps", Q_ARG(qreal, kbps), Q_ARG(int, maxCount));
        realPacketSize = 0;
        realPacketCount = 0;
    }
}

AVPixelFormat FFmpegThreadHelper::hw_pix_fmt = AV_PIX_FMT_NONE;

AVPixelFormat FFmpegThreadHelper::get_hw_format(AVCodecContext *ctx, const AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt) {
            return *p;
        }
    }

    return AV_PIX_FMT_NONE;
}

AVPixelFormat FFmpegThreadHelper::find_fmt_by_hw_type(const AVHWDeviceType &type, const AVCodec *codec) {
    enum AVPixelFormat fmt = AV_PIX_FMT_NONE;
#if (FFMPEG_VERSION_MAJOR < 4)
    switch (type) {
        case AV_HWDEVICE_TYPE_DXVA2:
            fmt = AV_PIX_FMT_DXVA2_VLD;
            break;
#if (FFMPEG_VERSION_MAJOR > 2)
        case AV_HWDEVICE_TYPE_D3D11VA:
            fmt = AV_PIX_FMT_D3D11;
            break;
#endif
        case AV_HWDEVICE_TYPE_VAAPI:
            fmt = AV_PIX_FMT_VAAPI;
            break;
        case AV_HWDEVICE_TYPE_VDPAU:
            fmt = AV_PIX_FMT_VDPAU;
            break;
        case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
            fmt = AV_PIX_FMT_VIDEOTOOLBOX;
            break;
        default:
            fmt = AV_PIX_FMT_NONE;
            break;
    }
#else
    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
        if (!config) {
            break;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == type) {
            fmt = config->pix_fmt;
            break;
        }
    }
#endif
    return fmt;
}

void FFmpegThreadHelper::decode(FFmpegThread *thread, AVPacket *packet, bool video) {
    //调用线程处理解码后的数据
    if (video) {
        thread->decodeVideo2(packet);
    } else {
        thread->decodeAudio2(packet);
    }
}

int FFmpegThreadHelper::decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frameSrc,
                               AVFrame *frameDst) {
    int result = -1;
#ifdef videoffmpeg
    QString flag = "硬解出错";
#if (FFMPEG_VERSION_MAJOR > 2)
    result = avcodec_send_packet(avctx, packet);
    if (result < 0) {
        thread->debug(result, flag, "avcodec_send_packet");
        return result;
    }

    while (result >= 0) {
        result = avcodec_receive_frame(avctx, frameSrc);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_frame");
            break;
        }

        //将数据从GPU拷贝到CPU(这一步也比较耗CPU/最佳做法就是直接底层绘制显卡数据/目前不会)
        //result = av_hwframe_map(frameDst, frameSrc, 0);
        thread->lockFrame();
        result = av_hwframe_transfer_data(frameDst, frameSrc, 0);
        thread->unlockFrame();
        if (result < 0) {
            av_frame_unref(frameDst);
            av_frame_unref(frameSrc);
            thread->debug(result, flag, "av_hwframe_transfer_data");
            return result;
        }

        FFmpegThreadHelper::decode(thread, packet, true);
    }
#endif
#endif
    return result;
}

int
FFmpegThreadHelper::decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video) {
    int result = -1;
#ifdef videoffmpeg
    QString flag = video ? "视频解码" : "音频解码";
#if (FFMPEG_VERSION_MAJOR < 3)
    if (video) {
        avcodec_decode_video2(avctx, frame, &result, packet);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_decode_video2");
            return result;
        }
    } else {
        avcodec_decode_audio4(avctx, frame, &result, packet);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_decode_audio4");
            return result;
        }
    }

    FFmpegThreadHelper::decode(thread, packet, video);
#else
    result = avcodec_send_packet(avctx, packet);
    //有些国标ts文件会是其他几种结果但是能正常解码
    if (result < 0 && (result != AVERROR(EAGAIN)) && (result != AVERROR_EOF)) {
        //if (result < 0) {
        thread->debug(result, flag, "avcodec_send_packet");
        return result;
    }

    result = 0;
    while (result >= 0) {
        if (video) {
            //这里为什么要加个锁/因为外面可能其他线程在调用getImage函数获取图片/数据可能有冲突
            thread->lockFrame();
            result = avcodec_receive_frame(avctx, frame);
            thread->unlockFrame();
        } else {
            result = avcodec_receive_frame(avctx, frame);
        }

        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            //thread->debug(result, flag, "avcodec_receive_frame");
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_frame");
            break;
        }

        FFmpegThreadHelper::decode(thread, packet, video);
    }
#endif
#endif
    return result;
}

int FFmpegThreadHelper::openAndReadCallBack(void *ctx) {
#ifdef videoffmpeg
    FFmpegThread *thread = (FFmpegThread *) ctx;
    //先判断是否尝试停止线程(有时候不存在的地址反复打开关闭会卡主导致崩溃/多了这个判断可以立即停止)
    if (thread->getTryStop()) {
        if (thread->getTryRead()) {
            thread->debug(0, "主动停止", "");
        }
        return 1;
    }

    //打开超时判定和读取超时判定
    if (thread->getTryOpen()) {
        //时间差值=当前时间-开始解码的时间(单位微秒)
        qint64 offset = av_gettime() - thread->getStartTime();
        int timeout = thread->getConnectTimeout() * 1000;
        //没有设定对应值的话限定最小值
        timeout = (timeout <= 0 ? 3000000 : timeout);
        if (offset > timeout) {
            //thread->debug(0, "打开超时", QString("最大: %1 超时: %2").arg(timeout).arg(offset));
            return 1;
        }
    } else if (thread->getTryRead()) {
        //时间差值=当前时间-最后一次读取的时间(单位毫秒)
        QDateTime now = QDateTime::currentDateTime();
        qint64 offset = thread->getLastTime().msecsTo(now);
        int timeout = thread->getReadTimeout();
        //没有设定对应值的话限定最小值(rtmp值要大一点)
        timeout = (timeout <= 0 ? (thread->getMediaType() == MediaType_Rtmp ? 15000 : 3000) : timeout);
        if (offset > timeout) {
            //thread->debug(0, "读取超时", QString("最大: %1 超时: %2").arg(timeout).arg(offset));
            return 1;
        }
    }
#endif
    return 0;
}
