#include "ffmpegsave.h"
#include "ffmpegsavehelper.h"
#include "ffmpeghelper.h"
#include "widgethelper.h"
#include "urlhelper.h"
#include "deviceinfohelper.h"

FFmpegSave::FFmpegSave(QObject *parent) : AbstractSaveThread(parent) {
    this->reset();

    mp4ToAnnexB = false;
    videoEncode = false;
    audioEncode = false;

    encodeSpeed = 1;
    encodeAudio = EncodeAudio_Auto;
    encodeVideo = EncodeVideo_None;
    encodeVideoFps = 0;
    encodeVideoRatio = 1;
    encodeVideoScale = "1";

    sendPacket = false;
    onlySendPacket = false;

    connectTimeout = 1000;

    formatCtx = NULL;
    bsfCtx = NULL;

    videoStreamIn = NULL;
    audioStreamIn = NULL;
    videoCodecCtx = NULL;
    audioCodecCtx = NULL;

    videoFrame = NULL;
    audioFrame = NULL;
    videoSwsCtx = NULL;
    audioSwrCtx = NULL;

    videoPacket = NULL;
    audioPacket = NULL;
}

void FFmpegSave::getMediaInfo() {
    //获取输入视频流的信息
    if (videoStreamIn) {
        videoIndexIn = videoStreamIn->index;
        this->videoCodecName = FFmpegHelper::getCodecName(videoStreamIn);
        FFmpegHelper::getResolution(videoStreamIn, videoWidth, videoHeight);
        //校验分辨率/比如采集指定窗口标题的很可能是奇数分辨率
        DeviceInfoHelper::checkSize(videoWidth, videoHeight);
        frameRate = FFmpegHelper::getFrameRate(videoStreamIn);
        rotate = FFmpegHelper::getRotate(videoStreamIn);
        WidgetHelper::rotateSize(rotate, videoWidth, videoHeight);
    } else {
        needVideo = false;
        videoCodecName = "none";
    }

    //获取输入音频流的信息
    if (audioStreamIn) {
        audioIndexIn = audioStreamIn->index;
        this->audioCodecName = FFmpegHelper::getCodecName(audioStreamIn);
        FFmpegHelper::getAudioInfo(audioStreamIn, sampleRate, channelCount, profile);

        //标记过重采样则对应参数从弱属性获取
        if (this->property("resample").toBool()) {
            sampleRate = this->property("sampleRate").toInt();
            channelCount = this->property("channelCount").toInt();
        }
    } else {
        needAudio = false;
        audioCodecName = "none";
    }
}

void FFmpegSave::checkEncode() {
    //如果是保存264裸流则不需要音频
    if (saveVideoType == SaveVideoType_Stream) {
        needAudio = false;
    }

    //可以选择只存储视频或者音频
    //this->needVideo = false;
    //this->needAudio = false;

    //自动调整是否需要重新编码视音频
    FFmpegSaveHelper::checkEncode(this, videoCodecName, audioCodecName, videoEncode, audioEncode, encodeAudio,
                                  needAudio);
    //校验文件名/比如设置了强转265则将文件名拓展名改成265
    FFmpegSaveHelper::checkFileName(saveVideoType, encodeVideo, videoCodecName, fileName);

    //限制倍速情况下暂时屏蔽音频
    if (encodeSpeed != 1) {
        this->needAudio = false;
    }

    //没有启用对应音视频保存则更新一些参数(方便打印线程那边/依赖这个变量判断)
    if (!needVideo) {
        videoWidth = 0;
        videoEncode = false;
        videoIndexIn = -1;
    }
    if (!needAudio) {
        sampleRate = 0;
        audioEncode = false;
        audioIndexIn = -1;
    }

    //如果设置过帧率则最终按照设置的
    if (encodeVideoFps > 0) {
        this->frameRate = encodeVideoFps;
    }

    //打印编码信息
    QString msg1 = QString("%1/%2/%3").arg(needVideo ? "启用" : "禁用").arg(videoEncode ? "编码" : "原始").arg(
            videoCodecName);
    QString msg2 = QString("%1/%2/%3").arg(needAudio ? "启用" : "禁用").arg(audioEncode ? "编码" : "原始").arg(
            audioCodecName);
    debug(0, "编码信息", QString("视频: %1 音频: %2").arg(msg1).arg(msg2));
}

