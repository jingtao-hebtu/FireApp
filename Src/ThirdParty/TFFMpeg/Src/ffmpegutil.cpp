#include "ffmpegutil.h"
#include "widgethead.h"
#include "qelapsedtimer.h"
#include "qstringlist.h"

//http://ruiy.leanote.com/post/c-使用ffmpeg
void FFmpegUtil::getCodec(QStringList &listEncoderVideo, QStringList &listEncoderAudio, QStringList &listEncoderOther,
                          QStringList &listDecoderVideo, QStringList &listDecoderAudio, QStringList &listDecoderOther) {
#if (FFMPEG_VERSION_MAJOR < 4)
    AVCodec *coder = av_codec_next(NULL);
    while (coder != NULL) {
#else
    void *opaque = NULL;
    const AVCodec *coder;
    while ((coder = av_codec_iterate(&opaque)) != NULL) {
#endif
        QString name = QString("%1").arg(coder->name);
        if (av_codec_is_encoder(coder)) {
            if (coder->type == AVMEDIA_TYPE_VIDEO) {
                listEncoderVideo << name;
            } else if (coder->type == AVMEDIA_TYPE_AUDIO) {
                listEncoderAudio << name;
            } else {
                listEncoderOther << name;
            }
        } else if (av_codec_is_decoder(coder)) {
            if (coder->type == AVMEDIA_TYPE_VIDEO) {
                listDecoderVideo << name;
            } else if (coder->type == AVMEDIA_TYPE_AUDIO) {
                listDecoderAudio << name;
            } else {
                listDecoderOther << name;
            }
        }
#if (FFMPEG_VERSION_MAJOR < 4)
        coder = coder->next;
#endif
    }

    qDebug() << TIMEMS << QString("视频编码 -> %1").arg(listEncoderVideo.join("/"));
    qDebug() << TIMEMS << QString("音频编码 -> %1").arg(listEncoderAudio.join("/"));
    qDebug() << TIMEMS << QString("其他编码 -> %1").arg(listEncoderOther.join("/"));

    qDebug() << TIMEMS << QString("视频解码 -> %1").arg(listDecoderVideo.join("/"));
    qDebug() << TIMEMS << QString("音频解码 -> %1").arg(listDecoderAudio.join("/"));
    qDebug() << TIMEMS << QString("其他解码 -> %1").arg(listDecoderOther.join("/"));
}

void FFmpegUtil::getProtocol(QStringList &listProtocolsIn, QStringList &listProtocolsOut) {
    struct URLProtocol *protocol = NULL;
    struct URLProtocol **protocol2 = &protocol;

    avio_enum_protocols((void **) protocol2, 0);
    while ((*protocol2)) {
        listProtocolsIn << avio_enum_protocols((void **) protocol2, 0);
    }

    protocol = NULL;
    avio_enum_protocols((void **) protocol2, 1);
    while ((*protocol2)) {
        listProtocolsOut << avio_enum_protocols((void **) protocol2, 1);
    }

    qDebug() << TIMEMS << QString("输入协议 -> %1").arg(listProtocolsIn.join("/"));
    qDebug() << TIMEMS << QString("输出协议 -> %1").arg(listProtocolsOut.join("/"));
}

QString FFmpegUtil::getConfiguration() {
    QString result;
    result = avformat_configuration();
    qDebug() << TIMEMS << QString("编译参数 -> %1").arg(result);
    return result;
}

QStringList FFmpegUtil::getHardware() {
    //硬解类型名称 ffmpeg -hwaccels
    QStringList listHardware;
    //其实ffmpeg2/ffmpeg3也有但是不完整一般建议3以上
#if (FFMPEG_VERSION_MAJOR > 3)
    enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
        listHardware << av_hwdevice_get_type_name(type);
    }
#endif

    qDebug() << TIMEMS << QString("硬解类型 -> %1").arg(listHardware.join("/"));
    return listHardware;
}

void FFmpegUtil::debugAll(bool codec, bool protocol, bool other) {
    if (codec) {
        QStringList listEncoderVideo, listEncoderAudio, listEncoderOther, listDecoderVideo, listDecoderAudio, listDecoderOther;
        FFmpegUtil::getCodec(listEncoderVideo, listEncoderAudio, listEncoderOther, listDecoderVideo, listDecoderAudio,
                             listDecoderOther);
    }

    if (protocol) {
        QStringList listProtocolsIn, listProtocolsOut;
        FFmpegUtil::getProtocol(listProtocolsIn, listProtocolsOut);
    }

    if (other) {
        FFmpegUtil::getConfiguration();
        FFmpegUtil::getHardware();
    }
}

