#include "devicetree.h"
#include "deviceicon.h"
#include <QDateTime>
#include "otherconfig.h"
#include "dbdata.h"


bool DeviceTree::isUrl(const QString &url) {
    return url.left(1).toInt() == 0;
}

QString DeviceTree::getId() {
    //将毫秒数作为唯一标识/保存的时候要用到
    qint64 id;
    QDateTime now = QDateTime::currentDateTime();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    id = now.toSecsSinceEpoch();
#else
    id = now.toTime_t();
#endif
    return QString::number(id);
}

QStringList DeviceTree::getGroupName() {
    QStringList names;
    int count = OtherConfig::DeviceGroupName.count();
    for (int i = 0; i < count; ++i) {
        names << OtherConfig::DeviceGroupName.at(i);
    }

    return names;
}

QStringList DeviceTree::getTreeText(QTreeWidget *treeWidget) {
    QStringList texts;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;
        texts << item->text(0);
    }

    return texts;
}

QTreeWidgetItem *DeviceTree::getTreeItem(QTreeWidget *treeWidget, const QString &text) {
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;
        if (item->text(0) == text) {
            return item;
        }
    }

    return nullptr;
}

QStringList DeviceTree::getUrls(QTreeWidgetItem *item) {
    QStringList urls;
    int count = item->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *child = item->child(i);
        QString url = child->data(0, Qt::UserRole).toString();
        if (child->childCount() > 0) {
            if (DeviceTree::isUrl(url)) {
                urls << url;
            } else {
                urls << DeviceTree::getUrls(child);
            }
        } else {
            //过滤不需要的节点
            QString text = child->text(0);
            if (text == "主码流" || text == "子码流") {
                continue;
            }

            //地址节点一般是http/rtsp/路径盘符开头/父节点一般是IP地址或者唯一编号
            if (DeviceTree::isUrl(url)) {
                urls << url;
            }
        }
    }

    return urls;
}

void DeviceTree::selectItem(QTreeWidget *treeWidget, const QString &url) {
    //遍历所有节点是当前url的选中
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;
        if (item->childCount() > 0) {
            item->setSelected(false);
            continue;
        }

        //最末端节点选中
        QString url2 = item->data(0, Qt::UserRole).toString();
        if (url2.isEmpty()) {
            continue;
        }

        bool selected = (url2 == url);
        item->setSelected(selected);
        //选中对应节点的父节点/不需要这个机制就注释下面几行就行
        if (selected && OtherConfig::TreeRtsp) {
            item->setSelected(false);
            item->parent()->setSelected(true);
        }
    }
}

void DeviceTree::initExpandItem(QTreeWidget *treeWidget) {
            foreach (QString id, OtherConfig::DeviceExpandItem) {
            QTreeWidgetItemIterator it(treeWidget);
            while (*it) {
                QTreeWidgetItem *item = (*it);
                ++it;
                if (item->data(0, Qt::UserRole).toString() == id) {
                    item->setExpanded(true);
                    //treeWidget->expandItem(item);
                    break;
                }
            }
        }
}

void DeviceTree::saveExpandItem(QTreeWidget *treeWidget) {
    QStringList list;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;
        if (item->isExpanded()) {
            QString data = item->data(0, Qt::UserRole).toString();
            if (!DeviceTree::isUrl(data)) {
                list << data;
            }
        }
    }

    OtherConfig::DeviceExpandItem = list;
    OtherConfig::writeConfig();
}

void DeviceTree::initTree(QTreeWidget *treeWidget) {
    //禁止动画效果
    treeWidget->setAnimated(false);
    //标题栏隐藏
    treeWidget->setHeaderHidden(true);
    //设置树状节点可拖曳
    treeWidget->setDragEnabled(true);
    //设置节点双击禁用打开
    treeWidget->setExpandsOnDoubleClick(false);

    //有些年纪大的领导说字体要放大
    if (OtherConfig::TreeBig) {
        QFont font = treeWidget->font();
        font.setPixelSize(20);
        treeWidget->setFont(font);
        //通过设置节点左侧距离来调整前面branch图标大小
        treeWidget->setIndentation(30);
        treeWidget->setIconSize(QSize(25, 25));
    }
}

void DeviceTree::initTreeParent(QTreeWidget *treeWidget) {
    DeviceTree::initTree(treeWidget,
                         OtherConfig::DeviceGroupFId,
                         OtherConfig::DeviceGroupId,
                         OtherConfig::DeviceGroupName);
}

