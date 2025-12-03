#ifndef WIDGETHELPER_H
#define WIDGETHELPER_H

#include "widgethead.h"
#include "abstractvideowidget.h"

class WidgetHelper {
public:
    //根据视频尺寸获取合适的线条宽度(用于滤镜绘制矩形)
    static int getBorderWidth(AbstractVideoWidget *widget, int *size = NULL);

    //传入两个坐标返回矩形
    static QRect getRect(const QString &p1, const QString &p2);

    static QRect getRect(const QPoint &p1, const QPoint &p2);

    //传入图片尺寸和窗体区域及边框大小返回居中区域(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static QRect getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth = 2,
                               const ScaleMode &scaleMode = ScaleMode_Auto);

    //传入图片尺寸和窗体尺寸及缩放策略返回合适尺寸(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static void getScaledImage(QImage &image, const QSize &widgetSize, const ScaleMode &scaleMode = ScaleMode_Auto,
                               bool fast = true);

    //根据旋转度数交换宽高
    static void rotateSize(int rotate, int &width, int &height);

    //传入标签格式获取日期时间字符串
    static QString getTextByFormat(const OsdInfo &osd);

    //传入标签位置获取真实坐标
    static QPoint getPointByPosition(const OsdInfo &osd, const QRect &rect, int &flag);

    //获取文本宽度
    static void getTextRect(QFontMetrics *fm, const QString &text, int &fwidth, int &fheight);

    //绘制标签信息
    static void drawOsd(QPainter *painter, const OsdInfo &osd, const QRect &rect);

    //绘制矩形区域比如人脸框
    static void drawRect(QPainter *painter, const QRect &rect, int borderWidth = 3, QColor borderColor = Qt::red,
                         bool angle = false);

    //绘制点集合多边形路径比如三角形
    static void
    drawPoints(QPainter *painter, const QList<QPoint> &pts, int borderWidth = 3, QColor borderColor = Qt::red);

    //绘制路径集合
    static void drawPath(QPainter *painter, QPainterPath path, int borderWidth = 3, QColor borderColor = Qt::red);

    //显示截图预览
    static QLabel *showImage(QLabel *label, QWidget *widget, const QImage &image);
};

#endif // WIDGETHELPER_H
