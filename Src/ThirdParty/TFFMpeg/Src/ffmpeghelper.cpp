#include "ffmpeghelper.h"
#include "ffmpegdevice.h"
#include "ffmpegutil.h"
#include "ffmpegsave.h"
#include "qmutex.h"
#include "qstringlist.h"
#include <QDebug>

QString FFmpegHelper::getVersion() {
    qDebug() << FFMPEG_VERSION;
    return QString("%1").arg(FFMPEG_VERSION);
}

QString FFmpegHelper::getError(int errnum) {
    //常见错误码 https://blog.csdn.net/sz76211822/article/details/52371966
    char errbuf[1024] = {0};
    av_strerror(errnum, errbuf, sizeof(errbuf));
    return QString("(%1) %2").arg(errnum).arg(errbuf);
}

void FFmpegHelper::initLib() {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit = false;
    if (!isInit) {
        //注册库中所有可用的文件格式和解码器
        //注册滤镜特效库相关(色调/模糊/水平翻转/裁剪/加方框/叠加文字等功能)
#if (FFMPEG_VERSION_MAJOR < 4)
        av_register_all();
        avfilter_register_all();
#endif
        //注册所有设备相关支持
#ifdef ffmpegdevice
        avdevice_register_all();
#endif
        //初始化网络流格相关(使用网络流时必须先执行)
        avformat_network_init();

        //设置日志级别
        //如果不想看到烦人的打印信息可以设置成 AV_LOG_QUIET 表示不打印日志
        //有时候发现使用不正常比如打开了没法播放视频则需要打开日志看下报错提示
        av_log_set_level(AV_LOG_QUIET);

        isInit = true;
        qDebug() << TIMEMS << QString("初始化库 -> 类型: %1 版本: %2").arg("ffmpeg").arg(getVersion());
        //FFmpegUtil::debugAll(false, true, false);
    }
}

void FFmpegHelper::initRtspFast(AVFormatContext *formatCtx) {
    //接口内部读取的最大数据量(从源文件中读取的最大字节数)
    //默认值5000000导致这里卡很久最耗时(可以调小来加快打开速度)
    formatCtx->probesize = 50000;
    //从文件中读取的最大时长(单位为 AV_TIME_BASE units)
    formatCtx->max_analyze_duration = 5 * AV_TIME_BASE;
    //内部读取的数据包不放入缓冲区
    //formatCtx->flags |= AVFMT_FLAG_NOBUFFER;
    //设置解码错误验证过滤花屏
    //formatCtx->error_recognition |= AV_EF_EXPLODE;
}

void FFmpegHelper::initDecryption(AVDictionary **options, const QByteArray &cryptoKey) {
    //如果设置了秘钥则需要启用解密
    if (!cryptoKey.isEmpty()) {
        av_dict_set(options, "decryption_key", cryptoKey.constData(), 0);
    }
}

void FFmpegHelper::initEncryption(AVDictionary **options, const QByteArray &cryptoKey) {
    //如果设置了秘钥则需要启用加密
    if (!cryptoKey.isEmpty()) {
        av_dict_set(options, "encryption_scheme", "cenc-aes-ctr", 0);
        av_dict_set(options, "encryption_key", cryptoKey.constData(), 0);
        av_dict_set(options, "encryption_kid", cryptoKey.constData(), 0);
    }
}

void FFmpegHelper::initOption(AVDictionary **options, int caching, const QString &transport) {
    //设置缓存大小(单位kb/默认32768/1024000=1MB)
    if (caching >= 1024000 && caching <= 102400000) {
        av_dict_set(options, "buffer_size", QString::number(caching).toUtf8().constData(), 0);
    }

    //设置超时断开连接时间(单位微秒/3000000表示3秒)
    av_dict_set(options, "stimeout", "3000000", 0);
    //设置最大时延(单位微秒/1000000表示1秒)
    av_dict_set(options, "max_delay", "1000000", 0);
    //自动开启线程数
    //av_dict_set(options, "threads", "auto", 0);
    av_dict_set(options, "threads", "8", 0);

    //通信协议采用tcp还是udp(udp优点是无连接/在网线拔掉以后十几秒钟重新插上还能继续接收/缺点是网络不好的情况下会丢包花屏)
    if (transport != "auto") {
        av_dict_set(options, "rtsp_transport", transport.toUtf8().constData(), 0);
    }

    //开启无缓存(rtmp等视频流不建议开启)
    //av_dict_set(options, "fflags", "nobuffer", 0);
    //av_dict_set(options, "fflags", "discardcorrupt", 0);

    //有些视频网站根据这个头部消息过滤/故意不让ffmpeg访问/所以需要模拟成其他的
    //可以使用\r\n添加多个/还有个headers参数也可以/scale用于倍速
    av_dict_set(options, "user_agent", "Mozilla", 0);
}

