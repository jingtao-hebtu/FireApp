#include "ffmpegsavehelper.h"
#include "ffmpegsave.h"
#include "ffmpeghelper.h"
#include "ffmpegutil.h"

//格式支持说明 https://github.com/bluenviron/mediamtx
//不同流媒体服务程序支持的格式可能不完全一致/要做相应的调整
//不需要转码的视音频格式/经过验证后的可以自行添加
bool FFmpegSaveHelper::rtmp_pcm = false;
QStringList FFmpegSaveHelper::vnames_file = QStringList() << "h264" << "hevc";
QStringList FFmpegSaveHelper::anames_pcm = QStringList() << "pcm_mulaw" << "pcm_alaw" << "pcm_s16be";
QStringList FFmpegSaveHelper::anames_file =
        QStringList() << "aac" << "mp2" << "mp3" << "ac3" << anames_pcm << "pcm_s16le";
QStringList FFmpegSaveHelper::anames_rtmp = QStringList() << "aac" << "mp3";
QStringList FFmpegSaveHelper::anames_rtsp = QStringList() << "aac" << "mp3" << anames_pcm;

void FFmpegSaveHelper::checkEncode(FFmpegSave *thread, const QString &videoCodecName, const QString &audioCodecName,
                                   bool &videoEncode, bool &audioEncode, EncodeAudio &encodeAudio, bool &needAudio) {
    //推流和录制要区分判断(推流更严格/主要限定在流媒体服务器端)
    bool notSupportVideo = false;
    bool notSupportAudio = false;
    SaveMode saveMode = thread->getSaveMode();
    QString mediaUrl = thread->property("mediaUrl").toString();
    if (saveMode == SaveMode_File) {
        notSupportVideo = !vnames_file.contains(videoCodecName);
        notSupportAudio = !anames_file.contains(audioCodecName);
    } else {
        //具体需要根据实际需求进行调整
        if (saveMode == SaveMode_Rtmp) {
            notSupportVideo = (videoCodecName != "h264");
            notSupportAudio = !anames_rtmp.contains(audioCodecName);
        } else if (saveMode == SaveMode_Rtsp) {
            notSupportVideo = !vnames_file.contains(videoCodecName);
            notSupportAudio = !anames_rtsp.contains(audioCodecName);
        } else if (saveMode == SaveMode_Srt) {
            notSupportVideo = !vnames_file.contains(videoCodecName);
            notSupportAudio = !anames_rtmp.contains(audioCodecName);
            //音频不符合暂时先禁用音频
            if (notSupportAudio) {
                needAudio = false;
            }
        } else if (saveMode == SaveMode_Udp || saveMode == SaveMode_Tcp) {
            notSupportVideo = !vnames_file.contains(videoCodecName);
            notSupportAudio = !anames_file.contains(audioCodecName);
        }

        //特定格式过滤
        if (mediaUrl.endsWith(".m3u8")) {
            notSupportAudio = true;
        }
    }

    if (notSupportVideo) {
        thread->debug(0, "视频格式", QString("警告: %1").arg(videoCodecName));
        videoEncode = true;
    }

    if (notSupportAudio) {
        thread->debug(0, "音频格式", QString("警告: %1").arg(audioCodecName));
        audioEncode = true;
    }

    //0. 因为还没有搞定万能转换/所以暂时做下面的限制
    //1. 保存文件模式下纯音频统一编码成pcma
    //2. 保存文件模式下视音频且启用了转码则禁用音频
    //3. 推流RTMP模式下启用了转码则禁用音频
    //4. 推流RTSP模式下纯音频且启用了转码则编码成pcma
    //5. 推流RTSP模式下启用了转码则禁用音频
    //6. 纯音频aac格式在推流的时候可选转码/有些流媒体程序必须要求转码才能用
    bool encodeAac = false;
    bool onlySaveAudio = thread->getOnlySaveAudio();
    bool onlyAac = (onlySaveAudio && audioCodecName == "aac");
    if (encodeAudio == EncodeAudio_Auto) {
        if (saveMode == SaveMode_File) {
            //后面发现设置成mov格式也可以将pcm数据写入到文件
            if (onlySaveAudio /*|| audioCodecName == "pcm_s16le"*/) {
                encodeAudio = EncodeAudio_Pcma;
            } else if (audioEncode) {
                needAudio = false;
            }
        } else if (saveMode == SaveMode_Rtmp) {
            if (audioEncode) {
                needAudio = false;
            } else if (onlyAac && encodeAac) {
                encodeAudio = EncodeAudio_Aac;
            }
        } else if (saveMode == SaveMode_Rtsp) {
            if (audioEncode) {
                encodeAudio = EncodeAudio_Pcma;
            } else if (onlyAac && encodeAac) {
                encodeAudio = EncodeAudio_Pcma;
            }
        }
    }

    //如果设置过需要检查B帧/有B帧推流需要转码/否则一卡卡
    if (!videoEncode && !onlySaveAudio && saveMode != SaveMode_File) {
        bool checkB = thread->property("checkB").toBool();
        bool isFile = thread->property("isFile").toBool();
        if (checkB && isFile && FFmpegUtil::hasB(mediaUrl)) {
            videoEncode = true;
        }
    }

    //部分流媒体服务支持推pcma和pcmu
    if (rtmp_pcm && saveMode == SaveMode_Rtmp && anames_pcm.contains(audioCodecName)) {
        needAudio = true;
        encodeAudio = EncodeAudio_Pcma;
    }

    //指定了加密但是视频文件中的音频非aac格式需要禁用音频
    if (!thread->property("encryptKey").toString().isEmpty() && anames_pcm.contains(audioCodecName)) {
        needAudio = false;
    }

    //音频需要强转则必须设置启用音频编码
    if (encodeAudio != EncodeAudio_Auto) {
        audioEncode = true;
    }
}

