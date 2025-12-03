#include "urlutil.h"
#include "qfile.h"
#include "qdebug.h"

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

void UrlUtil::readUrls(const QString &fileName, QStringList &urls) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream in(&file);
    int count = urls.count();
    while (!in.atEnd()) {
        //每次读取一行
        QString line = in.readLine();
        //去除空格回车换行
        line = line.trimmed();
        line.replace("\r", "");
        line.replace("\n", "");
        //空行或者注释行不用处理
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        //格式: 0,rtsp://192.168.1.200
        QStringList list = line.split(",");
        if (list.count() != 2) {
            continue;
        }

        int channel = list.at(0).toInt();
        //替换成新的url
        if (channel >= 0 && channel < count) {
            urls[channel] = list.at(1);
        }
    }

    file.close();
}

void UrlUtil::writeUrls(const QString &fileName, const QStringList &urls) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QTextStream out(&file);
    int count = urls.count();
    for (int i = 0; i < count; ++i) {
        QString url = urls.at(i);
        if (!url.isEmpty()) {
            out << QString("%1,%2\n").arg(i).arg(url);
        }
    }

    file.close();
}

QStringList UrlUtil::getUrls(int type) {
    QStringList urls;

    //安卓上暂时就放几个测试就好
#ifdef Q_OS_ANDROID
    appendUrl(urls, "/storage/emulated/0/000/mp4/16.mp4");
    appendUrl(urls, "rtsp://192.168.0.110:8554/stream/V1");
    appendUrl(urls, "ws://47.114.127.78:6903/stream/v1.live.flv");
    appendUrl(urls, "http://175.42.176.202:8086/hlsram/live5/index.m3u8");
    appendUrl(urls, "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid");
    appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");    
    appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101");
    appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0");
    return urls;
#endif

    if (type & 0x01) {
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video1");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video2");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/102");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=1");
        appendUrl(urls,
                  "rtsp://admin:Admin123456@192.168.0.160:554/stream0?username=admin&password=E10ADC3949BA59ABBE56E057F20F883E");
        appendUrl(urls,
                  "rtsp://admin:Admin123456@192.168.0.160:554/stream1?username=admin&password=E10ADC3949BA59ABBE56E057F20F883E");
    }

    //全网最全音视频媒体流 https://blog.csdn.net/weixin_42182599/article/details/126054178
    if (type & 0x02) {
        appendUrl(urls, "http://vjs.zencdn.net/v/oceans.mp4");
        appendUrl(urls, "http://media.w3.org/2010/05/sintel/trailer.mp4");
        appendUrl(urls, "http://sdk-release.qnsdk.com/VID_20220207_144828.mp4");
        appendUrl(urls, "http://sf1-hscdn-tos.pstatp.com/obj/media-fe/xgplayer_doc_video/flv/xgplayer-demo-360p.flv");
        appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear1/prog_index.m3u8");
        appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8");

        //https://gitee.com/zhaorunhai/test/blob/master/饭太硬直播源.m3u8
        appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");
        appendUrl(urls, "http://vd3.bdstatic.com/mda-ierq2z9f1exw934e/logo/hd/mda-ierq2z9f1exw934e.mp4");
        appendUrl(urls, "http://vd3.bdstatic.com/mda-mcf4jracp0aq1b64/sc/cae_h264/1615836158/mda-mcf4jracp0aq1b64.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-nj0ahm6t2ib5n2jy/720p/h264/1664610158927030550/mda-nj0ahm6t2ib5n2jy.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-mi1dd05gmhwejdwn/sc/cae_h264/1630576203346678103/mda-mi1dd05gmhwejdwn.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-nk3am8nwdgqfy6nh/sc/cae_h264/1667555203921394810/mda-nk3am8nwdgqfy6nh.mp4");
        appendUrl(urls,
                  "http://vd4.bdstatic.com/mda-nei02hdtnd2c88ci/sc/cae_h264/1652920810425683641/mda-nei02hdtnd2c88ci.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-mhhf5mr00yyhpfjs/1080p/cae_h264/1629284581057661229/mda-mhhf5mr00yyhpfjs.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-mhig1c3sw223mx8p/1080p/cae_h264/1629380139191731149/mda-mhig1c3sw223mx8p.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-mesdm11tdp69a9ye/1080p/cae_h264/1622115758783335310/mda-mesdm11tdp69a9ye.mp4");
        appendUrl(urls,
                  "http://vd3.bdstatic.com/mda-mekfm7wu6f4rtt99/1080p/cae_h264/1621595318412084671/mda-mekfm7wu6f4rtt99.mp4");
    }

    if (type & 0x04) {
        //格式 搜索到的 https://music.163.com/#/song?id=179768
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=179768.mp3");
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=5238772.mp3");
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=447925558.mp3");
    }

    if (type & 0x08) {
#ifdef Q_OS_WIN
        appendUrl(urls, "f:/mp4/push/1.mp4");
        appendUrl(urls, "f:/mp4/push/b1.mp4");
        appendUrl(urls, "f:/mp4/other/1.mkv");
        appendUrl(urls, "f:/mp4/other/1.wmv");
        appendUrl(urls, "f:/mp4/other/1.mov");
        appendUrl(urls, "f:/mp4/rotate/90.mp4");
        appendUrl(urls, "f:/mp4/rotate/180.mp4");
        appendUrl(urls, "f:/mp4/other/avi0.avi");
        appendUrl(urls, "f:/mp4/ts/1.ts");
        appendUrl(urls, "f:/mp4/asf/1.asf");
        appendUrl(urls, "f:/mp4/high/1.mp4");
        appendUrl(urls, "f:/mp4/high/4k.mp4");
        appendUrl(urls, "f:/mp4/push/record2651.mp4");
        appendUrl(urls, "f:/mp4/264265/test.h264");
        appendUrl(urls, "f:/mp4/新建文件夹/1.mp4");
#endif
    }

    if (type & 0x10) {
#ifdef Q_OS_WIN
        appendUrl(urls, "f:/mp3/1.mp3");
        appendUrl(urls, "f:/mp3/1.wav");
        appendUrl(urls, "f:/mp3/1.wma");
        appendUrl(urls, "f:/mp3/1.mid");
#endif
    }

    if (type & 0x20) {
#ifdef Q_OS_WIN
        appendUrl(urls, "video=OBS Virtual Camera");
        appendUrl(urls, "video=USB Video Device|1280x720|25");
        appendUrl(urls, "video=USB Video Device:audio=麦克风 (USB Audio Device)|1280x720|25");
#else
        appendUrl(urls, "video=/dev/video0");
        appendUrl(urls, "video=/dev/video1");
#endif
    }

    if (type & 0x40) {
#if defined(Q_OS_WIN)
        appendUrl(urls, "f:/mp3/1.mp3");
        appendUrl(urls, "f:/mp4/ts/1.ts");
        appendUrl(urls, "f:/mp4/other/1.mkv");
        appendUrl(urls, "f:/mp4/push/b1.mp4");
        appendUrl(urls, "f:/mp4/push/16.mp4");
        appendUrl(urls, "f:/mp4/high/1.mp4");
        appendUrl(urls, "f:/mp4/rotate/90.mp4");
        appendUrl(urls, "f:/mp4/push/record2651.mp4");
        appendUrl(urls, "f:/mp4/264265/test.h264");
        appendUrl(urls, "f:/mp4/新建文件夹/1.mp4");

        appendUrl(urls, "video=USB Video Device|1280x720|25|mjpeg");
        appendUrl(urls, "screen=desktop||25|0|0");
        appendUrl(urls, "screen=title=系统设置||25");

#elif defined(Q_OS_LINUX)
        appendUrl(urls, "/home/liu/1.mp4");
        appendUrl(urls, "audio=hw:0");
        appendUrl(urls, "video=/dev/video0");
        appendUrl(urls, "screen=:0.0||25|0|0");
#elif defined(Q_OS_MAC)
        appendUrl(urls, "screen=0||25|0|0");
#endif

        appendUrl(urls, "http://47.114.127.78:6900/v1");
        appendUrl(urls, "http://47.114.127.78:6900/v2");
        appendUrl(urls, "rtsp://47.114.127.78:6901/stream/v1");
        appendUrl(urls, "rtmp://47.114.127.78:6902/stream/v1");
        appendUrl(urls, "ws://47.114.127.78:6903/stream/v1.live.flv");

        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=5238772.mp3");
        appendUrl(urls, "http://video.ubtrobot.com/A Fly In The Hand.mp4");
        appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");
        appendUrl(urls,
                  "http://vd2.bdstatic.com/mda-mi1dd05gmhwejdwn/sc/cae_h264/1630576203346678103/mda-mi1dd05gmhwejdwn.mp4");
        appendUrl(urls, "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid");

        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video1");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.108:554/cam/realmonitor?channel=1&subtype=0");
    }

    if (type & 0x80) {
        //下面的地址可能随时会失效/http://www.24h9.com/
        appendUrl(urls, "http://175.42.176.202:8086/hlsram/live1/index.m3u8");
        appendUrl(urls, "http://175.42.176.202:8086/hlsram/live3/index.m3u8");
        appendUrl(urls, "http://175.42.176.202:8086/hlsram/live5/index.m3u8");
        appendUrl(urls, "http://175.42.176.202:8086/hlsram/live6/index.m3u8");
    }

    return urls;
}