void FFmpegHelper::initOption(AVDictionary **options, const QString &mediaUrl) {
    //设置音频采集选项
    if (mediaUrl.contains("audio=") && !mediaUrl.contains("audio=virtual-audio-capturer")) {
        av_dict_set(options, "sample_size", "16", 0);
        av_dict_set(options, "channels", "2", 0);
        av_dict_set(options, "sample_rate", "44100", 0);
        av_dict_set(options, "audio_buffer_size", "40", 0);
    }
}

void FFmpegHelper::initOption(AVDictionary **options, const QString &bufferSize, int frameRate) {
    //设置分辨率(约定10x10分辨率表示所有屏幕/多屏幕下会合并到一个屏幕采集)
    if (bufferSize != "0x0" && bufferSize != "10x10") {
        av_dict_set(options, "video_size", bufferSize.toUtf8().constData(), 0);
    }

    //设置帧率
    if (frameRate > 0) {
        av_dict_set(options, "framerate", QString::number(frameRate).toUtf8().constData(), 0);
    }
}

void FFmpegHelper::initOption(AVDictionary **options, int offsetX, int offsetY, QString &mediaUrl) {
    //av_dict_set(options, "draw_mouse", "0", 0);
    //av_dict_set(options, "show_region", "1", 0);

#ifdef Q_OS_WIN
    //windows上通过设置offset_x/offset_y来设置偏移值
    av_dict_set(options, "offset_x", QString::number(offsetX).toUtf8().constData(), 0);
    av_dict_set(options, "offset_y", QString::number(offsetY).toUtf8().constData(), 0);
#else
    //linux系统通过设备名称带上偏移值传入
    mediaUrl += QString("+%1,%2").arg(offsetX).arg(offsetY);
#endif
}

void FFmpegHelper::setVideoCodecName(AVFormatContext *formatCtx, const QString &videoCodecName) {
    //强制指定本地摄像头采集解码为mjpeg或者h264/找遍了网络原来是这样设置才起作用
    if (videoCodecName == "mjpeg") {
        formatCtx->video_codec_id = AV_CODEC_ID_MJPEG;
    } else if (videoCodecName == "h264") {
        formatCtx->video_codec_id = AV_CODEC_ID_H264;
    } else if (videoCodecName == "h265") {
        formatCtx->video_codec_id = AV_CODEC_ID_H265;
    } else if (videoCodecName.startsWith("raw")) {
        formatCtx->video_codec_id = AV_CODEC_ID_RAWVIDEO;
    }
}

QString FFmpegHelper::getFormatString(int format, bool video) {
    if (format < 0) {
        return "none";
    }

    QString name;
    if (video) {
        name = av_get_pix_fmt_name((AVPixelFormat) format);
    } else {
        name = av_get_sample_fmt_name((AVSampleFormat) format);
    }
    return name;
}

QString FFmpegHelper::getPixFormatString(int format) {
    return getFormatString(format, true);
}

QString FFmpegHelper::getSampleFormatString(int format) {
    return getFormatString(format, false);
}

qint64 FFmpegHelper::getPts(AVPacket *packet) {
    //有些文件(比如asf文件)取不到pts需要矫正
    qint64 pts = 0;
    if (packet->dts == AV_NOPTS_VALUE && packet->pts && packet->pts != AV_NOPTS_VALUE) {
        pts = packet->pts;
    } else if (packet->dts != AV_NOPTS_VALUE) {
        pts = packet->dts;
    }
    return pts;
}

double FFmpegHelper::getPtsTime(AVFormatContext *formatCtx, AVPacket *packet) {
    AVStream *stream = formatCtx->streams[packet->stream_index];
    qint64 pts = getPts(packet);
    //qDebug() << TIMEMS << pts << packet->pos << packet->duration;
    //double time = pts * av_q2d(stream->time_base) * 1000;
    double time = pts * 1.0 * av_q2d(stream->time_base) * AV_TIME_BASE;
    //double time = pts * 1.0 * stream->time_base.num / stream->time_base.den * AV_TIME_BASE;
    return time;
}