void
FFmpegSaveHelper::checkFileName(SaveVideoType saveVideoType, EncodeVideo encodeVideo, const QString &videoCodecName,
                                QString &fileName) {
    //流文件自动纠正拓展名
    if (saveVideoType == SaveVideoType_Stream) {
        QString suffix = "." + fileName.split(".").last();
        if (encodeVideo == EncodeVideo_H264) {
            fileName.replace(suffix, ".h264");
        } else if (encodeVideo == EncodeVideo_H265) {
            fileName.replace(suffix, ".h265");
        } else {
            fileName.replace(suffix, videoCodecName == "hevc" ? ".h265" : ".h264");
        }
    }
}

const char *FFmpegSaveHelper::getFormat(const QString &url, bool mov) {
    //默认是mp4/mov更具兼容性比如音频支持pcma等
    const char *format = mov ? "mov" : "mp4";
    if (url.startsWith("rtmp://") || url.startsWith("rtmps://")) {
        format = "flv";
    } else if (url.startsWith("rtsp://") || url.startsWith("rtsps://")) {
        format = "rtsp";
    } else if (url.startsWith("srt://") || url.startsWith("udp://") || url.startsWith("tcp://")) {
        format = "mpegts";
    }

    return format;
}

const char *FFmpegSaveHelper::getFormat(AVDictionary **options, QString &url, bool mov, const QString &flag) {
    const char *format = FFmpegSaveHelper::getFormat(url, mov);
    if (format == "mov" || format == "mp4") {
        QByteArray temp;
        if (!flag.isEmpty()) {
            temp = flag.toUtf8();
            format = temp.constData();
            QString suffix = url.split(".").last();
            url.replace(suffix, flag);
        }
    } else if (format == "rtsp") {
        av_dict_set(options, "stimeout", "3000000", 0);
        av_dict_set(options, "rtsp_transport", "tcp", 0);
    }

    return format;
}

qint64 FFmpegSaveHelper::getBitRate(int width, int height) {
    qint64 bitRate = 400;
    int size = width * height;
    if (size <= (640 * 360)) {
        bitRate = 400;
    } else if (size <= (960 * 540)) {
        bitRate = 900;
    } else if (size <= (1280 * 720)) {
        bitRate = 1500;
    } else if (size <= (1920 * 1080)) {
        bitRate = 3000;
    } else if (size <= (2560 * 1440)) {
        bitRate = 3500;
    } else if (size <= (3840 * 2160)) {
        bitRate = 6000;
    }

    return bitRate * 1000;
}

QStringList FFmpegSaveHelper::getSizes(const QString &size) {
    QStringList sizes;
    if (size.contains("*")) {
        sizes = size.split("*");
    } else if (size.contains("x")) {
        sizes = size.split("x");
    }
    return sizes;
}