bool FFmpegSave::initVideoCtx() {
    //没启用视频编码或者不需要视频则不继续
    if (!videoEncode || !needVideo) {
        return true;
    }

    //初始化视频编码器
    AVCodecx *videoCodec;
    FFmpegSaveHelper::initVideoCodec(&videoCodec, FFmpegHelper::getCodecId(videoStreamIn), encodeVideo, fileName);
    if (!videoCodec) {
        debug(0, "视频编码", "avcodec_find_encoder");
        return false;
    }

    //创建视频编码器上下文
    videoCodecCtx = avcodec_alloc_context3(videoCodec);
    if (!videoCodecCtx) {
        debug(0, "视频编码", "avcodec_alloc_context3");
        return false;
    }

    //获取缩放后的尺寸
    int width, height;
    FFmpegSaveHelper::getVideoSize(encodeVideoScale, videoWidth, videoHeight, width, height);

    //初始化编码器上下文
    FFmpegSaveHelper::initVideoCodecCtx(videoCodecCtx, mediaType, width, height, frameRate, encodeVideoRatio);
    if (saveVideoType == SaveVideoType_Mp4) {
        videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        //videoCodecCtx->flags |= (AV_CODEC_FLAG_GLOBAL_HEADER | AV_CODEC_FLAG_LOW_DELAY);
    }

    //打开视频编码器
    int result = avcodec_open2(videoCodecCtx, videoCodec, NULL);
    if (result < 0) {
        debug(result, "视频编码", "avcodec_open2");
        return false;
    }

    //创建编码用临时包
    videoPacket = FFmpegHelper::creatPacket(NULL);
    //设置了视频缩放则转换
    bool convert = (encodeVideoScale != "1");
    if (convert) {
        videoFrame = av_frame_alloc();
        bool ok = FFmpegSaveHelper::initVideoConvert(this, videoFrame, &videoSwsCtx, videoWidth, videoHeight, width,
                                                     height);
        if (!ok) {
            return false;
        }
    }

    QString srcFormatString = FFmpegHelper::getPixFormatString(AV_PIX_FMT_YUV420P);
    QString dstFormatString = FFmpegHelper::getPixFormatString(AV_PIX_FMT_YUV420P);
    QString msg1 = QString("源头: %1/%2/%3").arg(srcFormatString).arg(videoWidth).arg(videoHeight);
    QString msg2 = QString("目标: %1/%2/%3").arg(dstFormatString).arg(width).arg(height);
    debug(0, "视频转换", QString("转换: %1").arg(convert ? "启用" : "禁用") + " " + msg1 + " " + msg2);
    debug(0, "视频编码", QString("编码: %1").arg(videoCodec->name));
    debug(0, "视频编码",
          QString("帧率: %1 压缩: %2 缩放: %3").arg(encodeVideoFps).arg(encodeVideoRatio).arg(encodeVideoScale));
    return true;
}