void DeviceTree::initTreeChild(QTreeWidget *treeWidget) {
    if (treeWidget->topLevelItemCount() == 0) {
        return;
    }

    //循环添加分组下的摄像机节点
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        //找到对应的父节点
        const QString &nvrName = DbData::IpcInfo_NvrName.at(i);
        QTreeWidgetItem *itemNvr = DeviceTree::getTreeItem(treeWidget, nvrName);
        if (!itemNvr) {
            continue;
        }

        const QString &ipcName = DbData::IpcInfo_IpcName.at(i);
        const QString &rtspMain = DbData::IpcInfo_RtspMain.at(i);
        const QString &rtspSub = DbData::IpcInfo_RtspSub.at(i);
        QString rtspAddr = DbData::getRtspAddr(i);

        //添加摄像机节点
        QTreeWidgetItem *itemIpc = new QTreeWidgetItem(itemNvr);
        itemIpc->setText(0, ipcName);
        itemIpc->setData(0, Qt::UserRole, rtspAddr);

        //主码流子码流节点
        if (OtherConfig::TreeRtsp) {
            QTreeWidgetItem *itemRtspMain = new QTreeWidgetItem(itemIpc);
            itemRtspMain->setText(0, "主码流");
            itemRtspMain->setData(0, Qt::UserRole, rtspMain);

            QTreeWidgetItem *itemRtspSub = new QTreeWidgetItem(itemIpc);
            itemRtspSub->setText(0, "子码流");
            itemRtspSub->setData(0, Qt::UserRole, rtspSub);
        }
    }
}

void DeviceTree::hideTreeChild(QTreeWidget *treeWidget) {
    if (!OtherConfig::HideNoDevice) {
        return;
    }

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;

        //存在地址则说明不是分组
        QString url = item->data(0, Qt::UserRole).toString();
        if (DeviceTree::isUrl(url)) {
            continue;
        }

        //分组下面没有一个媒体地址则隐藏或删除
        if (DeviceTree::getUrls(item).count() == 0) {
            //item->setHidden(true);
            DeviceTree::deleteTreeItem(treeWidget, item);
        }
    }
}

void DeviceTree::initTree(QTreeWidget *treeWidget, const QStringList &parentIds, const QStringList &childIds,
                          const QStringList &texts) {
    treeWidget->clear();
    QMap<QString, QTreeWidgetItem *> nodes;

    int count = static_cast<int>(texts.count());
    for (int i = 0; i < count; ++i) {
        const QString &parentId = parentIds.at(i);
        const QString &childId = childIds.at(i);
        const QString &text = texts.at(i);

        auto *item = new QTreeWidgetItem;
        item->setText(0, text);
        item->setIcon(0, DeviceIcon::getGroupIcon(text));
        item->setData(0, Qt::UserRole, childId);
        nodes.insert(childId, item);

        //没有父节点编号则说明是顶层节点
        if (parentId.isEmpty()) {
            treeWidget->addTopLevelItem(item);
        } else {
            QTreeWidgetItem *parentItem = nodes.value(parentId);
            if (parentItem) {
                parentItem->addChild(item);
            }
        }
    }
}

void DeviceTree::saveTree(QTreeWidget *treeWidget, QStringList &parentIds, QStringList &childIds, QStringList &texts) {
    //先清空
    parentIds.clear();
    childIds.clear();
    texts.clear();

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        ++it;

        //存在地址则说明不是分组
        QString url = item->data(0, Qt::UserRole).toString();
        if (DeviceTree::isUrl(url)) {
            continue;
        }

        //没有父节点则对应父节点标识为空
        QTreeWidgetItem *parentItem = item->parent();
        parentIds << (parentItem ? parentItem->data(0, Qt::UserRole).toString() : "");
        childIds << url;
        texts << item->text(0);
    }
}

void DeviceTree::addTreeItem(QTreeWidget *treeWidget, QTreeWidgetItem *treeItem, const QString &text) {
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, text);
    item->setIcon(0, DeviceIcon::getIcon("group"));
    item->setData(0, Qt::UserRole, DeviceTree::getId());

    //存在节点则添加到节点下/否则添加顶层节点
    if (treeItem) {
        treeItem->addChild(item);
        treeWidget->expandItem(treeItem);
    } else {
        treeWidget->addTopLevelItem(item);
    }
}

void DeviceTree::deleteTreeItem(QTreeWidget *treeWidget, QTreeWidgetItem *treeItem) {
    //先清空所有子节点
    qDeleteAll(treeItem->takeChildren());
    QTreeWidgetItem *parentItem = treeItem->parent();

    //存在父节点则指定父节点来移除当前节点/否则移除顶层节点
    if (parentItem) {
        int index = parentItem->indexOfChild(treeItem);
        parentItem->takeChild(index);
    } else {
        int index = treeWidget->indexOfTopLevelItem(treeItem);
        treeWidget->takeTopLevelItem(index);
    }
}