void FFmpegSaveHelper::getVideoSize(const QString &encodeVideoScale, int videoWidth, int videoHeight, int &width,
                                    int &height) {
    //需要转换尺寸的启用目标尺寸
    width = videoWidth;
    height = videoHeight;
    if (encodeVideoScale != "1") {
        QStringList sizes = FFmpegSaveHelper::getSizes(encodeVideoScale);
        if (sizes.count() == 2) {
            width = sizes.at(0).toInt();
            height = sizes.at(1).toInt();
        } else {
            float scale = encodeVideoScale.toFloat();
            width = videoWidth * scale;
            height = videoHeight * scale;
        }

        //尺寸必须是偶数 https://blog.csdn.net/weixin_30776545/article/details/98363730
        if (width % 2 != 0) {
            width--;
        }
        if (height % 2 != 0) {
            height--;
        }
    }
}

void
FFmpegSaveHelper::initVideoCodec(AVCodecx **videoCodec, AVCodecID codecId, int encodeVideo, const QString &fileName) {
    //指定了则按照指定的格式转码/没有指定则自动识别/源头是啥格式就转啥格式
    if (encodeVideo == EncodeVideo_H264) {
        (*videoCodec) = avcodec_find_encoder(AV_CODEC_ID_H264);
    } else if (encodeVideo == EncodeVideo_H265) {
        (*videoCodec) = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    } else {
        if (codecId == AV_CODEC_ID_HEVC) {
            (*videoCodec) = avcodec_find_encoder(AV_CODEC_ID_HEVC);
        } else {
            (*videoCodec) = avcodec_find_encoder(AV_CODEC_ID_H264);
        }
    }

    //RTMP流媒体目前只支持H264
    if (fileName.startsWith("rtmp://") || fileName.startsWith("rtmps://")) {
        (*videoCodec) = avcodec_find_encoder(AV_CODEC_ID_H264);
    }
}

void
FFmpegSaveHelper::initVideoCodecCtx(AVCodecContext *videoCodecCtx, int mediaType, int width, int height, int frameRate,
                                    float encodeVideoRatio) {
    //AVCodecContext结构体参数: https://blog.csdn.net/weixin_44517656/article/details/109707539
    //放大系数是为了小数位能够正确放大到整型
    int ratio = 1000000;
    videoCodecCtx->time_base.num = 1 * ratio;
    videoCodecCtx->time_base.den = frameRate * ratio;
    videoCodecCtx->framerate.num = frameRate * ratio;
    videoCodecCtx->framerate.den = 1 * ratio;

    //下面这种方式对编译器有版本要求(c++11)
    //videoCodecCtx->time_base = {1, frameRate};
    //videoCodecCtx->framerate = {frameRate, 1};

    //参数说明 https://blog.csdn.net/qq_40179458/article/details/110449653
    //大分辨率需要加上下面几个参数设置(否则在32位的库不能正常编码提示 Generic error in an external library)
    if ((width >= 3840 || height >= 2160)) {
        videoCodecCtx->qmin = 10;
        videoCodecCtx->qmax = 51;
        videoCodecCtx->me_range = 16;
        videoCodecCtx->max_qdiff = 4;
        videoCodecCtx->qcompress = 0.6f;
    }

    //初始化视频编码器参数
    videoCodecCtx->bit_rate = 0;
    videoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    videoCodecCtx->width = width;
    videoCodecCtx->height = height;
    videoCodecCtx->level = 50;
    //多少帧一个I帧(关键帧)
    videoCodecCtx->gop_size = frameRate;
    //去掉B帧只留下I帧和P帧
    videoCodecCtx->max_b_frames = 0;
    //videoCodecCtx->bit_rate_tolerance = 1;
    videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    videoCodecCtx->profile = FF_PROFILE_H264_MAIN;

    //缩放比小于1则按照这个缩放比设置码率/大于1则将这个值作为关键帧间隔
    //如果要文件体积小一些画质差一些可以降低码率/通过指定缩放比来设置关键帧间隔/极大降低文件体积
    if (encodeVideoRatio < 1) {
        videoCodecCtx->bit_rate = FFmpegSaveHelper::getBitRate(width, height) * encodeVideoRatio;
    } else if (encodeVideoRatio > 1) {
        videoCodecCtx->gop_size = encodeVideoRatio;
        return;
    }

    //加速选项 https://www.jianshu.com/p/034f5b3e7f94
    //加载预设 https://blog.csdn.net/JineD/article/details/125304570
    //速度选项 ultrafast/superfast/veryfast/faster/fast/medium/slow/slower/veryslow/placebo
    //视觉优化 film/animation/grain/stillimage/psnr/ssim/fastdecode/zerolatency

    //设置零延迟(本地采集设备视频流保存如果不设置则播放的时候会越来越模糊)
    //测试发现有些文件需要开启才不会慢慢变模糊/有些开启后在部分系统环境会偶尔卡顿(webrtc下)/根据实际需求决定是否开启
    av_opt_set(videoCodecCtx->priv_data, "tune", "zerolatency", 0);
    //crf参数可以很好的控制清晰度和文件体积/范围值1-51/值越大越模糊/建议18-28
    //av_opt_set(videoCodecCtx->priv_data, "crf", "28", 0);

    //文件类型除外(保证文件的清晰度)
    if (mediaType > 2) {
        av_opt_set(videoCodecCtx->priv_data, "preset", "ultrafast", 0);
        av_opt_set(videoCodecCtx->priv_data, "x265-params", "qp=20", 0);
    }
}

