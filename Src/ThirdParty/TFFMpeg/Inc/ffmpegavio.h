#ifndef EP_FFMPEGAVIO_H_VIEWER
#define EP_FFMPEGAVIO_H_VIEWER

#include "ffmpeginclude.h"
#include "videostruct.h"
#include "qthread.h"

class VideoWebSocket;

class FFmpegAvio : public QThread {
Q_OBJECT

public:
    //初始化和释放
    static void init(FFmpegAvio **obj, const QString &url, const QString &flag, MediaType mediaType);

    static void free(FFmpegAvio **obj);

    explicit FFmpegAvio(QObject *parent = nullptr);

    ~FFmpegAvio();

protected:
    void run();

private:
    //通信地址
    QString url;
    //网络通信对象
    VideoWebSocket *videoWeb;

    //连接请求超时时间
    int connTimeout;
    //打开最小缓存
    int minSizeOpen;
    //读取最小缓存
    int minSizeRead;
    //分配缓存大小
    int fifoSize;

    //缓存数据
    AVFifox *fifo;
    //操作上下文
    AVIOContext *avioCtx;

private:
    //初始化和释放对象
    void initFifo();

    void freeFifo();

private slots:

    //添加流数据
    void append(const QByteArray &message);

public:
    //读取数据回调
    static int read_packet(void *opaque, uint8_t *buf, int size);

    //根据缓存数据大小判断是否可以打开和读取
    bool canOpen();

    bool canRead();

    bool checkData(int size);

    //返回对象
    AVIOContext *getCtx();

    //设置地址
    void setUrl(const QString &video_url);

    //重新打开
    Q_INVOKABLE void reopen();
    //释放资源
    Q_INVOKABLE void free();
};

#endif // FFMPEGAVIO_H
