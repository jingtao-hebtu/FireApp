#include "ffmpegthread.h"
#include "ffmpegthreadhelper.h"
#include "ffmpegavio.h"
#include "ffmpeghelper.h"
#include "ffmpegutil.h"
#include "ffmpegfilter.h"
#include "ffmpegsync.h"
#include "ffmpegsave.h"
#include "videohelper.h"

#include "audioplayer.h"

FFmpegThread::FFmpegThread(QObject *parent) : VideoThread(parent) {
    //执行初始化
    FFmpegHelper::initLib();

    changePosition = false;
    pushPreview = true;
    needClear = false;
    keyFrame = true;

    sendPacket = false;
    sendFrame = false;

    speed = 1.0;
    tryOpen = false;
    tryRead = false;

    keyPacketIndex = 0;
    keyPacketCount = 0;
    realPacketSize = 0;
    realPacketCount = 0;

    videoFirstPts = 0;
    audioFirstPts = 0;

    packet = NULL;

    videoFrame = NULL;
    tempFrame = NULL;
    yuvFrame = NULL;
    imageFrame = NULL;
    imageData = NULL;

    audioFrame = NULL;
    pcmFrame = NULL;

    yuvSwsCtx = NULL;
    imageSwsCtx = NULL;
    pcmSwrCtx = NULL;

    options = NULL;
    ifmt = NULL;
    formatCtx = NULL;

    videoCodec = NULL;
    audioCodec = NULL;

    videoCodecCtx = NULL;
    audioCodecCtx = NULL;

    saveVideoType = SaveVideoType_Mp4;
    saveAudioType = SaveAudioType_None;

    //初始化音频解码同步线程(本线程释放的时候也会自动释放)
    useSync = true;
    audioSync = new FFmpegSync(FFmpegSync::StreamType_Audio, this);
    connect(audioSync, SIGNAL(receivePosition(qint64)), this, SIGNAL(receivePosition(qint64)));

    //初始化视频解码同步线程(本线程释放的时候也会自动释放)
    videoSync = new FFmpegSync(FFmpegSync::StreamType_Video, this);
    connect(videoSync, SIGNAL(receivePosition(qint64)), this, SIGNAL(receivePosition(qint64)));

    //实例化恢复暂停定时器(单次定时器)
    timerPause = new QTimer(this);
    connect(timerPause, SIGNAL(timeout()), this, SLOT(pause()));
    timerPause->setInterval(100);
    timerPause->setSingleShot(true);

    //实例化切换进度定时器(单次定时器)
    timerPosition = new QTimer(this);
    connect(timerPosition, SIGNAL(timeout()), this, SLOT(setPosition()));
    timerPosition->setInterval(200);
    timerPosition->setSingleShot(true);

    //实例化音频播放类
    audioPlayer = new AudioPlayer;
    connect(audioPlayer, SIGNAL(receiveVolume(int)), this, SIGNAL(receiveVolume(int)));
    connect(audioPlayer, SIGNAL(receiveMuted(bool)), this, SIGNAL(receiveMuted(bool)));
    connect(audioPlayer, SIGNAL(receiveLevel(qreal, qreal)), this, SIGNAL(receiveLevel(qreal, qreal)));

    //裁剪标志位和最后结束裁剪时间
    isCrop = false;
    cropTime = QDateTime::currentDateTime().addDays(-1);

    //线程启动后初始化websocket通信
    avio = NULL;
    connect(this, SIGNAL(started()), this, SLOT(initAvio()));
}

FFmpegThread::~FFmpegThread() {
    //关闭音频播放类
    audioPlayer->close();
    FFmpegAvio::free(&avio);
}

void FFmpegThread::initAvio() {
    FFmpegAvio::init(&avio, mediaUrl, flag, mediaType);
}

void FFmpegThread::run() {
    this->checkData();
    while (!stopped) {
        if (!isOk) {
            //记住开始解码的时间用于用音视频同步等
            startTime = av_gettime();
            this->closeVideo();
            this->openVideo();
            this->checkOpen();
            continue;
        }

        //是否需要继续
        if (!checkRun()) {
            msleep(1);
            continue;
        }

        //读取一帧(通过标志位控制回调那边做超时判断)
        tryRead = true;
        int result = av_read_frame(formatCtx, packet);
        tryRead = false;
        if (result >= 0) {
            //错误计数清零以及更新最后的解码时间
            errorCount = 0;
            this->updateTime();
            //没有时间的数据帧也需要过滤
            if (formatName == "hls" && packet->pts == AV_NOPTS_VALUE && packet->dts == AV_NOPTS_VALUE) {
                continue;
            }

            //判断当前包是视频还是音频
            int index = packet->stream_index;
            //qint64 time = FFmpegHelper::getPtsTime(formatCtx, packet) / 1000;
            //QString msg = QString("time: %1 pts: %2 dts: %3 pos: %4").arg(time).arg(packet->pts).arg(packet->dts).arg(position);
            if (index == videoIndex) {
                decodeVideo0(packet);
                //debug(0, "视频数据", msg);
            } else if (index == audioIndex) {
                decodeAudio0(packet);
                //debug(0, "音频数据", msg);
            }
        } else if (getIsFile()) {
            //如果是视频文件则判断是否文件结束
            msleep(1);
            if (result == AVERROR_EOF || result == AVERROR_EXIT) {
                //当同步队列中的数量为0(表示解码处理完成)才需要跳出
                if (videoSync->getPacketCount() == 0 && audioSync->getPacketCount() == 0) {
                    if (playRepeat) {
                        this->replay();
                    } else {
                        this->reopen();
                        continue;
                    }
                }
            }
        } else {
            //下面这种情况在视频流断开后出现
            msleep(100);
            errorCount++;
            debug(result, "错误计数", QString("次数: %1").arg(errorCount));
            if (errorCount >= 3) {
                this->reopen();
                continue;
            }
        }

        av_packet_unref(packet);
    }

    //关闭视频
    this->closeVideo();
    debug(0, "线程结束", "");
}

void FFmpegThread::checkData() {
    //保证websocket通信成功且已经有数据了才继续执行
    if (mediaType == MediaType_WebSocket) {
        //一直尝试打开也要有个超时时间
        int count = 0;
        while (!stopped) {
            msleep(30);
            count++;
            if (count >= 100 || (avio && avio->canOpen())) {
                break;
            }
        }
    }
}