//AVFrame分配方式 https://blog.csdn.net/xionglifei2014/article/details/90693048/
//av_frame_get_buffer说明 https://blog.csdn.net/Hardy20200507/article/details/115491207
bool
FFmpegSaveHelper::initVideoConvert(FFmpegSave *thread, AVFrame *videoFrame, SwsContext **videoSwsCtx, int videoWidth,
                                   int videoHeight, int width, int height) {
    videoFrame->format = AV_PIX_FMT_YUV420P;
    videoFrame->width = width;
    videoFrame->height = height;

    //申请数据
    int result = av_frame_get_buffer(videoFrame, 0);
    if (result < 0) {
        thread->debug(result, "视频编码", "av_frame_get_buffer");
        return false;
    }

    //创建转换对象
    (*videoSwsCtx) = sws_getContext(videoWidth, videoHeight, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_YUV420P,
                                    SWS_BICUBIC, NULL, NULL, NULL);
    if (!(*videoSwsCtx)) {
        thread->debug(result, "视频编码", "sws_getContext");
        return false;
    }

    return true;
}

void FFmpegSaveHelper::initAudioCodecCtx(AVCodecContext *audioCodecCtx, AVSampleFormat sampleFormat, int sampleRate,
                                         int channelCount, float encodeSpeed) {
    audioCodecCtx->bit_rate = 64000;
    audioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    audioCodecCtx->sample_fmt = sampleFormat;
    audioCodecCtx->sample_rate = sampleRate * encodeSpeed;
    //audioCodecCtx->profile = FF_PROFILE_AAC_MAIN;
    audioCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    audioCodecCtx->time_base.den = audioCodecCtx->sample_rate;
    audioCodecCtx->time_base.num = 1;
    FFmpegHelper::initChannel(audioCodecCtx, channelCount);
}

bool FFmpegSaveHelper::initAudioConvert(FFmpegSave *thread, AVFrame *audioFrame, SwrContext **audioSwrCtx,
                                        AVCodecContext *audioCodecCtx, AVSampleFormat sampleFormat, int sampleRate,
                                        int channelCount) {
    FFmpegHelper::initFrame(audioFrame, audioCodecCtx);

    //申请数据
    int result = av_frame_get_buffer(audioFrame, 0);
    if (result < 0) {
        thread->debug(result, "音频编码", "av_frame_get_buffer");
        return false;
    }

    //实例化音频转换对象
    result = FFmpegHelper::initSwrContext(audioSwrCtx, channelCount, audioCodecCtx->sample_fmt,
                                          audioCodecCtx->sample_rate, channelCount, sampleFormat, sampleRate);
    if (result < 0) {
        thread->debug(result, "音频编码", "initSwrContext");
        return false;
    }

    return true;
}

void FFmpegSaveHelper::initBsfCtx(AVStream *stream, AVBSFContextx **bsfCtx, bool h264) {
    if (!(*bsfCtx)) {
        const char *name = (h264 ? "h264_mp4toannexb" : "hevc_mp4toannexb");
#if (FFMPEG_VERSION_MAJOR < BSFVersion)
        *bsfCtx = av_bitstream_filter_init(name);
#else
        av_bsf_alloc(av_bsf_get_by_name(name), bsfCtx);
        avcodec_parameters_copy((*bsfCtx)->par_in, stream->codecpar);
        av_bsf_init(*bsfCtx);
#endif
    }
}

void FFmpegSaveHelper::freeBsfCtx(AVBSFContextx **bsfCtx) {
    if (*bsfCtx) {
#if (FFMPEG_VERSION_MAJOR < BSFVersion)
        av_bitstream_filter_close(*bsfCtx);
#else
        av_bsf_free(bsfCtx);
#endif
        bsfCtx = NULL;
    }
}