double FFmpegHelper::getDurationTime(AVFormatContext *formatCtx, AVPacket *packet) {
    AVStream *stream = formatCtx->streams[packet->stream_index];
    double time = packet->duration * av_q2d(stream->time_base);
    return time;
}

qint64 FFmpegHelper::getDelayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime) {
    AVRational time_base = formatCtx->streams[packet->stream_index]->time_base;
    AVRational time_base_q = {1, AV_TIME_BASE};//AV_TIME_BASE_Q
    qint64 pts = getPts(packet);
    qint64 pts_time = av_rescale_q(pts, time_base, time_base_q);
    qint64 now_time = av_gettime() - startTime;
    qint64 offset_time = pts_time - now_time;
    return offset_time;
}

void FFmpegHelper::delayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime) {
    qint64 offset_time = getDelayTime(formatCtx, packet, startTime);
    //qDebug() << TIMEMS << offset_time << packet->pts << packet->dts;
    if (offset_time > 0 && offset_time < 1 * 1000 * 1000) {
        av_usleep(offset_time);
    }
}

qreal FFmpegHelper::getFrameRate(AVStream *stream, const QString &formatName) {
    //帧率优先取平均帧率
    double fps = av_q2d(stream->avg_frame_rate);
    double fps2 = av_q2d(stream->r_frame_rate);
    //qDebug() << TIMEMS << formatName << fps << fps2;
    if (fps <= 0 || fps > 60 || qIsNaN(fps)) {
        fps = fps2;
    }

    //有时候平均帧率是25而基本帧率是24.9127这种则取基本帧率
    if (qAbs(fps - fps2) < 5) {
        //fps = fps2;
    }

    //海康摄像头265设置的帧率25但是识别到的是50
    if (formatName == "rtsp" && fps2 > 0) {
        fps = qMin(fps, fps2);
    }

    //264/265裸流纠正下fps
    if (fps > 120 && (formatName == "h264" || formatName == "hevc")) {
        fps = 25;
    }

    return fps;
}

int
FFmpegHelper::initSwrContext(SwrContext **swrCtx, int out_channels, AVSampleFormat out_sample_fmt, int out_sample_rate,
                             int in_channels, AVSampleFormat in_sample_fmt, int in_sample_rate) {
    int result = 0;
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    qint64 in_ch_layout = av_get_default_channel_layout(in_channels);
    qint64 out_ch_layout = av_get_default_channel_layout(out_channels);
    (*swrCtx) = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt, out_sample_rate, in_ch_layout, in_sample_fmt, in_sample_rate, 0, NULL);
    result = swr_init((*swrCtx));
#else
    AVChannelLayout in_ch_layout, out_ch_layout;
    av_channel_layout_default(&in_ch_layout, in_channels);
    av_channel_layout_default(&out_ch_layout, out_channels);
    result = swr_alloc_set_opts2(swrCtx, &out_ch_layout, out_sample_fmt, out_sample_rate, &in_ch_layout, in_sample_fmt,
                                 in_sample_rate, 0, NULL);
    if (result >= 0) {
        result = swr_init((*swrCtx));
    }
#endif
    return result;
}

void FFmpegHelper::initFrame(AVFrame *frame, AVCodecContext *codecCtx) {
    frame->format = codecCtx->sample_fmt;
    frame->nb_samples = codecCtx->frame_size;
    frame->sample_rate = codecCtx->sample_rate;
    FFmpegHelper::initChannel(frame, codecCtx);
}

int FFmpegHelper::getChannel(AVCodecContext *codecCtx) {
    int channels = 0;
    //有些文件的channel_layout居然是空的需要修复下
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    channels = codecCtx->channels;
    codecCtx->channel_layout = av_get_default_channel_layout(channels);
#else
    channels = codecCtx->ch_layout.nb_channels;
    av_channel_layout_default(&codecCtx->ch_layout, channels);
#endif
    return channels;
}

void FFmpegHelper::initChannel(AVFrame *frame, int channels) {
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    frame->channels = channels;
    frame->channel_layout = av_get_default_channel_layout(channels);
#else
    frame->ch_layout.nb_channels = channels;
    av_channel_layout_default(&frame->ch_layout, channels);
#endif
}

void FFmpegHelper::initChannel(AVCodecContext *codecCtx, int channels) {
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    codecCtx->channels = channels;
    codecCtx->channel_layout = av_get_default_channel_layout(channels);
#else
    codecCtx->ch_layout.nb_channels = channels;
    av_channel_layout_default(&codecCtx->ch_layout, channels);
#endif
}