bool FFmpegThread::checkRun() {
    //暂停及切换进度期间继续更新时间防止超时
    if (isPause || changePosition) {
        this->updateTime();
        this->readAndClear();
        return false;
    }

    //解码队列中帧数过多暂停读取(倍速越大数量也大)
    if (useSync) {
        int max = speed * (formatName == "hls" ? 500 : 100);
        if (videoSync->getPacketCount() >= max || audioSync->getPacketCount() >= max) {
            this->updateTime();
            return false;
        }
    }

    //websocket数据需要判断缓存大小才能继续
    if (avio && !avio->canRead()) {
        if (lastTime.msecsTo(QDateTime::currentDateTime()) >= 3000) {
            readTimeout > 0 ? this->reopen() : this->stop3();
        }
        return false;
    }

    return true;
}

void FFmpegThread::readAndClear() {
    //视频流要继续读取不然会一直累积
    if (!getIsFile() && mediaType != MediaType_Screen /*&& mediaType != MediaType_Device*/) {
        av_read_frame(formatCtx, packet);
        av_packet_unref(packet);
    }
}

void FFmpegThread::replay() {
    //加个时间限制防止频繁执行
    QDateTime now = QDateTime::currentDateTime();
    if (qAbs(now.msecsTo(lastTime)) < 300) {
        return;
    }

    lastTime = now;
    videoSync->reset();
    audioSync->reset();

    //有些文件前面几帧缺失导致播放定位失败
    this->position = (videoFirstPts > 0 ? 1000 : 100);
    this->setPosition();
    debug(0, "循环播放", "");
}

void FFmpegThread::reopen() {
    //释放数据并判断是否需要重连
    av_packet_unref(packet);
    this->stop2();
    msleep(100);
}

bool FFmpegThread::checkFrameSize(AVFrame *frame) {
    //有些视频流会动态改变分辨率需要重新打开(主动设置过旋转角度的不用处理)
    //执行了裁剪滤镜也会触发变化
    int width = frame->width;
    int height = frame->height;
    if (width > 0 && height > 0 && (videoWidth != width || videoHeight != height)) {
        //先要根据旋转角度调整尺寸
        int w = width;
        int h = height;
        VideoHelper::rotateSize(rotate, w, h);
        if (isCrop) {
            this->checkVideoSize(w, h);
        } else {
            //此时是取消裁剪后触发的分辨率变化
            qint64 offset = cropTime.msecsTo(QDateTime::currentDateTime());
            if (offset < 1000) {
                this->checkVideoSize(w, h);
            } else if (rotate <= 0 && mediaType == MediaType_Rtsp && hardware == "none") {
                debug(0, "尺寸变化",
                      QString("变化: %1x%2 -> %3x%4").arg(videoWidth).arg(videoHeight).arg(width).arg(height));
                isOk = false;
                return false;
            }
        }
    }

    return true;
}

bool FFmpegThread::scaleAndSaveVideo(bool &needScale, AVFrame *frame) {
    if (!checkFrameSize(frame)) {
        return false;
    }

    //不是yuv420则先要转换(本地摄像头格式yuyv422/还有些视频文件是各种各样的格式)
    AVPixelFormat format = videoCodecCtx->pix_fmt;
    //很多摄像头视频流是yuvj420也不需要转换可以直接用(硬解码模式下格式是NV12所以needScale永远为真)
    needScale = (format != AV_PIX_FMT_YUV420P && format != AV_PIX_FMT_YUVJ420P);

    //非录像阶段硬解码模式下不需要转换
    if (!isRecord && hardware != "none") {
        needScale = false;
    }

    //在录像阶段硬解码模式下非文件没开启转码则不需要转换
    if (isRecord && !getIsFile() && encodeVideo == EncodeVideo_None && hardware != "none") {
        needScale = false;
    }

    //非录像阶段必须处于截图和绘制模式下才需要转换
    if (!isRecord && (isSnap || videoMode == VideoMode_Painter)) {
        needScale = false;
    }

    //将非yuv420格式转换到yuv420格式
    if (needScale) {
        //默认没有pts需要拷贝下pts/不拷贝的话在保存那边可能有问题
        yuvFrame->pts = frame->pts;
        int result = sws_scale(yuvSwsCtx, (const quint8 **) frame->data, frame->linesize, 0, videoHeight,
                               yuvFrame->data, yuvFrame->linesize);
        if (result < 0) {
            debug(result, "转换失败", QString("格式: %1").arg(FFmpegHelper::getPixFormatString(format)));
            return false;
        }
    }

    //转换过的用转换后的数据
    this->writeFile(needScale ? yuvFrame : frame, true);
    return true;
}

void FFmpegThread::checkAndShowVideo(bool needScale, AVFrame *frame) {
    //截图和绘制都转成图片
    if (isDetect || isSnap || videoMode == VideoMode_Painter) {
        //启动计时
        timer.restart();
        //截图如果设置的从原始图像截图则取最原始的视频帧
        AVFrame *framex = ((isSnap && snapSource) ? videoFrame : frame);
        QImage image = FFmpegThreadHelper::frameToImage(imageSwsCtx, framex, imageFrame, imageData);
        if (image.isNull()) {
            return;
        }

        //如果有旋转角度先要旋转
        VideoHelper::rotateImage(rotate, image);
        if (isSnap) {
            isSnap = false;
            //裁剪期间应用了裁剪滤镜对应的截图有问题
            if (isCrop) {
                int width, height;
                FFmpegHelper::getResolution(formatCtx->streams[videoIndex], width, height);
                if (width != videoWidth || height != videoHeight) {
                    return;
                }
            }

            image.save(this->snapName, "jpg");
            QMetaObject::invokeMethod(this, "snapFinsh");
            //带旋转角度的视频抓图后需要重新初始化滤镜
            if (rotate > 0) {
                videoFilter.init = false;
            }
        } else {
            emit receiveImage(image, timer.elapsed());
            return;
        }
    }

    //可能有tile滤镜导致产生空帧
    if (frame->width == 0) {
        return;
    }

    //qDebug() << TIMEMS << videoWidth << videoHeight << frame->width << frame->height << frame->linesize[0] << frame->linesize[1] << frame->linesize[2];
    if (hardware == "none") {
        if (needScale) {
            emit receiveFrame(videoWidth, videoHeight, yuvFrame->data[0], yuvFrame->data[1], yuvFrame->data[2],
                              videoWidth, videoWidth / 2, videoWidth / 2);
            this->writeVideoData(videoWidth, videoHeight, yuvFrame->data[0], yuvFrame->data[1], yuvFrame->data[2]);
        } else {
            emit receiveFrame(frame->width, frame->height, frame->data[0], frame->data[1], frame->data[2],
                              frame->linesize[0], frame->linesize[1], frame->linesize[2]);
            this->writeVideoData(frame->linesize[0], frame->height, frame->data[0], frame->data[1], frame->data[2]);
        }
    } else {
        emit receiveFrame(videoWidth, videoHeight, frame->data[0], frame->data[1], frame->linesize[0],
                          frame->linesize[1]);
    }
}