void FFmpegSaveHelper::writeBsf(AVPacket *packet, AVStream *stream, AVBSFContextx *bsfCtx) {
    if (bsfCtx) {
#if (FFMPEG_VERSION_MAJOR < BSFVersion)
        av_bitstream_filter_filter(bsfCtx, stream->codec, NULL, &packet->data, &packet->size, packet->data, packet->size, 0);
#else
        av_bsf_send_packet(bsfCtx, packet);
        av_bsf_receive_packet(bsfCtx, packet);
#endif
    }
}

void FFmpegSaveHelper::freeBsf(AVPacket *packet) {
    //旧版本必须写入后释放/否则内存泄漏
#if (FFMPEG_VERSION_MAJOR < BSFVersion)
    av_free(packet->data);
#endif
}

void FFmpegSaveHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, qint64 &count, qreal fps) {
    count++;
    qreal duration = AV_TIME_BASE / fps;
    packet->pts = (count * duration) / (av_q2d(timeBaseIn) * AV_TIME_BASE);
    packet->dts = packet->pts;
    packet->duration = duration / (av_q2d(timeBaseIn) * AV_TIME_BASE);
}

void FFmpegSaveHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut) {
    //1. av_rescale_q里面调用的av_rescale_q_rnd
    //2. av_rescale_q_rnd里面调用的av_rescale_rnd
    //3. av_packet_rescale_ts对pts/dts/duration三者调用av_rescale_q
#if 1
    packet->pts = av_rescale_q_rnd(packet->pts, timeBaseIn, timeBaseOut,
                                   AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->dts = av_rescale_q_rnd(packet->dts, timeBaseIn, timeBaseOut,
                                   AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->duration = av_rescale_q(packet->duration, timeBaseIn, timeBaseOut);
#else
    av_packet_rescale_ts(packet, timeBaseIn, timeBaseOut);
#endif
    packet->pos = -1;
}

void FFmpegSaveHelper::rescalePacket(AVPacket *packet, qint64 &duration) {
    packet->pts = duration;
    packet->dts = duration;
    packet->pos = -1;
    duration += packet->duration;
}

void
FFmpegSaveHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut, qint64 &duration) {
    //qDebug() << TIMEMS << duration << packet->duration << packet->pts << packet->dts << timeBaseIn.den << timeBaseIn.num << timeBaseOut.den << timeBaseOut.num;
    packet->pts = av_rescale_q_rnd(duration, timeBaseIn, timeBaseOut, AV_ROUND_UP);
    packet->dts = packet->pts;//av_rescale_q_rnd(duration, timeBaseIn, timeBaseOut, AV_ROUND_UP);
    //packet->duration = av_rescale_q_rnd(packet->duration, timeBaseIn, timeBaseOut, AV_ROUND_UP);
    packet->pos = -1;
    duration += packet->duration;
}

int FFmpegSaveHelper::encode(FFmpegSave *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video) {
    int result = -1;
    QString flag = video ? "视频编码" : "音频编码";
#if (FFMPEG_VERSION_MAJOR < 3)
    if (video) {
        avcodec_encode_video2(avctx, packet, frame, &result);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_encode_video2");
            return result;
        }
    } else {
        avcodec_encode_audio2(avctx, packet, frame, &result);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_encode_audio2");
            return result;
        }
    }
    goto end;
#else
    result = avcodec_send_frame(avctx, frame);
    if (result < 0) {
        thread->debug(result, flag, "avcodec_send_frame");
        return result;
    }

    while (result >= 0) {
        result = avcodec_receive_packet(avctx, packet);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            //thread->debug(result, flag, "avcodec_receive_packet");
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_packet");
            break;
        }
        goto end;
    }
#endif
    return result;

    end:
    thread->writePacket2(packet, video);
    return result;
}

int FFmpegSaveHelper::openAndWriteCallBack(void *ctx) {
    FFmpegSave *thread = (FFmpegSave *) ctx;
    if (thread->getTryOpen()) {
        //时间差值=当前时间-开始解码的时间(单位微秒)
        qint64 offset = av_gettime() - thread->getStartTime();
        int timeout = thread->getConnectTimeout() * 1000;
        //没有设定对应值的话限定最小值
        timeout = (timeout <= 0 ? 3000000 : timeout);
        if (offset > timeout) {
            thread->debug(0, "打开超时", QString("最大: %1 超时: %2").arg(timeout).arg(offset));
            return 1;
        }
    }

    return 0;
}