void FFmpegHelper::initChannel(AVFrame *frame, AVCodecContext *codecCtx) {
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    frame->channels = codecCtx->channels;
    frame->channel_layout = codecCtx->channel_layout;
#else
    frame->ch_layout.nb_channels = codecCtx->ch_layout.nb_channels;
    frame->ch_layout = codecCtx->ch_layout;
#endif
}

int FFmpegHelper::getRotate(AVStream *stream) {
    int rotate = 0;
    //测试发现ffmpeg2不支持旋转滤镜
#if (FFMPEG_VERSION_MAJOR < 3)
    return rotate;
#endif

#if (FFMPEG_VERSION_MAJOR < 5)
    AVDictionaryEntry *tag = NULL;
    tag = av_dict_get(stream->metadata, "rotate", NULL, 0);
    if (tag) {
        rotate = atoi(tag->value);
    }
#else
    //从ffplay源码中找到的方法
    double theta = 0;
    qint32 *displaymatrix = NULL;
#if (FFMPEG_VERSION_MAJOR < 6)
    displaymatrix = (qint32 *)av_stream_get_side_data(stream, AV_PKT_DATA_DISPLAYMATRIX, NULL);
#else
    const AVPacketSideData *sd = av_packet_side_data_get(stream->codecpar->coded_side_data,
                                                         stream->codecpar->nb_coded_side_data,
                                                         AV_PKT_DATA_DISPLAYMATRIX);
    if (sd) {
        displaymatrix = (qint32 *) sd->data;
    }
#endif
    if (displaymatrix) {
        theta = -av_display_rotation_get(displaymatrix);
        theta -= 360 * floor(theta / 360 + 0.9 / 360);
        rotate = theta;
    }
#endif
    return rotate;
}

void FFmpegHelper::setRotate(AVStream *stream, int rotate) {
#if (FFMPEG_VERSION_MAJOR < 5)
    av_dict_set(&stream->metadata, "rotate", QString::number(rotate).toUtf8().constData(), 0);
#else
#if (FFMPEG_VERSION_MAJOR < 6)
    qint32 *sidedata = (qint32 *)av_stream_new_side_data(stream, AV_PKT_DATA_DISPLAYMATRIX, sizeof(qint32) * 9);
    if (sidedata) {
        av_display_rotation_set(sidedata, rotate);
    }
#else
    //查阅代码得知ffmpeg6开始会自动设置旋转角度
    //qint32 *sidedata = (qint32 *)av_packet_side_data_new(&stream->codecpar->coded_side_data, &stream->codecpar->nb_coded_side_data, AV_PKT_DATA_DISPLAYMATRIX, sizeof(qint32) * 9, 0);
#endif
#endif
}

AVCodecID FFmpegHelper::getCodecId(AVStream *stream) {
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return AVCodecID(id);
}

QString FFmpegHelper::getCodecName(AVStream *stream) {
    AVCodecID id = getCodecId(stream);
    return (id == AV_CODEC_ID_NONE ? "none" : avcodec_descriptor_get(id)->name);
}

qint64 FFmpegHelper::getBitRate(AVStream *stream) {
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return bitrate;
}

int FFmpegHelper::getFormat(AVStream *stream) {
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return format;
}

AVMediaType FFmpegHelper::getMediaType(AVStream *stream) {
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return AVMediaType(type);
}

void FFmpegHelper::getResolution(AVStream *stream, int &width, int &height) {
    qint64 bitrate;
    int id, type, format, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
}

void FFmpegHelper::getAudioInfo(AVStream *stream, int &sampleRate, int &channelCount, int &profile) {
    qint64 bitrate;
    int id, type, format, width, height;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
}

void
FFmpegHelper::getStreamInfo(AVStream *stream, int &id, int &type, int &format, qint64 &bitrate, int &width, int &height,
                            int &sampleRate, int &channelCount, int &profile) {
#if (FFMPEG_VERSION_MAJOR < 3)
    type = stream->codec->codec_type;
    if (type == AVMEDIA_TYPE_VIDEO) {
        format = stream->codec->pix_fmt;
    } else if (type == AVMEDIA_TYPE_AUDIO) {
        format = stream->codec->sample_fmt;
    }

    id = stream->codec->codec_id;
    bitrate = stream->codec->bit_rate;
    width = stream->codec->width;
    height = stream->codec->height;
    sampleRate = stream->codec->sample_rate;
    profile = stream->codec->profile;
    channelCount = stream->codec->channels;
#else
    id = stream->codecpar->codec_id;
    type = stream->codecpar->codec_type;
    format = stream->codecpar->format;
    bitrate = stream->codecpar->bit_rate;
    width = stream->codecpar->width;
    height = stream->codecpar->height;
    sampleRate = stream->codecpar->sample_rate;
    profile = stream->codecpar->profile;

#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    channelCount = stream->codecpar->channels;
#else
    channelCount = stream->codecpar->ch_layout.nb_channels;
#endif
#endif
}

