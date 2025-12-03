#ifndef DEVICEICON_H
#define DEVICEICON_H

#include <QMap>
#include <QString>
#include <QPixmap>
#include <QTreeWidget>


class DeviceIcon
{
public:
    //生成图标集合
    static QMap<QString, QPixmap> icons;
    static void initIcon();

    //获取节点图标
    static QPixmap getIcon(const QString &key);
    //获取分组图标
    static QPixmap getGroupIcon(const QString &text);

    //初始化图标
    static void initTreeIcon(QTreeWidget *treeWidget, bool checkOffline);
    //重新设置图标比如上下线
    static void setTreeIcon(QTreeWidget *treeWidget, const QString &url, bool online);
};

#endif // DEVICEICON_H