void FFmpegThread::decodeVideo0(AVPacket *packet) {
    //统计关键帧间隔和实时码率
    //FFmpegThreadHelper::checkKeyPacketCount(this, packet, keyPacketIndex, keyPacketCount);
    if (realBitRate) {
        FFmpegThreadHelper::checkRealPacketSize(this, packet, frameRate, realPacketSize, realPacketCount);
    }

    //本地摄像头的包都是关键帧
    if (mediaType == MediaType_Device) {
        packet->flags = 1;
    }

    //有些首帧开始的时间不是0/而是一个指定的时间所以需要减去
    if (videoFirstPts > 5000 && mediaType != MediaType_Rtsp) {
        packet->pts -= videoFirstPts;
        packet->dts = packet->pts;
    }

    if (useSync) {
        //加入到队列交给解码同步线程处理
        videoSync->append(FFmpegHelper::creatPacket(packet));
    } else {
        //直接当前线程解码
        decodeVideo1(packet);
        if (decodeType != DecodeType_Fastest) {
            FFmpegHelper::delayTime(formatCtx, packet, startTime);
        }
    }
}

void FFmpegThread::decodeVideo1(AVPacket *packet) {
    this->writeFile(packet, true);
    //未编码推流不用继续(可以节约大量CPU/预览还需要继续解码)
    //未编码的才存在缓存数据需要清空
    bool videoEncode = saveFile->getVideoEncode();
    if (!videoEncode) {
        if (needClear) {
            keyFrame = false;
            this->clearBuffer(true);
            return;
        }
        if (!keyFrame) {
            keyFrame = FFmpegHelper::checkPacketKey(packet);
        }
        if (!keyFrame) {
            return;
        }
    }

    if (saveFile->getIsOk() && !videoEncode && !pushPreview) {
        return;
    }

    if (hardware == "none") {
        FFmpegThreadHelper::decode(this, videoCodecCtx, packet, videoFrame, true);
    } else {
        FFmpegThreadHelper::decode(this, videoCodecCtx, packet, tempFrame, videoFrame);
    }
}

void FFmpegThread::decodeVideo2(AVPacket *packet) {
    //判断帧类型 I B P
#if 0
    int type = (hardware == "none" ? videoFrame->pict_type : tempFrame->pict_type);
    qDebug() << TIMEMS << type;
    if (type == 1) {
        qDebug() << TIMEMS << type;
        //return;
    }
#endif

    //如果需要重新初始化则先初始化滤镜(带旋转角度的抓图也需要重新处理)
    if (!videoFilter.init || (isSnap && rotate > 0)) {
        this->initFilter();
    }

    //先处理滤镜(软解码需要数据中转一下/硬解码源头就已经中转)
    AVFrame *frame = videoFrame;
    if (videoFilter.isOk) {
        if (av_buffersrc_add_frame_flags(videoFilter.filterSrcCtx, videoFrame, AV_BUFFERSRC_FLAG_KEEP_REF) >= 0) {
            //到了末尾则重新开始
            filterFrameIndex++;
            if (filterFrameIndex >= maxFilterFrame) {
                filterFrameIndex = 0;
            }

            //从队列中指定帧用来存放滤镜后的帧
            frame = filterFrames.at(filterFrameIndex);
            //添加到等待释放的队列中
            filterFramex << frame;
            //超过了大小则释放移除以一个
            if (filterFramex.count() >= maxFilterFrame) {
                av_frame_unref(filterFramex.takeFirst());
            }

            av_buffersink_get_frame(videoFilter.filterSinkCtx, frame);
        }
    }

    bool needScale = false;
    if (!scaleAndSaveVideo(needScale, frame)) {
        return;
    }

    this->checkAndShowVideo(needScale, frame);

    //开启了逐帧播放则每次播放完一帧就暂停
    if (playStep) {
        this->pause();
    }
}

void FFmpegThread::decodeAudio0(AVPacket *packet) {
    //如果没有开启则不用继续
    if (!decodeAudio) {
        return;
    }

    if (audioFirstPts > AV_TIME_BASE && mediaType != MediaType_Rtsp) {
        packet->pts -= audioFirstPts;
        packet->dts = packet->pts;
    }

    if (useSync) {
        //加入到队列交给解码同步线程处理
        audioSync->append(FFmpegHelper::creatPacket(packet));
    } else {
        //直接当前线程解码
        decodeAudio1(packet);
        if (decodeType != DecodeType_Fastest) {
            FFmpegHelper::delayTime(formatCtx, packet, startTime);
        }
    }
}

void FFmpegThread::decodeAudio1(AVPacket *packet) {
    //限制超过一定倍速不解码音频数据
    if (speed >= 5) {
        //return;
    }

    this->writeFile(packet, false);
    //未编码推流不用继续(可以节约大量CPU/预览还需要继续解码)
    if (saveFile->getIsOk() && !saveFile->getAudioEncode() && !pushPreview) {
        return;
    }

    FFmpegThreadHelper::decode(this, audioCodecCtx, packet, audioFrame, false);
}

void FFmpegThread::decodeAudio2(AVPacket *packet) {
    char *data = (char *) audioFrame->data[0];
    int size = audioFrame->nb_samples;

    //执行音频数据转换/将原始收到的音频数据audioFrame转换到可以直接播放的pcm数据pcmFrame
    if (pcmSwrCtx) {
        int result = swr_convert(pcmSwrCtx, pcmFrame->data, audioFrame->nb_samples, (const quint8 **) audioFrame->data,
                                 audioFrame->nb_samples);
        if (result < 0) {
            return;
        }

        //纠正下采样率/不然可能是设置的默认的9600
        pcmFrame->nb_samples = audioFrame->nb_samples;
        //取出声音数据
        data = (char *) pcmFrame->data[0];
        //计算重采样后的大小/可能采样率变了/比如源头是44100而指定了转换成48000
        size = av_rescale_rnd(audioFrame->nb_samples, pcmFrame->sample_rate, audioFrame->sample_rate, AV_ROUND_UP);
    }

    //计算直接播放的数据的大小
    int len = av_samples_get_buffer_size(NULL, pcmChannels, size, pcmSampleFormat, 1);
    //播放声音数据
    if (playAudio && !timerPause->isActive()) {
        //audioPlayer->playAudioData(data, len);
        QMetaObject::invokeMethod(audioPlayer, "playAudioData", Q_ARG(const char *, data), Q_ARG(qint64, len));
    }

    //保存音频文件
    if (saveAudioType == SaveAudioType_Pcm || saveAudioType == SaveAudioType_Wav) {
        this->writeAudioData(data, len);
    } else if (saveAudioType == SaveAudioType_Aac) {
        this->writeAudioData((char *) packet->data, packet->size);
    }

    //写入音频数据
    this->writeFile(pcmSwrCtx ? pcmFrame : audioFrame, false);
}

