#ifndef VIDEOWEBSOCKET_H
#define VIDEOWEBSOCKET_H

#include <QObject>

#ifdef websocket
#include <QWebSocket>
#else

class QWebSocket;

#endif

class VideoWebSocket : public QObject {
Q_OBJECT

public:
    explicit VideoWebSocket(QObject *parent = 0);

private:
    //通信对象
    QWebSocket *webSocket;

public slots:

    //初始化和释放对象
    void initWeb();

    void freeWeb();

    //打开和关闭地址
    void openWeb(const QString &mediaUrl, int timeout = 1500);

    void closeWeb();

signals:

    //收到二进制数据
    void binaryMessageReceived(const QByteArray &message);
};

#endif // VIDEOWEBSOCKET_H
