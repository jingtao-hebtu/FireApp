#ifndef DEVICETREE_H
#define DEVICETREE_H

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>


class DeviceTree
{
public:
    //当前节点数据是媒体地址
    static bool isUrl(const QString &url);

    //获取时间编号
    static QString getId();
    //获取分组名称
    static QStringList getGroupName();    
    //获取节点名称
    static QStringList getTreeText(QTreeWidget *treeWidget);
    //根据名称获取节点
    static QTreeWidgetItem *getTreeItem(QTreeWidget *treeWidget, const QString &text);

    //获取对应节点所有子节点的媒体地址
    static QStringList getUrls(QTreeWidgetItem *item);
    //选中媒体地址对应节点
    static void selectItem(QTreeWidget *treeWidget, const QString &url);

    //初始化和保存最后展开的节点
    static void initExpandItem(QTreeWidget *treeWidget);
    static void saveExpandItem(QTreeWidget *treeWidget);

    //初始化树控件
    static void initTree(QTreeWidget *treeWidget);
    //初始化父节点
    static void initTreeParent(QTreeWidget *treeWidget);
    //初始化子节点
    static void initTreeChild(QTreeWidget *treeWidget);
    //隐藏空子节点
    static void hideTreeChild(QTreeWidget *treeWidget);

    //初始化树节点
    static void initTree(QTreeWidget *treeWidget, const QStringList &parentIds, const QStringList &childIds, const QStringList &texts);
    //保存节点数据
    static void saveTree(QTreeWidget *treeWidget, QStringList &parentIds, QStringList &childIds, QStringList &texts);

    //添加节点
    static void addTreeItem(QTreeWidget *treeWidget, QTreeWidgetItem *treeItem, const QString &text);
    //删除节点
    static void deleteTreeItem(QTreeWidget *treeWidget, QTreeWidgetItem *treeItem);
};

#endif // DEVICETREE_H