void FFmpegThread::writeFile(AVPacket *packet, bool video) {
    //启用了转发数据包则先发送对应信号
    if (sendPacket) {
        emit writePacket(packet, video ? videoIndex : audioIndex);
    }

    if (!saveFile->getIsOk()) {
        return;
    }

    //没开启音视频编码就直接写入原始数据包
    if (video) {
        if (!saveFile->getVideoEncode()) {
            saveFile->writePacket(packet, videoIndex);
        }
    } else {
        if (!saveFile->getAudioEncode()) {
            saveFile->writePacket(packet, audioIndex);
        }
    }
}

void FFmpegThread::writeFile(AVFrame *frame, bool video) {
    //启用了转发数据帧则先发送对应信号
    if (sendFrame) {
        emit writeFrame(frame, video ? videoIndex : audioIndex);
    }

    if (!saveFile->getIsOk()) {
        return;
    }

    //裁剪滤镜不支持录像存储
    if (isCrop) {
        if (!saveFile->getIsPause()) {
            saveFile->pause();
        }
        return;
    } else {
        if (isRecord && saveFile->getIsPause()) {
            saveFile->pause();
        }
    }

    //开启了音视频编码就直接写入解码数据帧
    if (video) {
        if (saveFile->getVideoEncode()) {
            saveFile->writeFrame(frame, videoIndex);
        }
    } else {
        if (saveFile->getAudioEncode()) {
            saveFile->writeFrame(frame, audioIndex);
        }
    }
}

void FFmpegThread::initOption() {
    //增加rtp/sdp支持/貌似网络地址带.sdp结尾的这种可以不用加
    if (mediaType == MediaType_FileLocal && mediaUrl.endsWith(".sdp")) {
        av_dict_set(&options, "protocol_whitelist", "file,rtp,udp", 0);
    }

    //设置解密秘钥字串
    FFmpegHelper::initDecryption(&options, this->property("decryptKey").toByteArray());
    //设置缓存大小/通信协议
    FFmpegHelper::initOption(&options, caching, transport);

    //设置分辨率/帧率/桌面采集偏移值等参数
    if (mediaType == MediaType_Device || mediaType == MediaType_Screen) {
        FFmpegHelper::initOption(&options, mediaUrl);
        FFmpegHelper::initOption(&options, bufferSize, frameRate);
        if (mediaType == MediaType_Screen) {
            int offsetX = this->property("offsetX").toInt();
            int offsetY = this->property("offsetY").toInt();
            FFmpegHelper::initOption(&options, offsetX, offsetY, mediaUrl);
        }
    }
}

bool FFmpegThread::initInput() {
    //本地摄像头/桌面录屏/linux系统可以打开cheese程序查看本地摄像头(如果是在虚拟机中需要设置usb选项3.1)
    if (mediaType == MediaType_Device) {
        ifmt = av_find_input_format(mediaUrl.startsWith("audio=") ? Device_Audio : Device_Video);
    } else if (mediaType == MediaType_Screen) {
        ifmt = av_find_input_format(Device_Screen);
    }

    //实例化格式处理上下文
    formatCtx = avformat_alloc_context();
    //设置超时回调(有些不存在的地址或者网络不好的情况下要卡很久)
    formatCtx->interrupt_callback.callback = FFmpegThreadHelper::openAndReadCallBack;
    formatCtx->interrupt_callback.opaque = this;

    //本地设备设置采集格式
    if (mediaType == MediaType_Device) {
        QMetaObject::invokeMethod(audioPlayer, "setValue", Q_ARG(const char *, "microphone"), Q_ARG(QVariant, true));
        FFmpegHelper::setVideoCodecName(formatCtx, videoCodecName);
    }

    //取出最终的播放地址
    QString url = VideoHelper::getPlayUrl(VideoCore_FFmpeg, mediaType, mediaUrl);
    //桌面采集指定窗口标题需要转换才能支持中文/ffmpeg5开始才修复了这个问题
    QByteArray urlData = VideoHelper::getUrlData(mediaType, url, FFMPEG_VERSION_MAJOR < 5);

    //打开输入(通过标志位控制回调那边做超时判断)
    tryOpen = true;
    int result = -1;
    if (mediaType == MediaType_WebSocket) {
        formatCtx->pb = avio->getCtx();
        if (!formatCtx->pb) {
            return false;
        }

        //打开失败则重新连接
        result = avformat_open_input(&formatCtx, NULL, NULL, &options);
        if (result < 0) {
            QMetaObject::invokeMethod(avio, "reopen");
        }
    } else {
        result = avformat_open_input(&formatCtx, urlData.constData(), ifmt, &options);
    }

    tryOpen = false;
    if (result < 0) {
        debug(result, "打开地址", "");
        return false;
    }

    //根据自己项目需要/开启下面部分代码/加快视频流打开速度/开启后由于值太小可能会出现部分视频流获取不到分辨率
    if (decodeType == DecodeType_Fastest && mediaType == MediaType_Rtsp) {
        FFmpegHelper::initRtspFast(formatCtx);
    }

    //获取流信息
    result = avformat_find_stream_info(formatCtx, NULL);
    if (result < 0) {
        debug(result, "找流失败", "");
        return false;
    }

    //封装格式
    formatName = formatCtx->iformat->name;
    //校验硬解码
    FFmpegThreadHelper::checkHardware(formatName, hardware);

    //获取文件时长(这里获取到的是秒)
    double length = (double) formatCtx->duration / AV_TIME_BASE;
    //如果是本地文件而且没有时长则用最原始方法读取时长
    //有部分设备导出的视频文件读取出来时长不对也可以用此方法读取
    if (mediaType == MediaType_FileLocal && duration <= 0) {
        if (this->property("getDurationByFrame").toBool()) {
            length = FFmpegUtil::getDuration(mediaUrl);
        }
    }

    duration = length * 1000;
    this->checkMediaType();

    //发送文件时长信号
    if (getIsFile()) {
        emit receiveDuration(duration > 0 ? duration : 0);
    }

    //获取音视频轨道信息(一般有一个音频或者一个视频/ts节目文件可能有多个)
    FFmpegHelper::getTracks(formatCtx, audioTracks, videoTracks);
    emit receiveTrack(audioTracks, videoTracks);
    QString msg = QString("格式: %1 时长: %2 秒 加速: %3").arg(formatName).arg(duration / 1000).arg(hardware);
    debug(0, "媒体信息", msg);
    return true;
}

