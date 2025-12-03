#include "urlhelper.h"
#include "qtcpsocket.h"
#include "qregexp.h"

QStringList UrlHelper::getCompany() {
    QStringList listId, listName;
    UrlHelper::getCompany(listId, listName);
    return listName;
}

void UrlHelper::getCompany(QStringList &listId, QStringList &listName) {
    listId.clear();
    listName.clear();
    listId << "1" << "2" << "3" << "4" << "5" << "10" << "11" << "12";
    listName << "海康" << "大华" << "宇视" << "华为" << "天地" << "常规" << "其他" << "文件";
}

void UrlHelper::loadCompany(QComboBox *cbox, bool file) {
    QStringList listId, listName;
    UrlHelper::getCompany(listId, listName);

    cbox->clear();
    int count = listId.count() - (file ? 0 : 1);
    for (int i = 0; i < count; ++i) {
        cbox->addItem(listName.at(i), listId.at(i));
    }
}

void UrlHelper::getOnvifString(CompanyType company, const QString &ip, bool nvr, int index, QString &ipcType,
                               QString &onvifAddr, QString &profileToken, QString &videoSource) {
    //可以自行增加其他厂家的格式
    onvifAddr = QString("http://%1/onvif/device_service").arg(ip);
    if (company == CompanyType_HaiKang) {
        ipcType = "HIKVISION";
        profileToken = "Profile_1";
        videoSource = "VideoSource_1";
    } else if (company == CompanyType_DaHua) {
        ipcType = "Dahua";
        profileToken = "Profile000";
        videoSource = "VideoSource000";
    } else if (company == CompanyType_UniView) {
        ipcType = "UniView";
    } else if (company == CompanyType_HuaWei) {
        ipcType = "HuaWei";
    } else if (company == CompanyType_Tiandy) {
        ipcType = "Tiandy";
    }

    //通道递增一般是NVR/对应的标识符又不一样
    if (nvr) {
        ipcType = QString("NVR_Ch%1_%2").arg(index + 1).arg(ipcType);
        if (company == CompanyType_HaiKang) {
            QString flag = QString("%1").arg(index + 1, 3, 10, QChar('0'));
            profileToken = "ProfileToken" + flag;
            videoSource = "VideoSourceToken" + flag;
        } else if (company == CompanyType_DaHua) {
            QString flag = QString("%1%2").arg(index, 3, 10, QChar('0')).arg("00");
            profileToken = "MediaProfile" + flag;
            videoSource = flag;
        }
    }
}

//参考链接 https://blog.csdn.net/immerseinstudy/article/details/139395513
QString UrlHelper::getRtspUrl(CompanyType company, const QString &userName, const QString &userPwd, int channel,
                              int streamType) {
    QString url;
    channel = channel + 1;
    QString head = QString("rtsp://%1:%2@[Addr]").arg(userName).arg(userPwd);
    if (company == CompanyType_HaiKang) {
        //?transportmode=unicast
        url = QString("/Streaming/Channels/%1%2").arg(channel).arg(streamType + 1, 2, 10, QChar('0'));
    } else if (company == CompanyType_DaHua) {
        //&unicast=true&proto=Onvif
        url = QString("/cam/realmonitor?channel=%1&subtype=%2").arg(channel).arg(streamType);
    } else if (company == CompanyType_UniView) {
        url = QString("/media/video%1").arg(streamType + 1);
    } else if (company == CompanyType_HuaWei) {
        url = QString("/LiveMedia/ch%1/Media%2").arg(channel).arg(streamType + 1);
    } else if (company == CompanyType_Tiandy) {
        url = QString("/%1/%2").arg(channel).arg(streamType + 1);
    } else if (company == CompanyType_Normal) {
        url = QString("/live?channel=%1&stream=%2").arg(channel).arg(streamType);
    } else if (company == CompanyType_File) {
        head = "f:/mp4/push/[Addr].mp4";
    } else {
        url = QString("/%1").arg(streamType);
    }

    return head + url;
}