//https://blog.csdn.net/irainsa/article/details/129289254
bool FFmpegSave::initAudioCtx() {
    //没启用音频编码或者不需要音频则不继续
    if (!audioEncode || !needAudio) {
        return true;
    }

    AVCodecID dstCodecId = AV_CODEC_ID_AAC;
    AVSampleFormat dstSampleFormat = AV_SAMPLE_FMT_FLTP;
    if (encodeAudio == EncodeAudio_Pcma) {
        dstCodecId = AV_CODEC_ID_PCM_ALAW;
        dstSampleFormat = AV_SAMPLE_FMT_S16;
    }

    //可以在这里自行更改采样率和通道数
    int dstSampleRate = sampleRate;
    int dstChannels = channelCount;
    AVSampleFormat sampleFormat = AV_SAMPLE_FMT_S16;
    bool convert = (encodeAudio == EncodeAudio_Aac);

    AVCodecx *audioCodec = avcodec_find_encoder(dstCodecId);
    if (!audioCodec) {
        debug(0, "音频编码", "avcodec_find_encoder");
        return false;
    }

    //创建音频编码器上下文
    audioCodecCtx = avcodec_alloc_context3(audioCodec);
    if (!audioCodecCtx) {
        debug(0, "音频编码", "avcodec_alloc_context3");
        return false;
    }

    //初始化音频编码器上下文
    FFmpegSaveHelper::initAudioCodecCtx(audioCodecCtx, dstSampleFormat, dstSampleRate, dstChannels, encodeSpeed);
    if (saveVideoType == SaveVideoType_Mp4) {
        audioCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    //打开音频编码器
    int result = avcodec_open2(audioCodecCtx, audioCodec, NULL);
    if (result < 0) {
        debug(result, "音频编码", "avcodec_open2");
        return false;
    }

    //创建编码用临时包
    audioPacket = FFmpegHelper::creatPacket(NULL);
    //强制转换音频数据/重采样
    if (convert) {
        audioFrame = av_frame_alloc();
        bool ok = FFmpegSaveHelper::initAudioConvert(this, audioFrame, &audioSwrCtx, audioCodecCtx, sampleFormat,
                                                     sampleRate, channelCount);
        if (!ok) {
            return false;
        }
    }

    QString srcFormatString = FFmpegHelper::getSampleFormatString(sampleFormat);
    QString dstFormatString = FFmpegHelper::getSampleFormatString(dstSampleFormat);
    QString msg1 = QString("源头: %1/%2/%3").arg(srcFormatString).arg(sampleRate).arg(channelCount);
    QString msg2 = QString("目标: %1/%2/%3").arg(dstFormatString).arg(dstSampleRate).arg(dstChannels);
    debug(0, "音频转换", QString("转换: %1").arg(convert ? "启用" : "禁用") + " " + msg1 + " " + msg2);
    debug(0, "音频编码", QString("编码: %1").arg(audioCodec->name));
    return true;
}

bool FFmpegSave::initStream() {
    //如果存在秘钥则启用加密
    AVDictionary *options = NULL;
    FFmpegHelper::initEncryption(&options, this->property("encryptKey").toByteArray());

    QString flag;
    if (getOnlySaveAudio() && encodeAudio != EncodeAudio_Aac) {
        flag = "wav";
    }

    //既可以是保存到文件也可以是推流(对应格式要区分)
    bool mov = audioCodecName.startsWith("pcm_");
    const char *format = FFmpegSaveHelper::getFormat(&options, fileName, mov, flag);

    //开辟一个格式上下文用来处理视频流输出(末尾url不填则rtsp推流失败)
    QByteArray fileData = fileName.toUtf8();
    const char *url = fileData.data();
    int result = avformat_alloc_output_context2(&formatCtx, NULL, format, url);
    if (result < 0) {
        debug(result, "创建格式", "");
        return false;
    }

    //创建输出视频流
    if (!this->initVideoStream()) {
        goto end;
    }

    //创建输出音频流
    if (!this->initAudioStream()) {
        goto end;
    }

    //打开输出文件/当保存到文件的时候需要执行/推流不需要
    if (!(formatCtx->oformat->flags & AVFMT_NOFILE)) {
        //记录开始时间并设置回调用于超时判断
        startTime = av_gettime();
        formatCtx->interrupt_callback.callback = FFmpegSaveHelper::openAndWriteCallBack;
        formatCtx->interrupt_callback.opaque = this;

        tryOpen = true;
        result = avio_open2(&formatCtx->pb, url, AVIO_FLAG_WRITE, &formatCtx->interrupt_callback, NULL);
        tryOpen = false;
        if (result < 0) {
            debug(result, "打开输出", "");
            goto end;
        }
    }

    //写入文件开始符
    result = avformat_write_header(formatCtx, &options);
    if (result < 0) {
        debug(result, "写文件头", "");
        goto end;
    }

    writeHeader = true;
    debug(0, "打开输出", QString("格式: %1").arg(format));
    return true;

    end:
    //关闭释放并清理文件
    this->close();
    this->deleteFile(fileName);
    return false;
}

bool FFmpegSave::initVideoStream() {
    if (needVideo) {
        videoIndexOut = 0;
        AVStream *stream = avformat_new_stream(formatCtx, NULL);
        if (!stream) {
            return false;
        }

        //设置旋转角度(没有编码的数据是源头带有旋转角度的/编码后的是正常旋转好的)
        if (!videoEncode) {
            FFmpegHelper::setRotate(stream, rotate);
        }

        //复制解码器上下文参数(不编码从源头流拷贝/编码从设置的编码器拷贝)
        int result = -1;
        if (videoEncode) {
            stream->r_frame_rate = videoCodecCtx->framerate;
            result = FFmpegHelper::copyContext(videoCodecCtx, stream, true);
        } else {
            result = FFmpegHelper::copyContext(videoStreamIn, stream);
        }

        if (result < 0) {
            debug(result, "复制参数", "");
            return false;
        }
    }

    return true;
}

bool FFmpegSave::initAudioStream() {
    if (needAudio) {
        audioIndexOut = (videoIndexOut == 0 ? 1 : 0);
        AVStream *stream = avformat_new_stream(formatCtx, NULL);
        if (!stream) {
            return false;
        }

        //复制解码器上下文参数(不编码从源头流拷贝/编码从设置的编码器拷贝)
        int result = -1;
        if (audioEncode) {
            result = FFmpegHelper::copyContext(audioCodecCtx, stream, true);
        } else {
            result = FFmpegHelper::copyContext(audioStreamIn, stream);
        }

        if (result < 0) {
            debug(result, "复制参数", "");
            return false;
        }
    }

    return true;
}

bool FFmpegSave::init() {
    //必须存在输入视音频流对象其中一个
    if (fileName.isEmpty() || (!videoStreamIn && !audioStreamIn)) {
        return false;
    }

    //检查推流地址是否正常/udp不需要检测
    if (saveMode != SaveMode_File && saveMode != SaveMode_Srt && saveMode != SaveMode_Udp &&
        !UrlHelper::checkUrl(fileName, 1000)) {
        debug(0, "地址不通", "");
        if (!this->isRunning()) {
            this->start();
        }
        return false;
    }

    //获取媒体信息
    this->getMediaInfo();
    //检查编码处理
    this->checkEncode();

    //没有启用视音频则不用继续
    if (!needVideo && !needAudio) {
        debug(0, "无需处理", "原因: 没有启用音视频");
        return false;
    }

    //ffmpeg2不支持重新编码的推流
#if (FFMPEG_VERSION_MAJOR < 3)
    if (saveMode != SaveMode_File && (videoEncode || audioEncode)) {
        return false;
    }
#endif

    //初始化对应视音频编码器
    if (!this->initVideoCtx()) {
        return false;
    }
    if (!this->initAudioCtx()) {
        return false;
    }

    //设置了需要封装格式并且没有重新编码则需要加上/保存文件才需要/推流不需要
    if (mp4ToAnnexB && !videoEncode && needVideo && saveMode == SaveMode_File) {
        FFmpegSaveHelper::initBsfCtx(videoStreamIn, &bsfCtx, videoCodecName == "h264");
        debug(0, "封装格式", QString("格式: %1").arg(videoCodecName));
    }

    //保存264数据直接写文件/为什么放在这里而不是最前面/因为有些也需要编码成正规的264
    if (saveVideoType == SaveVideoType_Stream) {
        return true;
    }

    //初始化视音频流
    if (!this->initStream()) {
        return false;
    }

    debug(0, "索引信息",
          QString("视频: %1/%2 音频: %3/%4").arg(videoIndexIn).arg(videoIndexOut).arg(audioIndexIn).arg(audioIndexOut));
    return true;
}

bool FFmpegSave::save() {
    //从队列中取出数据处理
    //qDebug() << TIMEMS << packets.count() << frames.count();

    if (packets.count() > 0) {
        mutex.lock();
        AVPacket *packet = packets.takeFirst();
        mutex.unlock();

        this->writePacket2(packet, packet->stream_index == videoIndexIn);
        FFmpegHelper::freePacket(packet);
    }

    if (frames.count() > 0) {
        mutex.lock();
        AVFrame *frame = frames.takeFirst();
        mutex.unlock();

        if (frame->width > 0) {
            FFmpegSaveHelper::encode(this, videoCodecCtx, videoPacket, frame, true);
        } else {
            FFmpegSaveHelper::encode(this, audioCodecCtx, audioPacket, frame, false);
        }

        FFmpegHelper::freeFrame(frame);
    }

    return (packets.count() > 0 || frames.count() > 0);
}

void FFmpegSave::close() {
    //将最后的缓冲数据写入到文件
    if (audioEncode) {
        FFmpegSaveHelper::encode(this, audioCodecCtx, audioPacket, NULL, false);
    }
    if (videoEncode) {
        FFmpegSaveHelper::encode(this, videoCodecCtx, videoPacket, NULL, true);
    }

    //写入文件结束符/写入过开始符才能执行/否则会崩溃
    if (formatCtx && writeHeader) {
        av_write_trailer(formatCtx);
    }

    //释放视频临时数据包
    if (videoPacket) {
        FFmpegHelper::freePacket(videoPacket);
        videoPacket = NULL;
    }

    //释放音频临时数据包
    if (audioPacket) {
        FFmpegHelper::freePacket(audioPacket);
        audioPacket = NULL;
    }

    //释放队列中的数据包
            foreach (AVPacket *packet, packets) {
            FFmpegHelper::freePacket(packet);
        }

    //释放队列中的数据帧
            foreach (AVFrame *frame, frames) {
            FFmpegHelper::freeFrame(frame);
        }

    //关闭文件流并释放对象
    if (formatCtx) {
        //avio_close(formatCtx->pb);
        //avformat_free_context(formatCtx);
        avformat_close_input(&formatCtx);
        formatCtx = NULL;
    }

    //关闭视频编码器上下文并释放对象
    if (videoCodecCtx) {
        avcodec_free_context(&videoCodecCtx);
        videoCodecCtx = NULL;
    }

    //关闭视频编码器上下文并释放对象
    if (audioCodecCtx) {
        avcodec_free_context(&audioCodecCtx);
        audioCodecCtx = NULL;
    }

    //释放转换用的视频相关
    if (videoFrame) {
        FFmpegHelper::freeFrame(videoFrame);
        sws_freeContext(videoSwsCtx);
        videoFrame = NULL;
        videoSwsCtx = NULL;
    }

    //释放转换用的音频相关
    if (audioFrame) {
        FFmpegHelper::freeFrame(audioFrame);
        swr_free(&audioSwrCtx);
        audioFrame = NULL;
        audioSwrCtx = NULL;
    }

    FFmpegSaveHelper::freeBsfCtx(&bsfCtx);

    //清空队列并复位
    packets.clear();
    frames.clear();
    this->reset();
}

void FFmpegSave::reset() {
    keyFrame = false;
    writeHeader = false;
    tryOpen = false;

    videoCount = 0;
    audioDuration = 0;
    errorCount = 0;

    needVideo = true;
    needAudio = true;
    videoIndexIn = -1;
    audioIndexIn = -1;
    videoIndexOut = -1;
    audioIndexOut = -1;
}

int FFmpegSave::getVideoIndexIn() {
    return this->videoIndexIn;
}

int FFmpegSave::getAudioIndexIn() {
    return this->audioIndexIn;
}

int FFmpegSave::getVideoIndexOut() {
    return this->videoIndexOut;
}

int FFmpegSave::getAudioIndexOut() {
    return this->audioIndexOut;
}

AVStream *FFmpegSave::getVideoStream() {
    if (videoIndexOut >= 0) {
        return formatCtx->streams[videoIndexOut];
    } else {
        return NULL;
    }
}

AVStream *FFmpegSave::getAudioStream() {
    if (audioIndexOut >= 0) {
        return formatCtx->streams[audioIndexOut];
    } else {
        return NULL;
    }
}

bool FFmpegSave::getTryOpen() {
    return this->tryOpen;
}

qint64 FFmpegSave::getStartTime() {
    return this->startTime;
}

int FFmpegSave::getConnectTimeout() {
    return this->connectTimeout;
}

int FFmpegSave::getPacketCount() {
    return this->packets.count();
}

int FFmpegSave::getFrameCount() {
    return this->frames.count();
}

void FFmpegSave::debug(int result, const QString &head, const QString &msg) {
    if (result < 0) {
        QString text = (msg.isEmpty() ? "" : (" " + msg));
        AbstractSaveThread::debug(head, QString("错误: %1%2").arg(FFmpegHelper::getError(result)).arg(text));
    } else {
        AbstractSaveThread::debug(head, msg);
    }
}

bool FFmpegSave::getVideoEncode() {
    return this->videoEncode;
}

bool FFmpegSave::getAudioEncode() {
    return this->audioEncode;
}

bool FFmpegSave::getOnlySaveAudio() {
    return (needAudio && !needVideo);
}

void FFmpegSave::setSendPacket(bool sendPacket, bool onlySendPacket) {
    this->sendPacket = sendPacket;
    this->onlySendPacket = onlySendPacket;
}

void FFmpegSave::setEncodePara(bool mp4ToAnnexB, bool audioEncode, bool videoEncode, double encodeSpeed,
                               EncodeAudio encodeAudio, EncodeVideo encodeVideo, int encodeVideoFps,
                               float encodeVideoRatio, const QString &encodeVideoScale) {
    this->mp4ToAnnexB = mp4ToAnnexB;
    this->audioEncode = audioEncode;
    this->videoEncode = videoEncode;
    this->encodeSpeed = encodeSpeed;
    this->encodeAudio = encodeAudio;
    this->encodeVideo = encodeVideo;
    this->encodeVideoFps = encodeVideoFps;
    this->encodeVideoRatio = encodeVideoRatio;
    this->encodeVideoScale = encodeVideoScale;

    //指定了音频编码类型则启用音频编码
    if (encodeAudio != EncodeAudio_Auto) {
        this->audioEncode = true;
    }

    //指定了视频编码类型则启用视频编码
    if (encodeVideo != EncodeVideo_None) {
        this->videoEncode = true;
    }

    //设置了清晰度以及启用了缩放或者设置了尺寸则启用视频编码
    if (encodeVideoRatio != 1 || encodeVideoScale != "1") {
        this->videoEncode = true;
    }
}

void FFmpegSave::setSavePara(int mediaType, const SaveVideoType &saveVideoType, AVStream *videoStreamIn,
                             AVStream *audioStreamIn) {
    this->mediaType = mediaType;
    this->saveVideoType = saveVideoType;
    this->videoStreamIn = videoStreamIn;
    this->audioStreamIn = audioStreamIn;
}

void FFmpegSave::writePacket2(AVPacket *packet) {
    //非音视频流不用处理
    int index = packet->stream_index;
    if (index != videoIndexOut && index != audioIndexOut) {
        return;
    }

    //转发数据包(可以设置仅仅转发数据包不用继续)
    if (sendPacket) {
        emit receivePacket(FFmpegHelper::creatPacket(packet));
        if (onlySendPacket) {
            return;
        }
    }

    //封装格式 https://blog.csdn.net/weixin_44520287/article/details/113435440 https://xilixili.net/2018/08/20/ffmpeg-got-raw-h264/
    //测试发现部分文件如果是非编码保存也写入了/可能部分播放器不支持保存后的文件播放/比如安卓上
    if (index == videoIndexOut) {
        FFmpegSaveHelper::writeBsf(packet, videoStreamIn, bsfCtx);
    }

    if (saveVideoType == SaveVideoType_Stream) {
        //只需要写入视频数据
        if (index == videoIndexOut) {
            file.write((char *) packet->data, packet->size);
        }
    } else if (saveVideoType == SaveVideoType_Mp4) {
        //取出输入输出流的时间基
        AVStream *streamIn = (index == videoIndexOut ? videoStreamIn : audioStreamIn);
        AVStream *streamOut = formatCtx->streams[index];
        AVRational timeBaseIn = streamIn->time_base;
        AVRational timeBaseOut = streamOut->time_base;

        //转换时间基准
        if (index == videoIndexOut) {
            FFmpegSaveHelper::rescalePacket(packet, timeBaseIn, videoCount, frameRate);
            FFmpegSaveHelper::rescalePacket(packet, timeBaseIn, timeBaseOut);
        } else if (index == audioIndexOut) {
            if (audioEncode) {
                FFmpegSaveHelper::rescalePacket(packet, audioDuration);
            } else {
                FFmpegSaveHelper::rescalePacket(packet, timeBaseIn, timeBaseOut, audioDuration);
            }
        }

        //打印对应的信息方便查看/videoIndexOut/audioIndexOut
        if (index == -1) {
            qDebug() << TIMEMS << flag << index << packet->pts << packet->dts << packet->duration;
        }

        //倍速调整时间戳
        if (encodeSpeed != 1) {
            packet->pts = packet->pts / encodeSpeed;
            packet->dts = packet->dts / encodeSpeed;
        }

        //写入一帧数据 av_write_frame/如果用 av_interleaved_write_frame 默认会缓存/可能导致音频越来越慢
        int result = av_write_frame(formatCtx, packet);
        if (result < 0) {
            errorCount++;
            debug(result, QString("写%1包").arg(index == audioIndexOut ? "音频" : "视频"), "");
        } else {
            errorCount = 0;
        }

        //推流超过错误次数需要重连
        if (errorCount >= 5 && saveMode != SaveMode_File) {
            isOk = false;
            errorCount = 0;
            emit receiveSaveError(VideoError_Save);
        }

        //发送当前保存进度信号
        qint64 time = FFmpegHelper::getPtsTime(formatCtx, packet) / 1000;
        emit savePosition(index == videoIndexOut, time);
    }
}

void FFmpegSave::writePacket2(AVPacket *packet, bool video) {
    //编码后的数据包需要手动设置对应流索引
    packet->stream_index = (video ? videoIndexOut : audioIndexOut);
    this->writePacket2(packet);
}

bool FFmpegSave::checkIndex(int index) {
    //对应流没启用或者索引非法则不继续
    if (index == videoIndexIn) {
        if (!needVideo) {
            return false;
        }
    } else if (index == audioIndexIn) {
        if (!needAudio) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void FFmpegSave::writePacket(AVPacket *packet, int index) {
    //没打开不处理
    if (!isOk) {
        return;
    }

    //暂停期间不处理并且将关键帧标志位复位(继续录制后重新以关键帧开头保证画面流畅/没有这个处理可能暂停后继续播放中间图像会有花块)
    if (isPause) {
        keyFrame = false;
        return;
    }

    //校验索引
    if (!this->checkIndex(index)) {
        return;
    }

    //只有音频以及视频转码音频不转码情况下不需要判断关键帧
    if ((videoIndexIn < 0 && audioIndexIn >= 0) || (videoEncode && !audioEncode)) {
        keyFrame = true;
    }

    //保证收到视频关键帧以后再开始写入(如果首帧不是关键帧部分播放器比如vlc播放器打开可能是绿屏)
    if (!keyFrame && index == videoIndexIn) {
        keyFrame = FFmpegHelper::checkPacketKey(packet);
    }
    if (!keyFrame) {
        return;
    }

    //可以直接写入到文件也可以排队处理(直接写入会改变原有的包的各种属性参数)
    //this->writePacket2(packet, packet->stream_index == videoIndexIn);
    mutex.lock();
    packets << FFmpegHelper::creatPacket(packet);
    mutex.unlock();
}

void FFmpegSave::writeFrame(AVFrame *frame, int index) {
    //没打开或者暂停阶段不处理
    if (!isOk || isPause) {
        return;
    }

    //校验索引
    if (!this->checkIndex(index)) {
        return;
    }

    //只有音频以及视频编码的情况下不需要判断关键帧
    if ((videoIndexIn < 0 && audioIndexIn >= 0) || videoEncode) {
        keyFrame = true;
    }
    if (!keyFrame) {
        return;
    }

    mutex.lock();

    //有宽度是视频帧/否则是音频帧
    if (frame->width > 0) {
        //启用了转换则用先执行转换
        if (videoSwsCtx) {
            int result = sws_scale(videoSwsCtx, (const quint8 **) frame->data, frame->linesize, 0, videoHeight,
                                   videoFrame->data, videoFrame->linesize);
            if (result >= 0) {
                frames << av_frame_clone(videoFrame);
            }
        } else {
            frames << av_frame_clone(frame);
        }
    } else {
        //启用了转换则用先执行转换
        if (audioSwrCtx) {
            int len = frame->nb_samples;
            //int size = av_samples_get_buffer_size(NULL, audioFrame->channels, audioFrame->nb_samples, audioCodecCtx->sample_fmt, 1);
            int result = swr_convert(audioSwrCtx, audioFrame->data, len, (const quint8 **) frame->data,
                                     frame->nb_samples);
            if (result >= 0) {
                frames << av_frame_clone(audioFrame);
            }
        } else {
            frames << av_frame_clone(frame);
        }
    }

    mutex.unlock();
}