bool FFmpegThread::initVideo() {
    //找到视频流索引
    videoIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        //有些没有视频流所以这里不用返回
        videoIndex = -1;
        debug(0, "无视频流", "");
    } else {
        //如果手动指定了轨道则取指定的(节目流有多个轨道可以指定某个)
        if (videoTrack >= 0 && videoTracks.contains(videoTrack)) {
            videoIndex = videoTrack;
        }

        //取出流获取对应的信息创建解码器
        int result = -1;
        AVStream *videoStream = formatCtx->streams[videoIndex];

        //如果主动设置过旋转角度则将旋转信息设置到流信息中以便保存那边也应用(不需要保存也旋转可以注释)
        if (rotate != -1) {
            FFmpegHelper::setRotate(videoStream, rotate);
        }

        //先获取旋转角度(如果有旋转角度则不能用硬件加速)
        this->getRotate();
        if (rotate != 0) {
            hardware = "none";
        }

        //查找视频解码器(如果上面av_find_best_stream第五个参数传了则这里不需要)
        AVCodecID codecId = FFmpegHelper::getCodecId(videoStream);
        if (codecId == AV_CODEC_ID_NONE) {
            debug(result, "无视解码", "");
            return false;
        }

        //初始化解码器
        FFmpegThreadHelper::initVideoCodec(&videoCodec, codecId, videoCodecName, hardware);

        //创建视频解码器上下文
        videoCodecCtx = avcodec_alloc_context3(NULL);
        if (!videoCodecCtx) {
            debug(result, "创建视解", "");
            return false;
        }

        //将视频流的参数拷贝给视频解码器上下文/以便能够按照对应流参数进行解码
        result = FFmpegHelper::copyContext(videoCodecCtx, videoStream, false);
        if (result < 0) {
            debug(result, "视频参数", "");
            return false;
        }

        //初始化硬件加速(也可以叫硬解码/如果当前格式不支持硬解则立即切换到软解码)
        if (hardware != "none" && !FFmpegThreadHelper::initHardware(this, videoCodec, videoCodecCtx, hardware)) {
            hardware = "none";
            videoCodec = avcodec_find_decoder(codecId);
        }

        if (!videoCodec) {
            return false;
        }

        //设置低延迟和加速解码等参数(设置max_lowres的话很可能画面采用最小的分辨率)
        if (!getIsFile()) {
            //videoCodecCtx->lowres = videoCodec->max_lowres;
            videoCodecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
            videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            videoCodecCtx->flags2 |= AV_CODEC_FLAG2_FAST;
        }

        //打开视频解码器
        result = avcodec_open2(videoCodecCtx, videoCodec, &options);
        if (result < 0) {
            debug(result, "打开视解", "");
            return false;
        }

        if (videoCodecCtx->pix_fmt == AV_PIX_FMT_NONE) {
            debug(0, "格式为空", "");
            return false;
        }

        //获取分辨率大小
        FFmpegHelper::getResolution(videoStream, videoWidth, videoHeight);
        //如果没有获取到宽高则返回
        if (videoWidth == 0 || videoHeight == 0) {
            debug(0, "无分辨率", "");
            return false;
        }

        //记录首帧开始时间和解码器名称
        videoFirstPts = videoStream->start_time;
        videoCodecName = videoCodec->name;
        frameRate = FFmpegHelper::getFrameRate(videoStream, formatName);
        qint64 frames = videoStream->nb_frames;
        QMetaObject::invokeMethod(this, "setValue", Q_ARG(const char *, "frames"), Q_ARG(QVariant, frames));
        QString msg = QString("索引: %1 解码: %2 帧率: %3 宽高: %4x%5 角度: %6 帧数: %7 开始: %8").arg(videoIndex).arg(
                videoCodecName).arg(frameRate).arg(videoWidth).arg(videoHeight).arg(rotate).arg(frames).arg(
                videoFirstPts);
        debug(0, "视频信息", msg);
        //FFmpegUtil::getExtraData(videoCodecCtx);
    }

    return true;
}

bool FFmpegThread::initAudio() {
    //找到音频流索引
    audioIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIndex < 0) {
        //有些没有音频流所以这里不用返回
        audioIndex = -1;
        debug(0, "无音频流", "");
    } else {
        //如果手动指定了轨道则取指定的(节目流有多个轨道可以指定某个)
        if (audioTrack >= 0 && audioTracks.contains(audioTrack)) {
            audioIndex = audioTrack;
        }

        //取出流获取对应的信息创建解码器
        int result = -1;
        AVStream *audioStream = formatCtx->streams[audioIndex];

        //查找音频解码器(如果上面av_find_best_stream第五个参数传了则这里不需要)
        AVCodecID codecId = FFmpegHelper::getCodecId(audioStream);
        if (codecId == AV_CODEC_ID_NONE) {
            debug(result, "无音解码", "");
            return false;
        }

        audioCodec = avcodec_find_decoder(codecId);
        //audioCodec = avcodec_find_decoder_by_name("aac");
        if (!audioCodec) {
            return false;
        }

        //创建音频解码器上下文
        audioCodecCtx = avcodec_alloc_context3(audioCodec);
        if (!audioCodecCtx) {
            debug(result, "创建音解", "");
            return false;
        }

        //将音频流的参数拷贝给音频解码器上下文/以便能够按照对应流参数进行解码
        result = FFmpegHelper::copyContext(audioCodecCtx, audioStream, false);
        if (result < 0) {
            debug(result, "音频参数", "");
            return false;
        }

        //打开音频解码器
        result = avcodec_open2(audioCodecCtx, audioCodec, &options);
        if (result < 0) {
            debug(result, "打开音解", "");
            return false;
        }

        //记录首帧开始时间和解码器名称
        audioFirstPts = audioStream->start_time;
        audioCodecName = audioCodec->name;

        //获取音频相关参数
        profile = audioCodecCtx->profile;
        sampleRate = audioCodecCtx->sample_rate;
        //发现有些地址居然有6个声道
        channelCount = FFmpegHelper::getChannel(audioCodecCtx);

        //部分设备比如海康不支持48000采样率/大华的居然都支持/ffmpeg5开始又修复了
        if (mediaType == MediaType_Rtsp && sampleRate >= 48000 && mediaUrl.contains("/Streaming/Channels/")) {
            if (FFMPEG_VERSION_MAJOR < 5) {
                decodeAudio = false;
                audioIndex = -1;
            }
        }

        qint64 bitrate = FFmpegHelper::getBitRate(audioStream);
        qint64 frames = audioStream->nb_frames;
        QString msg = QString("索引: %1 解码: %2 比特: %3 声道: %4 采样: %5 品质: %6 帧数: %7 开始: %8").arg(
                audioIndex).arg(audioCodecName).arg(bitrate).arg(channelCount).arg(sampleRate).arg(profile).arg(
                frames).arg(audioFirstPts);
        debug(0, "音频信息", msg);
    }

    return true;
}

