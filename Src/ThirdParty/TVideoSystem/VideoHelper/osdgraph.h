#ifndef OSDGRAPH_H
#define OSDGRAPH_H

#include "widgethead.h"
#include "abstractvideowidget.h"

class OsdGraph {
public:
    //演示添加标签
    static void addOsd(AbstractVideoWidget *widget, int &index);

    static void addOsd2(AbstractVideoWidget *widget, int count);

    //演示添加图形
    static void addGraph(AbstractVideoWidget *widget, int &index);

    static void addGraph2(AbstractVideoWidget *widget, int count);

    //获取测试用文字水印集合
    static QList<OsdInfo> getTestOsd(int height);

    //演示文字水印集合
    static void testOsd(AbstractVideoWidget *widget);

    //添加日期时间水印
    static void addDateTimeOsd(AbstractVideoWidget *widget, const QString &name = "dateTime", int size = 50,
                               OsdPosition position = OsdPosition_LeftTop);

    //调整里边外边
    static int getOsdSide(const QRect &rect, quint8 position, quint8 side, int offset, int height);

    //根据里边外边获取文本坐标位置
    static QPoint getOsdPoint(const QRect &rect, quint8 position, quint8 side, int padding, int fwidth, int fheight);

    //合并图形和标签/传入区域和文本组合成一个图形/position 1-4依次表示左上右上左下右下/side 0-自动 1-里边 2-外边
    static void mergeOsdGraph(const QList<QString> &texts, const QList<QColor> &textColors, int fontSize,
                              const QList<QRect> &rects, const QList<QColor> &borderColors, int borderWidth,
                              QList<OsdInfo> &osds, QList<GraphInfo> &graphs, int videoHeight,
                              quint8 position = 1, quint8 side = 0);

    //演示合并图形和标签
    static void testOsdGraph(AbstractVideoWidget *widget, int fontSize, quint8 position = 1, quint8 side = 0);
};

#endif // OSDGRAPH_H