void UrlUtil::appendUrl(QStringList &urls, const QString &url) {
    if (!urls.contains(url)) {
        urls << url;
    }
}

QStringList UrlUtil::getUrlByType(int type) {
    QStringList urls;
    if (type == 1) {
        for (int i = 1; i <= 4; ++i) {
            urls << QString("f:/mp4/push/%1.mp4").arg(i);
        }
    } else if (type == 2) {
        urls << "f:/mp4/asf/1.asf";
        urls << "f:/mp4/push/16.mp4";
        urls << "f:/mp4/high/1.mp4";
        urls << "f:/mp4/high/4k.mp4";
    } else if (type == 3) {
        for (int i = 1; i <= 16; ++i) {
            urls << QString("f:/mp4/push/%1.mp4").arg(i);
        }
    } else if (type == 4) {
        urls
                << "http://vd2.bdstatic.com/mda-mhhf5mr00yyhpfjs/1080p/cae_h264/1629284581057661229/mda-mhhf5mr00yyhpfjs.mp4";
        urls
                << "http://vd2.bdstatic.com/mda-mhig1c3sw223mx8p/1080p/cae_h264/1629380139191731149/mda-mhig1c3sw223mx8p.mp4";
        urls
                << "http://vd2.bdstatic.com/mda-mesdm11tdp69a9ye/1080p/cae_h264/1622115758783335310/mda-mesdm11tdp69a9ye.mp4";
        urls << "http://vd2.bdstatic.com/mda-mi1dd05gmhwejdwn/sc/cae_h264/1630576203346678103/mda-mi1dd05gmhwejdwn.mp4";
    } else if (type == 5) {
        for (int i = 1; i <= 16; ++i) {
            urls
                    << "http://vd2.bdstatic.com/mda-mi1dd05gmhwejdwn/sc/cae_h264/1630576203346678103/mda-mi1dd05gmhwejdwn.mp4";
        }
    } else if (type == 6) {
        urls << "ws://127.0.0.1:8080/stream/V1.live.flv";
        urls << "ws://127.0.0.1:8080/stream/V2.live.flv";
        urls << "ws://127.0.0.1:8080/stream/V3.live.flv";
        urls << "ws://127.0.0.1:8080/stream/V4.live.flv";
    } else if (type == 7) {
        for (int i = 1; i <= 16; ++i) {
            urls << QString("ws://127.0.0.1:8080/stream/V%1.live.flv").arg(i);
        }
    } else if (type == 8) {
        urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
        urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101";
        urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0";
        urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0";
    } else if (type == 9) {
        for (int i = 1; i <= 16; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0";
        }
    } else if (type == 10) {
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0";
        }
    } else if (type == 11) {
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video2";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/102";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=1";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=1";
        }
    }

    return urls;
}