QString UrlHelper::getRtspUrl(const UrlPara &urlPara) {
    //实时视频流则直接取
    if (urlPara.videoType == 0) {
        QString url = UrlHelper::getRtspUrl(urlPara.companyType, urlPara.userName, urlPara.userPwd, urlPara.channel,
                                            urlPara.streamType);
        url.replace("[Addr]", urlPara.deviceIP);
        return url;
    }

    //头部地址格式完全一致
    QString url;
    int channel = urlPara.channel + 1;
    QString head = QString("rtsp://%1:%2@%3:554").arg(urlPara.userName).arg(urlPara.userPwd).arg(urlPara.deviceIP);
    if (urlPara.companyType == CompanyType_HaiKang) {
        //实时预览格式 rtsp://admin:12345@192.168.1.128:554/Streaming/Channels/101?transportmode=unicast
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/Streaming/tracks/101?starttime=20120802t063812z&endtime=20120802t064816z
        //流媒体视频流 rtsp://172.6.24.15:554/Devicehc8://172.6.22.106:8000:0:0?username=admin&password=12345
        //日期时间格式 ISO 8601 表示Zulu(GMT) 时间 YYYYMMDD”T”HHmmSS.fraction”Z”，
        //unicast表示单播/multicast表示多播/默认单播可以省略
        //101解析: 1是通道号 01是通道的码流编号 也可以是02 03
        QString startTimeISO = urlPara.dateTimeStart.toString(Qt::ISODate);
        startTimeISO.replace("-", "");
        startTimeISO.replace(":", "");
        startTimeISO = startTimeISO.toLower();

        QString endTimeISO = urlPara.dateTimeEnd.toString(Qt::ISODate);
        endTimeISO.replace("-", "");
        endTimeISO.replace(":", "");
        endTimeISO = endTimeISO.toLower();

        //通道号和码流编号
        QString info = QString("%1%2%3").arg(channel).arg(0).arg(urlPara.streamType + 1);
        //回放时间范围
        QString time = QString("starttime=%1z&endtime=%2z").arg(startTimeISO).arg(endTimeISO);
        //最终字符串
        url = QString("%1/Streaming/tracks/%2?%3").arg(head).arg(info).arg(time);
    } else if (urlPara.companyType == CompanyType_DaHua) {
        //实时预览格式 rtsp://admin:12345@192.168.1.128:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/cam/playback?channel=1&subtype=0&starttime=2021_03_18_11_36_01&endtime=2021_03_18_12_05_01
        QString startTimeStr = urlPara.dateTimeStart.toString("yyyy_MM_dd_HH_mm_ss");
        QString endTimeStr = urlPara.dateTimeEnd.toString("yyyy_MM_dd_HH_mm_ss");
        //通道号和码流编号
        QString info = QString("channel=%1&subtype=%2").arg(channel).arg(urlPara.streamType);
        //回放时间范围
        QString time = QString("starttime=%1&endtime=%2").arg(startTimeStr).arg(endTimeStr);
        //最终字符串
        url = QString("%1/cam/playback?%2&%3").arg(head).arg(info).arg(time);
    } else {
        //实时预览格式 rtsp://admin:12345@192.168.1.128:554/live?channel=1&stream=1
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/file?channel=1&start=1494485280&stop=1494485480
        //先转换时间戳/1970年到该时间经过的秒数
        qint64 startTimeSec = urlPara.dateTimeStart.toMSecsSinceEpoch() / 1000;
        qint64 stopTimeSec = urlPara.dateTimeEnd.toMSecsSinceEpoch() / 1000;
        //回放时间范围
        QString time = QString("start=%1&stop=%2").arg(startTimeSec).arg(stopTimeSec);
        //最终字符串
        url = QString("%1/file?channel=%2&%3").arg(head).arg(channel).arg(time);
    }

    //还有一种通用格式 rtsp://admin:12345@192.168.1.128:554/0  0-主码流 1-子码流
    return url;
}

bool UrlHelper::checkUrl(const QString &url, int timeout) {
    //没有超时时间则认为永远返回真
    if (timeout <= 0) {
        return true;
    }

    //找出IP地址和端口号
    QString ip = UrlHelper::getUrlIP(url);
    int port = UrlHelper::getUrlPort(url);
    //取到了IP地址则判断IP地址(否则是网址暂时不判断)
    if (ip == url) {
        return true;
    }

    //局部的事件循环不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    QObject::connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(ip, port);
    eventLoop.exec();

    //超时没有连接上则判断该设备不在线
    if (tcpSocket.state() != QAbstractSocket::ConnectedState) {
        //qDebug() << TIMEMS << QString("连接失败 -> 地址: %1").arg(mediaUrl);;
        return false;
    }

    return true;
}

void UrlHelper::checkPrefix(QString &url) {
    //网络地址前缀全部转小写
    QStringList list = url.split("://");
    if (list.count() == 2) {
        url = QString("%1://%2").arg(list.at(0).toLower()).arg(list.at(1));
    }
}

bool UrlHelper::isIP(const QString &ip) {
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}

void UrlHelper::removeDefaultPort(QString &url) {
    if (url.startsWith("rtsp://")) {
        if (url.contains(":554/")) {
            url.replace(":554/", "/");
        }
    } else if (url.startsWith("rtmp://")) {
        if (url.contains(":1935/")) {
            url.replace(":1935/", "/");
        }
    } else if (url.startsWith("http://")) {
        if (url.contains(":80/")) {
            url.replace(":80/", "/");
        }
    }
}