void FFmpegThread::initData() {
    if (videoIndex >= 0) {
        videoFrame = av_frame_alloc();
        tempFrame = av_frame_alloc();
        yuvFrame = av_frame_alloc();
        imageFrame = av_frame_alloc();

        //加速转码标志位
        int flags = FFmpegThreadHelper::getDecodeFlags(decodeType);
        //定义及获取像素格式/硬件加速需要指定格式
        AVPixelFormat srcFormat = (hardware == "none" ? videoCodecCtx->pix_fmt : AV_PIX_FMT_NV12);
        AVPixelFormat dstFormat = (videoMode == VideoMode_Painter ? AV_PIX_FMT_RGB24 : AV_PIX_FMT_YUV420P);
        bool ok = FFmpegThreadHelper::initVideoData(this, yuvFrame, imageFrame, &yuvSwsCtx, &imageSwsCtx, &imageData,
                                                    srcFormat, dstFormat, videoWidth, videoHeight, hardware, flags);
        if (!ok) {
            videoIndex = -1;
        }

        //滤镜相关帧处理
        maxFilterFrame = 10;
        filterFrameIndex = -1;
        for (int i = 0; i < maxFilterFrame; ++i) {
            filterFrames << av_frame_alloc();
        }
    }

    if (audioIndex >= 0) {
        audioFrame = av_frame_alloc();
        pcmFrame = av_frame_alloc();
        bool ok = FFmpegThreadHelper::initAudioData(this, audioCodecCtx, pcmFrame, &pcmSwrCtx, pcmSampleFormat,
                                                    pcmSampleRate, pcmChannels, audioCodecName);
        if (!ok) {
            audioIndex = -1;
        }
    }

    //分配数据包内存
    packet = FFmpegHelper::creatPacket(NULL);
    //校验音频保存格式/目前用的是ffmpeg的保存/这里其实完全可以不用
    FFmpegThreadHelper::checkSaveAudioType(mediaType, audioIndex, audioCodecName, formatCtx->bit_rate, saveAudioType);
}

void FFmpegThread::initAudioPlayer() {
    //设置是否需要计算音频振幅
    audioPlayer->setLevel(audioLevel);
    //初始化音频输出
    if (audioIndex >= 0) {
        QMetaObject::invokeMethod(audioPlayer, "open");
        this->initAudioOutput();
        this->initAudioEffect();
    }
}

void FFmpegThread::initAudioOutput() {
    //打开音频播放设备
    QMetaObject::invokeMethod(audioPlayer, "openAudioOutput", Q_ARG(QString, audioDevice), Q_ARG(int, pcmSampleRate),
                              Q_ARG(int, pcmChannels), Q_ARG(int, 16));
}

void FFmpegThread::initAudioEffect() {
    if (audioIndex >= 0) {
        //打开音效库/速度不为1则启用
        bool useSonic = (speed != 1);
        audioPlayer->openSonic(useSonic, pcmSampleRate, pcmChannels, speed);
    }
}

void FFmpegThread::initMetadata() {
    //限制本地mp3文件才需要读取
    if (!getIsMp3()) {
        return;
    }

    //去掉音频索引
    videoIndex = -1;

    //读取专辑信息
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(formatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        debug(0, "专辑信息", QString("名称: %1 键值: %2").arg(tag->key).arg(tag->value));
    }

    //取出专辑封面
    QImage image = FFmpegThreadHelper::getAttachedPic(formatCtx);
    if (!image.isNull()) {
        emit receiveImage(image, 0);
    }
}

void FFmpegThread::initFilter() {
    //存在视频则初始化视频滤镜用于标签和图形绘制
    videoFilter.init = true;
    if (videoIndex >= 0 && videoFilter.enable) {
        mutex.lock();
        //抓图和绘制模式不用旋转
        videoFilter.rotate = ((isSnap || videoMode == VideoMode_Painter) ? 0 : this->rotate);
        videoFilter.listOsd = listOsd;
        videoFilter.listGraph = listGraph;
        videoFilter.formatSrc = videoCodecCtx->pix_fmt;
        videoFilter.formatIn = (hardware == "none" ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_NV12);
        videoFilter.formatOut = (hardware == "none" ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_NV12);
        FFmpegFilter::initFilter(this, formatCtx->streams[videoIndex], videoCodecCtx, videoFilter);
        mutex.unlock();
    }
}

void FFmpegThread::resetFilter() {
    if (videoFilter.enable && isOk) {
        videoFilter.init = false;
    }
}

bool FFmpegThread::openVideo() {
    //先检查地址是否正常(文件是否存在或者网络地址是否可达)
    if (!VideoHelper::checkUrl(this, mediaType, mediaUrl, connectTimeout)) {
        return false;
    }

    //启动计时
    timer.start();

    //初始化参数
    this->initOption();

    //初始化输入
    if (!initInput()) {
        this->stop3();
        return false;
    }

    //初始化视频
    if (!initVideo()) {
        videoIndex = -1;
    }

    //很奇怪有些流可以获取到索引但是没有视频
    if (videoIndex >= 0 && videoWidth <= 0) {
        videoIndex = -1;
    }

    //初始化音频(有些设备的音频格式在ffmpeg中找不到对应的解码器/G.722.1)
    if (!initAudio()) {
        audioIndex = -1;
    }

    //初始化相关数据
    this->initData();
    //音视频都失败的时候不用继续
    if (videoIndex < 0 && audioIndex < 0) {
        this->stop3();
        return false;
    }

    //初始化音频播放
    this->initAudioPlayer();
    //获取专辑信息
    this->initMetadata();
    //初始化滤镜
    this->initFilter();

    //校验同步标志位
    FFmpegThreadHelper::checkUseSync(this, videoIndex, audioIndex, useSync);

    //启动音视频同步线程
    if (useSync) {
        if (audioIndex >= 0) {
            audioSync->start();
        }
        if (videoIndex >= 0) {
            videoSync->start();
        }
    }

    this->openFinsh(true);
    this->checkOnlyAudio();
    return isOk;
}

