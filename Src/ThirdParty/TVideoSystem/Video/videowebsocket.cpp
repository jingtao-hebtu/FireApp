#include "videowebsocket.h"
#include "videohead.h"

VideoWebSocket::VideoWebSocket(QObject *parent) : QObject(parent) {
    webSocket = NULL;
#ifdef websocket
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
#endif
}

void VideoWebSocket::initWeb() {
#ifdef websocket
    if (!webSocket) {
        webSocket = new QWebSocket("WebSocket", QWebSocketProtocol::VersionLatest);
        connect(webSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SIGNAL(binaryMessageReceived(QByteArray)), Qt::DirectConnection);
    }
#endif
}

void VideoWebSocket::freeWeb() {
#ifdef websocket
    if (webSocket) {
        disconnect(webSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SIGNAL(binaryMessageReceived(QByteArray)));
        this->closeWeb();
        webSocket->deleteLater();
        webSocket = NULL;
    }
#endif
}

//有部分场景是需要通过私有协议拿到播放地址
#ifdef webhelper
#include "webhelper.h"
#endif

void VideoWebSocket::openWeb(const QString &mediaUrl, int timeout) {
#ifdef websocket
    QString url = mediaUrl;
#ifdef webhelper
    url = WebHelper::checkUrl(url, timeout);
#endif
    if (!url.isEmpty() && webSocket) {
        webSocket->open(QUrl(url));
    }
#endif
}

void VideoWebSocket::closeWeb() {
#ifdef websocket
    //如果不加判断直接关闭会提示 QNativeSocketEngine::write() was not called in QAbstractSocket::ConnectedState
    if (webSocket && webSocket->state() != QAbstractSocket::ConnectingState) {
        webSocket->close();
    }
#endif
}