void FFmpegUtil::debugPara(AVCodecContext *codecCtx) {
    QStringList list;
    list << QString("width: %1").arg(codecCtx->width);
    list << QString("height: %1").arg(codecCtx->height);
    list << QString("bit_rate: %1").arg(codecCtx->bit_rate);
    list << QString("level: %1").arg(codecCtx->level);
    list << QString("gop_size: %1").arg(codecCtx->gop_size);
    list << QString("max_b_frames: %1").arg(codecCtx->max_b_frames);
    list << QString("pix_fmt: %1").arg(FFmpegHelper::getPixFormatString(codecCtx->pix_fmt));
    list << QString("sample_fmt: %1").arg(FFmpegHelper::getSampleFormatString(codecCtx->sample_fmt));
    list << QString("codec_id: %1").arg(codecCtx->codec_id);
    list << QString("profile: %1").arg(codecCtx->profile);
    list << QString("qmin: %1").arg(codecCtx->qmin);
    list << QString("qmax: %1").arg(codecCtx->qmax);
    list << QString("me_range: %1").arg(codecCtx->me_range);
    list << QString("max_qdiff: %1").arg(codecCtx->max_qdiff);
    list << QString("qcompress: %1").arg(codecCtx->qcompress);
    qDebug() << TIMEMS << QString("参数信息 -> %1").arg(list.join("  "));
}

void FFmpegUtil::debugPara(AVStream *stream) {
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount;

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
    channelCount = stream->codec->channels;
#else
    id = stream->codecpar->codec_id;
    type = stream->codecpar->codec_type;
    format = stream->codecpar->format;
    bitrate = stream->codecpar->bit_rate;
    width = stream->codecpar->width;
    height = stream->codecpar->height;
    sampleRate = stream->codecpar->sample_rate;
#if (FFMPEG_VERSION_MAJOR < FFMPEG_VERSION_NEW)
    channelCount = stream->codecpar->channels;
#else
    channelCount = stream->codecpar->ch_layout.nb_channels;
#endif
#endif

    QStringList list;
    list << QString("id: %1").arg(id);
    list << QString("type: %1").arg(type);
    list << QString("format: %1").arg(format);
    list << QString("bitrate: %1").arg(bitrate);
    list << QString("width: %1").arg(width);
    list << QString("height: %1").arg(height);
    list << QString("sampleRate: %1").arg(sampleRate);
    list << QString("channelCount: %1").arg(channelCount);
    qDebug() << TIMEMS << QString("参数信息 -> %1").arg(list.join("  "));
}

void FFmpegUtil::rotateFrame(int rotate, AVFrame *frameSrc, AVFrame *frameDst) {
    int n = 0;
    int pos = 0;
    int w = frameSrc->width;
    int h = frameSrc->height;
    int hw = w / 2;
    int hh = h / 2;

    //根据不同的旋转角度拷贝yuv分量
    if (rotate == 90) {
        n = 0;
        int size = w * h;
        for (int j = 0; j < w; j++) {
            pos = size;
            for (int i = h - 1; i >= 0; i--) {
                pos -= w;
                frameDst->data[0][n++] = frameSrc->data[0][pos + j];
            }
        }

        n = 0;
        int hsize = size / 4;
        for (int j = 0; j < hw; j++) {
            pos = hsize;
            for (int i = hh - 1; i >= 0; i--) {
                pos -= hw;
                frameDst->data[1][n] = frameSrc->data[1][pos + j];
                frameDst->data[2][n] = frameSrc->data[2][pos + j];
                n++;
            }
        }
    } else if (rotate == 180) {
        n = 0;
        pos = w * h;
        for (int i = 0; i < h; i++) {
            pos -= w;
            for (int j = 0; j < w; j++) {
                frameDst->data[0][n++] = frameSrc->data[0][pos + j];
            }
        }

        n = 0;
        pos = w * h / 4;
        for (int i = 0; i < hh; i++) {
            pos -= hw;
            for (int j = 0; j < hw; j++) {
                frameDst->data[1][n] = frameSrc->data[1][pos + j];
                frameDst->data[2][n] = frameSrc->data[2][pos + j];
                n++;
            }
        }
    } else if (rotate == 270) {
        n = 0;
        for (int i = w - 1; i >= 0; i--) {
            pos = 0;
            for (int j = 0; j < h; j++) {
                frameDst->data[0][n++] = frameSrc->data[0][pos + i];
                pos += w;
            }
        }

        n = 0;
        for (int i = hw - 1; i >= 0; i--) {
            pos = 0;
            for (int j = 0; j < hh; j++) {
                frameDst->data[1][n] = frameSrc->data[1][pos + i];
                frameDst->data[2][n] = frameSrc->data[2][pos + i];
                pos += hw;
                n++;
            }
        }
    }

    //设置尺寸
    if (rotate == 90 || rotate == 270) {
        frameDst->linesize[0] = h;
        frameDst->linesize[1] = h / 2;
        frameDst->linesize[2] = h / 2;
        frameDst->width = h;
        frameDst->height = w;
    } else {
        frameDst->linesize[0] = w;
        frameDst->linesize[1] = w / 2;
        frameDst->linesize[2] = w / 2;
        frameDst->width = w;
        frameDst->height = h;
    }

    //设置其他参数
    frameDst->pts = frameSrc->pts;
    frameDst->pkt_dts = frameSrc->pkt_dts;
    frameDst->format = frameSrc->format;
    frameDst->flags = frameSrc->flags;
}