void FFmpegThread::closeVideo() {
    speed = 1.0;
    tryOpen = false;
    tryRead = false;
    changePosition = false;

    keyPacketIndex = 0;
    keyPacketCount = 0;
    realPacketSize = 0;
    realPacketCount = 0;

    videoFirstPts = 0;
    audioFirstPts = 0;

    //停止解码同步线程
    audioSync->stop();
    videoSync->stop();

    //先停止录制
    this->recordStop();
    //搞个标志位判断是否需要调用父类的释放(可以防止重复调用)
    bool needClose = formatCtx;

    if (packet) {
        FFmpegHelper::freePacket(packet);
        packet = NULL;
    }

    if (yuvSwsCtx) {
        sws_freeContext(yuvSwsCtx);
        yuvSwsCtx = NULL;
    }

    if (imageSwsCtx) {
        sws_freeContext(imageSwsCtx);
        imageSwsCtx = NULL;
    }

    if (pcmSwrCtx) {
        swr_free(&pcmSwrCtx);
        pcmSwrCtx = NULL;
    }

    if (videoFrame) {
        FFmpegHelper::freeFrame(videoFrame);
        videoFrame = NULL;
    }

    if (tempFrame) {
        FFmpegHelper::freeFrame(tempFrame);
        tempFrame = NULL;
    }

    if (yuvFrame) {
        FFmpegHelper::freeFrame(yuvFrame);
        yuvFrame = NULL;
    }

    if (imageFrame) {
        FFmpegHelper::freeFrame(imageFrame);
        imageFrame = NULL;
    }

    if (imageData) {
        av_free(imageData);
        imageData = NULL;
    }

    if (audioFrame) {
        FFmpegHelper::freeFrame(audioFrame);
        audioFrame = NULL;
    }

    if (pcmFrame) {
        FFmpegHelper::freeFrame(pcmFrame);
        pcmFrame = NULL;
    }

            foreach (AVFrame *frame, filterFrames) {
            FFmpegHelper::freeFrame2(&frame);
        }

    mutex.lock();
    FFmpegFilter::freeFilter(videoFilter);
    mutex.unlock();

    filterFrames.clear();
    filterFramex.clear();
    this->clearBuffer(true);

    //videoCodec会跟着自动释放
    if (videoCodecCtx) {
        avcodec_free_context(&videoCodecCtx);
        videoCodec = NULL;
        videoCodecCtx = NULL;
    }

    //audioCodec会跟着自动释放
    if (audioCodecCtx) {
        avcodec_free_context(&audioCodecCtx);
        audioCodec = NULL;
        audioCodecCtx = NULL;
    }

    if (options) {
        av_dict_free(&options);
        options = NULL;
    }

    if (formatCtx) {
        avformat_close_input(&formatCtx);
        avformat_free_context(formatCtx);
        formatCtx = NULL;
    }

    if (needClose) {
        VideoThread::closeVideo();
    }
}

void FFmpegThread::lockFrame() {
    if (lockData) {
        mutexFrame.lock();
    }
}

void FFmpegThread::unlockFrame() {
    if (lockData) {
        mutexFrame.unlock();
    }
}

void FFmpegThread::setLockData(bool lockData) {
    this->lockData = lockData;
}

bool FFmpegThread::setValue(const char *name, const QVariant &value) {
    return this->setProperty(name, value);
}

QImage FFmpegThread::getImage() {
    //还没有开始或者没有图片转换对象则不用继续/说明是纯音频
    QImage image;
    if (!isOk || !imageSwsCtx) {
        return image;
    }

    //处于抓图和绘制模式不用继续/那边传出去的就是图片
    if (isSnap || videoMode == VideoMode_Painter) {
        return image;
    }

    //将数据转成图片/这里必须用最原始的数据帧/不能是加上水印后的
    this->lockFrame();
    image = FFmpegThreadHelper::frameToImage(imageSwsCtx, videoFrame, imageFrame, imageData);
    this->unlockFrame();
    if (image.isNull()) {
        return image;
    }

    //如果有旋转角度先要旋转
    VideoHelper::rotateImage(rotate, image);
    return image;
}

qint64 FFmpegThread::getStartTime() {
    return this->startTime;
}

QDateTime FFmpegThread::getLastTime() {
    return this->lastTime;
}

FFmpegSave *FFmpegThread::getSaveFile() {
    return this->saveFile;
}

AVStream *FFmpegThread::getVideoStream() {
    if (videoIndex >= 0) {
        return formatCtx->streams[videoIndex];
    } else {
        return NULL;
    }
}

AVStream *FFmpegThread::getAudioStream() {
    if (audioIndex >= 0) {
        return formatCtx->streams[audioIndex];
    } else {
        return NULL;
    }
}

bool FFmpegThread::getPushPreview() {
    return this->pushPreview;
}

void FFmpegThread::setPushPreview(bool pushPreview) {
    this->pushPreview = pushPreview;
}

void FFmpegThread::setSendPacket(bool sendPacket) {
    this->sendPacket = sendPacket;
}

void FFmpegThread::setSendFrame(bool sendFrame) {
    this->sendFrame = sendFrame;
}

bool FFmpegThread::getTryOpen() {
    return this->tryOpen;
}

bool FFmpegThread::getTryRead() {
    return this->tryRead;
}

bool FFmpegThread::getTryStop() {
    return this->stopped;
}

void FFmpegThread::clearBuffer(bool direct) {
    if (direct) {
        needClear = false;
        if (videoCodecCtx) {
            avcodec_flush_buffers(videoCodecCtx);
        }
        if (audioCodecCtx) {
            avcodec_flush_buffers(audioCodecCtx);
        }
    } else {
        needClear = true;
    }
}

void FFmpegThread::clearAndReset() {
    audioSync->clear();
    videoSync->clear();
    audioSync->reset();
    videoSync->reset();
}

int FFmpegThread::getRotate() {
    //不是默认值说明已经获取过旋转角度不用再去获取
    if (rotate != -1) {
        return rotate;
    }

    rotate = 0;
    if (videoIndex >= 0) {
        rotate = FFmpegHelper::getRotate(formatCtx->streams[videoIndex]);
        if (rotate == 90 || rotate == 270) {
            emit receiveSizeChanged();
        }
    }

    return rotate;
}

void FFmpegThread::setRotate(int rotate) {
    if (rotate < 0) {
        return;
    }

    //只有变化了才需要触发
    bool changed = (this->rotate != rotate);
    this->rotate = rotate;
    if (isOk && rotate >= 0 && changed) {
        //旋转通过滤镜实现效果
        this->resetFilter();
        QMetaObject::invokeMethod(this, "receiveSizeChanged");
    }
}

qint64 FFmpegThread::getDuration() {
    return duration;
}

qint64 FFmpegThread::getPosition() {
    return position;
}

void FFmpegThread::setPosition(qint64 position) {
    if (isOk && getIsFile()) {
        //设置切换进度标志位以便暂停解析
        changePosition = true;
        //用定时器设置有个好处避免重复频繁设置进度(频繁设置可能会崩溃)
        this->position = position;
        timerPosition->stop();
        timerPosition->start();
    }
}

