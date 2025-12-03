#ifndef WIDGETHEAD_H
#define WIDGETHEAD_H

//把这几个头文件全部包含下懒得每次增加一个新类又来引入对应头文件
#include <QtGui>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#include <QtWidgets>

#endif

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef TIME
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#endif
#ifndef QDATE
#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#endif
#ifndef QTIME
#define QTIME qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif
#ifndef STRDATETIME
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#endif
#ifndef STRDATETIMEMS
#define STRDATETIMEMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
#define Format_RGB QImage::Format_RGBA8888
#else
#define Format_RGB QImage::Format_RGB32
#endif

//ffmpeg在不同系统采集设备对应不同前缀标识
//windows上可以是 dshow/vfwcap linux上可以是 v4l2/video4linux2
#if defined(Q_OS_WIN)
#define Device_Audio    "dshow"
#define Device_Video    "dshow"
#define Device_Screen   "gdigrab"
#elif defined(Q_OS_LINUX)
#define Device_Audio    "alsa"
#define Device_Video    "v4l2"
#define Device_Screen   "x11grab"
#elif defined(Q_OS_MAC)
#define Device_Audio    "avfoundation"
#define Device_Video    "avfoundation"
#define Device_Screen   "avfoundation"
#endif

//linux录音命令 arecord -d 10 -f S16_LE -r 44100 -c 2 output.wav

#include "widgetstruct.h"

#endif // WIDGETHEAD_H