qint64 FFmpegUtil::getDuration(const QString &fileName, quint64 *useTime) {
    qint64 duration = 0;
    QElapsedTimer timer;
    timer.start();

    //打开文件
    AVFormatContext *formatCtx = NULL;
    QString url = FFmpegHelper::getPlayUrl(fileName);
    avformat_open_input(&formatCtx, url.toUtf8().constData(), NULL, NULL);
    avformat_find_stream_info(formatCtx, NULL);
    int videoIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        return duration;
    }

    AVStream *videoStream = formatCtx->streams[videoIndex];
    int frameCount = videoStream->nb_frames;
    double fps = FFmpegHelper::getFrameRate(videoStream);

    //没有获取到帧数则从头读取到尾部统计
    AVPacket *packet = FFmpegHelper::creatPacket(NULL);
    if (frameCount <= 0) {
        while (av_read_frame(formatCtx, packet) >= 0) {
            if (packet->stream_index == videoIndex) {
                frameCount++;
            }
            av_packet_unref(packet);
        }
    }

    //计算总时长
    duration = (double) frameCount / fps;
    QString type = (videoStream->nb_frames == 0 ? "循环计算" : "文件信息");
    //qDebug() << TIMEMS << QString("文件时长: %1s 统计用时: %2ms 统计方式: %3").arg(duration).arg(timer.elapsed()).arg(type);
    if (useTime) {
        *useTime = timer.elapsed();
    }

    FFmpegHelper::freePacket(packet);
    avformat_close_input(&formatCtx);
    avformat_free_context(formatCtx);
    return duration;
}

bool FFmpegUtil::checkEncry(const QString &fileName) {
    //https://blog.csdn.net/qq_36924305/article/details/134425622
    static QByteArray key = QByteArray(
            "\x73\x63\x68\x6D\x00\x00\x00\x00\x63\x65\x6E\x63\x00\x01\x00\x00\x00\x00\x00\x28\x73\x63\x68\x69\x00\x00\x00\x20\x74\x65\x6E\x63");
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        return data.contains(key);
    }

    return false;
}