void FFmpegThread::setPosition() {
    if (!isOk) {
        return;
    }

    //清空同步线程缓存数据
    audioSync->clear();
    videoSync->clear();
    //清空解码缓存数据
    this->clearBuffer(true);

    //发过来的是毫秒而参数需要微秒(有些文件开始时间不是0所以需要加上该时间)
    //asf文件开始时间是一个很大的负数这里不需要加上
    qint64 timestamp = position * 1000;
    qint64 timestart = formatCtx->start_time;
    if (timestart > 0) {
        timestamp += timestart;
    }

    //AVSEEK_FLAG_BACKWARD=往前找最近的关键帧/AVSEEK_FLAG_ANY=直接定位到指定帧不管画面是否清晰
    av_seek_frame(formatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    //avformat_seek_file(formatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);

    //如果处于暂停阶段在等待新进度切换好以后需要再次暂停
    if (isPause) {
        timerPause->stop();
        timerPause->start();
    }

    //继续播放以便切换到当前新进度的位置
    this->next();
}

double FFmpegThread::getSpeed() {
    return this->speed;
}

void FFmpegThread::setSpeed(double speed) {
    //启用了音视频同步的h264裸流也需要设置
    if (isOk && this->speed != speed && (getIsFile() || (useSync && (formatName == "h264" || formatName == "hevc")))) {
        this->pause();
        this->speed = speed;
        this->initAudioEffect();
        this->next();
    }
}

int FFmpegThread::getVolume() {
    return audioPlayer->getVolume();
}

void FFmpegThread::setVolume(int volume) {
    audioPlayer->setVolume(volume);
}

bool FFmpegThread::getMuted() {
    return audioPlayer->getMuted();
}

void FFmpegThread::setMuted(bool muted) {
    audioPlayer->setMuted(muted);
}

void FFmpegThread::setAudioLevel(bool audioLevel) {
    this->audioLevel = audioLevel;
    audioPlayer->setLevel(audioLevel);
}

void FFmpegThread::setRealBitRate(bool realBitRate) {
    realPacketSize = 0;
    realPacketCount = 0;
    this->realBitRate = realBitRate;
}

void FFmpegThread::setAudioDevice(const QString &audioDevice) {
    //默认设备不用处理
    if (audioDevice == "default") {
        return;
    }

    this->audioDevice = audioDevice;
    if (audioIndex < 0 || !audioPlayer || audioDevice.isEmpty()) {
        return;
    }

    this->initAudioOutput();
}

void FFmpegThread::setCrop(bool isCrop) {
    this->isCrop = isCrop;
    if (!isCrop) {
        cropTime = QDateTime::currentDateTime();
    }
}

void FFmpegThread::setFlag(const QString &flag) {
    //先执行父类的设置
    AbstractVideoThread::setFlag(flag);
    //设置音频对象名称
    audioPlayer->setObjectName("audioPlayer_" + flag);
}

void FFmpegThread::debug(int result, const QString &head, const QString &msg) {
    if (result < 0) {
        QString text = (msg.isEmpty() ? "" : (" " + msg));
        VideoThread::debug(head, QString("错误: %1%2").arg(FFmpegHelper::getError(result)).arg(text));
    } else {
        VideoThread::debug(head, msg);
    }
}

void FFmpegThread::pause() {
    if (this->isRunning()) {
        isPause = true;
    }
}

void FFmpegThread::next() {
    if (this->isRunning()) {
        isPause = false;
        changePosition = false;
        //复位同步线程(不复位继续播放后会瞬间快速跳帧)
        audioSync->reset();
        videoSync->reset();
    }
}

void FFmpegThread::step(bool backward) {
    if (getIsFile() && playStep) {
        this->next();
    }
}

void FFmpegThread::initSaveFile() {
    bool mp4ToAnnexB = false;
    bool audioEncode = false;
    bool videoEncode = false;

    //有正确的帧率则就用该帧率/否则用设置的帧率
    encodeVideoFps = (frameRate <= 120 ? frameRate : encodeVideoFps);
    FFmpegThreadHelper::checkVideoEncode(mediaType, mediaUrl, saveVideoType, formatName, getIsFile(), mp4ToAnnexB,
                                         videoEncode);
    saveFile->setEncodePara(mp4ToAnnexB, audioEncode, videoEncode, encodeSpeed, encodeAudio, encodeVideo,
                            encodeVideoFps, encodeVideoRatio, encodeVideoScale);
    saveFile->setSavePara(mediaType, saveVideoType, getVideoStream(), getAudioStream());

    //设置加密秘钥字串
    QByteArray cryptoKey = this->property("encryptKey").toByteArray();
    saveFile->setProperty("encryptKey", cryptoKey);

    //设置一些弱属性方便其他地方处理
    saveFile->setProperty("mediaUrl", FFmpegHelper::getUrl(formatCtx));
    saveFile->setProperty("isFile", getIsFile());

    //告诉保存那边这边传过去的真实的音频数据相关参数
    saveFile->setProperty("resample", (pcmSwrCtx != NULL));
    saveFile->setProperty("sampleRate", pcmSampleRate);
    saveFile->setProperty("channelCount", pcmChannels);
}

void FFmpegThread::recordStart(const QString &fileName) {
    AbstractVideoThread::recordStart(fileName);
    if (saveVideoType > 1) {
        this->setFileName(fileName);
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveFile->getIsPause()) {
            isRecord = true;
            saveFile->pause();
            emit recorderStateChanged(RecorderState_Recording, saveFile->getFileName());
        } else {
            this->initSaveFile();
            saveFile->open(this->fileName);
            if (saveFile->getIsOk()) {
                isRecord = true;
                emit recorderStateChanged(RecorderState_Recording, saveFile->getFileName());
            }
        }
    }
}

void FFmpegThread::recordPause() {
    AbstractVideoThread::recordPause();
    if (saveVideoType > 1) {
        if (saveFile->getIsOk()) {
            isRecord = false;
            saveFile->pause();
            emit recorderStateChanged(RecorderState_Paused, saveFile->getFileName());
        }
    }
}

void FFmpegThread::recordStop() {
    AbstractVideoThread::recordStop();
    if (saveVideoType > 1) {
        if (saveFile->getIsOk()) {
            //需要等缓存中的所有数据存储完再停止
            while (saveFile->getPacketCount() > 0 || saveFile->getFrameCount() > 0) {
                msleep(10);
            }

            isRecord = false;
            saveFile->stop();
            emit recorderStateChanged(RecorderState_Stopped, saveFile->getFileName());
        }
    }
}

void FFmpegThread::setOsdInfo(const QList<OsdInfo> &listOsd) {
    if (!FFmpegThreadHelper::checkFilter(false, videoCodecName, hardware, mediaType, mediaUrl)) {
        return;
    }

    mutex.lock();
    this->listOsd = listOsd;
    this->resetFilter();
    mutex.unlock();
}

void FFmpegThread::setGraphInfo(const QList<GraphInfo> &listGraph) {
    if (!FFmpegThreadHelper::checkFilter(true, videoCodecName, hardware, mediaType, mediaUrl)) {
        return;
    }

    mutex.lock();
    this->listGraph = listGraph;
    this->resetFilter();
    mutex.unlock();
}