QString FFmpegHelper::getPlayUrl(const QString &mediaUrl) {
    //网络视频文件带空格要转换
    QString url = mediaUrl;
    if ((url.startsWith("http://") || url.startsWith("https://")) && url.contains(" ")) {
        url = QUrl(url).toEncoded();
    }
    return url;
}

QString FFmpegHelper::getUrl(AVFormatContext *formatCtx) {
#if (FFMPEG_VERSION_MAJOR < 4)
    return formatCtx->filename;
#else
    return formatCtx->url;
#endif
}

void FFmpegHelper::getTracks(AVFormatContext *formatCtx, QList<int> &audioTracks, QList<int> &videoTracks) {
    //获取音视频轨道信息(一般有一个音频或者一个视频/ts节目文件可能有多个)
    audioTracks.clear();
    videoTracks.clear();
    int count = formatCtx->nb_streams;
    for (int i = 0; i < count; ++i) {
        AVMediaType type = FFmpegHelper::getMediaType(formatCtx->streams[i]);
        if (type == AVMEDIA_TYPE_AUDIO) {
            audioTracks << i;
        } else if (type == AVMEDIA_TYPE_VIDEO) {
            videoTracks << i;
        }
    }
}

int FFmpegHelper::copyContext(AVStream *streamIn, AVStream *streamOut) {
    int result = -1;
    //设置 codec_tag = 0 这个很关键(不加保存的数据可能不正确)
#if (FFMPEG_VERSION_MAJOR < 3)
    result = avcodec_copy_context(streamOut->codec, streamIn->codec);
    streamOut->codec->codec_tag = 0;
#else
    result = avcodec_parameters_copy(streamOut->codecpar, streamIn->codecpar);
    streamOut->codecpar->codec_tag = 0;
#endif
    return result;
}

int FFmpegHelper::copyContext(AVCodecContext *avctx, AVStream *stream, bool from) {
    int result = -1;
#if (FFMPEG_VERSION_MAJOR < 3)
    if (from) {
        result = avcodec_copy_context(stream->codec, avctx);
    } else {
        result = avcodec_copy_context(avctx, stream->codec);
    }
#else
    if (from) {
        result = avcodec_parameters_from_context(stream->codecpar, avctx);
    } else {
        result = avcodec_parameters_to_context(avctx, stream->codecpar);
    }
#endif
    return result;
}

bool FFmpegHelper::checkPacketKey(AVPacket *packet) {
    return (packet->flags & AV_PKT_FLAG_KEY);
}

AVPacket *FFmpegHelper::creatPacket(AVPacket *packet) {
    AVPacket *pkt;
#if (FFMPEG_VERSION_MAJOR < 3)
    //旧方法(废弃使用)
    if (packet) {
        pkt = new AVPacket;
        av_init_packet(pkt);
        av_copy_packet(pkt, packet);
    } else {
        pkt = new AVPacket;
        av_new_packet(pkt, sizeof(AVPacket));
    }
#else
    //新方法(推荐使用)
    if (packet) {
        pkt = av_packet_clone(packet);
    } else {
        pkt = av_packet_alloc();
    }
#endif
    return pkt;
}

void FFmpegHelper::freeFrame(AVFrame *frame) {
    if (frame) {
        av_frame_free(&frame);
        av_free(frame->data);
    }
}

void FFmpegHelper::freeFrame2(AVFrame **frame) {
    if (*frame) {
        av_frame_free(frame);
        av_free((*frame)->data);
    }
}

void FFmpegHelper::freePacket(AVPacket *packet) {
    if (packet) {
#if (FFMPEG_VERSION_MAJOR < 3)
        av_packet_unref(packet);
        av_freep(packet);
#else
        //查阅代码得知 av_packet_free 就是执行上面两行
        av_packet_free(&packet);
#endif
    }
}

void FFmpegHelper::freePacket2(AVPacket **packet) {
    if (*packet) {
        av_packet_unref(*packet);
        av_freep(packet);
    }
}