bool FFmpegUtil::hasB(const QString &fileName, int maxFrame) {
    bool b = false;
    int ret = -1;
    int frameCount = 0;
    int videoIndex = -1;

    AVFormatContext *formatCtx = NULL;
    AVStream *videoStream = NULL;
    AVCodecx *videoCodec = NULL;
    AVCodecContext *videoCodecCtx = NULL;
    AVPacket *packet = NULL;
    AVFrame *frame = NULL;

    //打开文件
    QString url = FFmpegHelper::getPlayUrl(fileName);
    ret = avformat_open_input(&formatCtx, url.toUtf8().constData(), NULL, NULL);
    if (ret < 0) {
        goto end;
    }

    ret = avformat_find_stream_info(formatCtx, NULL);
    if (ret < 0) {
        goto end;
    }

    videoIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoCodec, 0);
    if (videoIndex < 0) {
        goto end;
    }

    videoStream = formatCtx->streams[videoIndex];
    videoCodecCtx = avcodec_alloc_context3(NULL);
    //videoCodec = avcodec_find_decoder(FFmpegHelper::getCodecId(videoStream));
    FFmpegHelper::copyContext(videoCodecCtx, videoStream, false);
    if ((ret = avcodec_open2(videoCodecCtx, videoCodec, NULL)) < 0) {
        goto end;
    }

    //后面发现有个内置的方法
    return videoCodecCtx->has_b_frames;

    //取出前XX帧解码出来判断有没有B帧
    packet = FFmpegHelper::creatPacket(NULL);
    frame = av_frame_alloc();
    while (av_read_frame(formatCtx, packet) >= 0) {
#if (FFMPEG_VERSION_MAJOR < 3)
        maxFrame = 30;
        if (avcodec_decode_video2(videoCodecCtx, frame, &ret, packet) < 0) {
            continue;
        }
#else
        if (avcodec_send_packet(videoCodecCtx, packet) < 0) {
            continue;
        }
        if (avcodec_receive_frame(videoCodecCtx, frame) < 0) {
            continue;
        }
#endif

        if (frame->pict_type == 3) {
            b = true;
            goto end;
        }

        frameCount++;
        if (frameCount == maxFrame) {
            goto end;
        }
    }

    end:
    if (packet) {
        FFmpegHelper::freePacket(packet);
    }
    if (frame) {
        FFmpegHelper::freeFrame(frame);
    }
    if (videoCodecCtx) {
        avcodec_free_context(&videoCodecCtx);
    }
    if (formatCtx) {
        avformat_close_input(&formatCtx);
        avformat_free_context(formatCtx);
    }
    return b;
}

int FFmpegUtil::binToDecimal(const QString &bin) {
    return bin.toInt(NULL, 2);
}

QString FFmpegUtil::decimalToBin(int decimal) {
    QString bin = QString::number(decimal, 2);
    quint8 len = bin.length();
    if (len <= 8) {
        for (int i = 0; i < 8 - len; ++i) {
            bin = "0" + bin;
        }
    }
    return bin;
}

int FFmpegUtil::dataToInt(quint8 data1, quint8 data2) {
    int result = 0;
    result = data2 & 0x000000ff;
    result |= ((data1 << 8) & 0x0000ff00);
    if (result >= 32768) {
        result = result - 65536;
    }
    return result;
}

//http://www.taodudu.cc/news/show-983932.html
void FFmpegUtil::getExtraData(AVCodecContext *codecCtx) {
    QString codecName = codecCtx->codec_descriptor->name;
    if (codecName != "h264") {
        return;
    }

    QStringList list;
    quint8 *data = codecCtx->extradata;
    int size = codecCtx->extradata_size;
    for (int i = 0; i < size; ++i) {
        list << QString::number(data[i]);
    }

    //至少要有11个字节
    qDebug() << "extradata" << size << list;
    if (size < 11) {
        return;
    }

    //版本和其他信息
    int version = data[0];
    int avcprofile = data[1];
    int profile = data[2];
    int avclevel = data[3];

    //NALU长度 FF=11111111 取后两位11=4 表示长度4
    QString bin = decimalToBin(data[4]).right(2);
    int nalulen = 1 + binToDecimal("000000" + bin);

    //sps个数 低5位 一般是1个
    bin = decimalToBin(data[5]).right(5);
    int spsnum = binToDecimal("000" + bin);
    if (spsnum == 0) {
        return;
    }

    //sps长度
    int spsindex = 5;
    int spslen = dataToInt(data[spsindex + 1], data[spsindex + 2]);
    //sps内容
    QByteArray spsdata;
    for (int i = 0; i < spslen; ++i) {
        spsdata.append(data[spsindex + 3 + i]);
    }

    //pps个数
    int ppsindex = 8 + spslen;
    int ppsnum = data[ppsindex];
    if (ppsnum == 0) {
        return;
    }

    //pps长度
    int ppslen = dataToInt(data[ppsindex + 1], data[ppsindex + 2]);
    //pps内容
    QByteArray ppsdata;
    for (int i = 0; i < ppslen; ++i) {
        ppsdata.append(data[ppsindex + 3 + i]);
    }

    qDebug() << "extradata" << nalulen << spsnum << spslen << spsdata.toHex() << ppsnum << ppslen << ppsdata.toHex();
}

void FFmpegUtil::test(const QString &fileName) {
    FFmpegHelper::initLib();

}

QString FFmpegUtil::dataToString(char *data, qint64 len) {
    return dataToStringList(data, len).join(",");
}

QStringList FFmpegUtil::dataToStringList(char *data, qint64 len) {
    QStringList list;
    for (int i = 0; i < len; ++i) {
        list << QString::number(data[i]);
    }
    return list;
}