QString UrlHelper::getAddr(const QString &url) {
    //本地文件则取末尾
    if (QFile(url).exists() && !url.contains("/dev/")) {
        return url.split("/").last();
    } else {
        return UrlHelper::getUrlIP(url);
    }
}

QString UrlHelper::getUrlHost(const QString &url) {
    //先获取IP地址
    QString host = getUrlIP(url);
    //如果非IP地址则取出中间主机部分
    if (host.split(".").first().toInt() == 0) {
        host = host.split("/").at(2);
        //也可能带了端口号也要去掉
        host = host.split(":").at(0);
    }
    return host;
}

QString UrlHelper::getUrlIP(const QString &url) {
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    int start = regExp.indexIn(url);
    int length = regExp.matchedLength();
    return url.mid(start, length);
}

int UrlHelper::getUrlPort(const QString &url) {
    //必须是最后一个:符号(用户可能地址中带了用户信息用:作为分隔符)
    int index = url.lastIndexOf(":");
    QString temp = url.mid(index + 1, 6);
    QStringList list = temp.split("/");
    int port = list.at(0).toInt();
    if (port == 0) {
        if (url.startsWith("rtsp://")) {
            port = 554;
        } else if (url.startsWith("rtmp://")) {
            port = 1935;
        } else if (url.startsWith("http://")) {
            port = 80;
        } else if (url.contains("s://")) {
            port = 443;
        }
    }
    return port;
}

void UrlHelper::getUserInfo(const QString &url, QString &userName, QString &userPwd) {
    userName = "admin";
    userPwd = "123456";
    //必须是最后一个@符号(因为用户名称和密码也可能是这个字符)
    int index = url.lastIndexOf("@");
    if (index > 0) {
        QString userInfo = url.mid(0, index);
        userInfo.replace("rtsp://", "");
        QStringList list = userInfo.split(":");
        userName = list.at(0);
        userPwd = list.at(1);
    }
}

QString UrlHelper::getCompanyName(const QString &url) {
    QString companyName;
    if (url.contains("/Streaming/Channels/")) {
        companyName = "HIKVISION";
    } else if (url.contains("/cam/realmonitor?channel=")) {
        companyName = "Dahua";
    } else if (url.contains("/media/video")) {
        companyName = "UNIVIEW";
    } else if (url.contains("/live?channel=")) {
        companyName = "General";
    }
    return companyName;
}

CompanyType UrlHelper::getCompanyType(const QString &url) {
    CompanyType companyType = CompanyType_Other;
    if (url.contains("/Streaming/Channels/")) {
        companyType = CompanyType_HaiKang;
    } else if (url.contains("/cam/realmonitor?channel=")) {
        companyType = CompanyType_DaHua;
    } else if (url.contains("/media/video")) {
        companyType = CompanyType_UniView;
    } else if (url.contains("/live?channel=")) {
        companyType = CompanyType_Normal;
    }
    return companyType;
}

void UrlHelper::getOtherInfo(const QString &url, int &channel, int &streamType) {
    CompanyType companyType = getCompanyType(url);
    QString temp = url.split("/").last();
    if (companyType == CompanyType_HaiKang) {
        temp = temp.split("?").first();
        //101=通道1码流01 1602=通道16码流02
        channel = temp.mid(0, temp.length() - 2).toInt();
        streamType = temp.right(2).toInt();
    } else if (companyType == CompanyType_DaHua) {
        temp = temp.split("?").last();
        QStringList list = temp.split("&");
                foreach (QString text, list) {
                int value = text.split("=").last().toInt();
                if (text.startsWith("channel")) {
                    channel = value;
                } else if (text.startsWith("subtype")) {
                    streamType = value;
                }
            }
    } else if (companyType == CompanyType_UniView) {
        streamType = temp.mid(5, 2).toInt();
    } else if (companyType == CompanyType_Normal) {
        temp = temp.split("?").last();
        QStringList list = temp.split("&");
                foreach (QString text, list) {
                int value = text.split("=").last().toInt();
                if (text.startsWith("channel")) {
                    channel = value;
                } else if (text.startsWith("stream")) {
                    streamType = value;
                }
            }
    } else {
        streamType = temp.toInt();
    }
}

void UrlHelper::getUrlPara(const QString &url, UrlPara &urlPara) {
    urlPara.deviceIP = getUrlIP(url);
    urlPara.devicePort = getUrlPort(url);
    getUserInfo(url, urlPara.userName, urlPara.userPwd);
    urlPara.companyName = getCompanyName(url);
    urlPara.companyType = getCompanyType(url);
    getOtherInfo(url, urlPara.channel, urlPara.streamType);
}
