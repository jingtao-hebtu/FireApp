#ifndef DEVICEURL_H
#define DEVICEURL_H

#include <QString>
#include <QListWidget>


class DeviceUrl
{
public:
    //播放地址历史记录文件
    static QString UrlFile;
    //媒体地址集合
    static QStringList MediaUrls;

    //加载媒体地址
    static int initUrls();
    //保存媒体地址
    static void saveUrls();
    //清空媒体地址
    static void clearUrls();
    //更新媒体地址
    static void changeUrls();

    //找到当前空闲的通道
    static int getFreeChannel();
    //获取回放下载地址
    static QStringList getSelectUrls(QListWidget *listWidget, bool checked);
};

#endif // DEVICEURL_H
