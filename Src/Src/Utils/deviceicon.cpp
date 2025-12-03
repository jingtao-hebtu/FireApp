#include "deviceicon.h"
#include "devicetree.h"
#include "globalconfig.h"
#include "iconhelper.h"

QMap<QString, QPixmap> DeviceIcon::icons = QMap<QString, QPixmap>();
void DeviceIcon::initIcon()
{
    if (icons.count() > 0) {
        icons.clear();
    }

    //图标的大小和宽高
    int size = 22;
    int width = size + 2;
    int height = size + 2;

    //可以自行查找对照表改成不同的图标/还可以增加不同的图标用来区分枪机球机等
    QString colorOn = GlobalConfig::TextColor;
    QString colorOff = "thistle";
    icons.insert("nvr", IconHelper::getPixmap(colorOn, 0xe9ef, size, width, height));
    icons.insert("group", IconHelper::getPixmap(colorOn, 0xea24, size + 1, width, height));
    icons.insert("ipcOn", IconHelper::getPixmap(colorOn, 0xea07, size + 2, width, height));
    icons.insert("ipcOff", IconHelper::getPixmap(colorOff, 0xea0a, size, width, height));
    icons.insert("subOn", IconHelper::getPixmap(colorOn, 0xe9ff, size, width, height));
    icons.insert("subOff", IconHelper::getPixmap(colorOff, 0xea0a, size, width, height));
}

QPixmap DeviceIcon::getIcon(const QString &key)
{
    return icons.value(key);
}

QPixmap DeviceIcon::getGroupIcon(const QString &text)
{
    return icons.value("group");
}

void DeviceIcon::initTreeIcon(QTreeWidget *treeWidget, bool checkOffline)
{
    //不同的节点显示不同的图标
    QPixmap iconIpc = icons.value(checkOffline ? "ipcOff" : "ipcOn");
    QPixmap iconSub = icons.value(checkOffline ? "subOff" : "subOn");

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;

        QString text = item->text(0);
        QString url = item->data(0, Qt::UserRole).toString();
        if (text == "主码流" || text == "子码流") {
            item->setIcon(0, iconSub);
        } else if (DeviceTree::isUrl(url)) {
            item->setIcon(0, iconIpc);
        } else {
            item->setIcon(0, getGroupIcon(text));
        }
    }
}

void DeviceIcon::setTreeIcon(QTreeWidget *treeWidget, const QString &url, bool online)
{
    //不同的节点显示不同的图标
    QPixmap iconIpc = icons.value(online ? "ipcOn" : "ipcOff");
    QPixmap iconSub = icons.value(online ? "subOn" : "subOff");

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;

        //找到指定地址所在节点
        if (item->data(0, Qt::UserRole).toString() != url) {
            continue;
        }

        //码率节点不用处理
        QString text = item->text(0);
        if (text == "主码流" || text == "子码流") {
            continue;
        }

        //重新设置对应图标/如果还有子节点也就是码流节点则也要设置
        item->setIcon(0, iconIpc);
        for (int i = 0; i < item->childCount(); ++i) {
            item->child(i)->setIcon(0, iconSub);
        }

        break;
    }
}
