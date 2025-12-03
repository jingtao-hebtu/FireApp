#include "ffmpegavio.h"
#include "videowebsocket.h"

void FFmpegAvio::init(FFmpegAvio **obj, const QString &url, const QString &flag, MediaType mediaType) {
    if (mediaType == MediaType_WebSocket) {
        if (!(*obj)) {
            (*obj) = new FFmpegAvio;
            (*obj)->setObjectName(QString("FFmpegAvio_%1").arg(flag));
        }

        (*obj)->setUrl(url);
        (*obj)->start();
    }
}

void FFmpegAvio::free(FFmpegAvio **obj) {
    if ((*obj)) {
        (*obj)->quit();
        (*obj)->wait();
        (*obj)->free();
        (*obj)->deleteLater();
        (*obj) = NULL;
    }
}

FFmpegAvio::FFmpegAvio(QObject *parent) : QThread(parent) {
    //可以自行调整下面几个值
    connTimeout = 1500;
    //值越小打开速度越快/分辨率越大值建议越大
    minSizeOpen = 100 * 1024;
    //值越小越容易花屏/控制多大缓存才能读取
    minSizeRead = 1 * 1024;
    //整个缓存队列的大小
    fifoSize = 10 * 1024 * 1024;

    fifo = NULL;
    avioCtx = NULL;

    //实例化网络数据通信对象
    videoWeb = new VideoWebSocket(this);
    connect(videoWeb, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(append(QByteArray)));
}

FFmpegAvio::~FFmpegAvio() {
}

void FFmpegAvio::run() {
    videoWeb->initWeb();
    videoWeb->closeWeb();
    this->freeFifo();
    this->initFifo();
    videoWeb->openWeb(this->url, this->connTimeout);
    this->exec();
}

void FFmpegAvio::initFifo() {
    if (!fifo) {
#if (FFMPEG_VERSION_MAJOR < 5)
        fifo = av_fifo_alloc(fifoSize);
#else
        fifo = av_fifo_alloc2(fifoSize, sizeof(uint8_t), AV_FIFO_FLAG_AUTO_GROW);
#endif
        uint8_t *avioBuf = (uint8_t *) av_malloc(fifoSize);
        avioCtx = avio_alloc_context(avioBuf, fifoSize, 0, fifo, &read_packet, NULL, NULL);
    }
}

void FFmpegAvio::freeFifo() {
    if (fifo) {
#if (FFMPEG_VERSION_MAJOR < 5)
        av_fifo_reset(fifo);
        av_fifo_freep(&fifo);
#else
        av_fifo_reset2(fifo);
        av_fifo_freep2(&fifo);
#endif

        av_freep(&avioCtx->buffer);
#if (FFMPEG_VERSION_MAJOR < 3)
        av_free(avioCtx);
#else
        avio_context_free(&avioCtx);
#endif
        fifo = NULL;
    }
}

void FFmpegAvio::append(const QByteArray &message) {
    int size = message.size();
    const void *data = message.data();
#if (FFMPEG_VERSION_MAJOR < 5)
    int num = av_fifo_space(fifo);
    int ret = av_fifo_generic_write(fifo, (void *)data, size, NULL);
#else
    int num = static_cast<int>(av_fifo_can_write(fifo));
    int ret = static_cast<int>(av_fifo_write(fifo, data, size));
#endif
    Q_UNUSED(num);
    Q_UNUSED(ret);
    //qDebug() << TIMEMS << num << ret << size;
}

int FFmpegAvio::read_packet(void *opaque, uint8_t *buf, int size) {
    AVFifox *fifo = (AVFifox *) opaque;
    int ret = -1;
    if (!fifo) {
        return ret;
    }

#if (FFMPEG_VERSION_MAJOR < 5)
    int availableBytes = av_fifo_size(fifo);
#else
    int availableBytes = static_cast<int>(av_fifo_can_read(fifo));
#endif
    if (availableBytes <= 0) {
        return ret;
    }

    int len = qMin(availableBytes, size);
#if (FFMPEG_VERSION_MAJOR < 5)
    ret = av_fifo_generic_read(fifo, buf, len, NULL);
#else
    ret = av_fifo_read(fifo, buf, len);
#endif
    return len;
}

bool FFmpegAvio::canOpen() {
    return this->checkData(minSizeOpen);
}

bool FFmpegAvio::canRead() {
    return this->checkData(minSizeRead);
}

bool FFmpegAvio::checkData(int size) {
    if (!fifo) {
        return false;
    }

#if (FFMPEG_VERSION_MAJOR < 5)
    return (av_fifo_size(fifo) > size);
#else
    return (av_fifo_can_read(fifo) > size);
#endif
}

AVIOContext *FFmpegAvio::getCtx() {
    return this->avioCtx;
}

void FFmpegAvio::setUrl(const QString &video_url) {
    this->url = video_url;
}

void FFmpegAvio::reopen() {
    this->quit();
    this->wait();
    this->start();
}

void FFmpegAvio::free() {
    videoWeb->freeWeb();
    this->freeFifo();
}
