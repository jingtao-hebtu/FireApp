#include "widgethelper.h"

int WidgetHelper::getBorderWidth(AbstractVideoWidget *widget, int *size) {
    //根据画面尺寸主动加大边框
    int width = widget->getVideoWidth();
    if (width == 0) {
        return 1;
    }

    int offset = width / 35;
    if (size) {
        *size = offset;
    }

    int borderWidth = offset / 10;
    borderWidth = (borderWidth < 1 ? 1 : borderWidth);
    return borderWidth;
}

QRect WidgetHelper::getRect(const QString &p1, const QString &p2) {
    QStringList list1 = p1.split(",");
    QStringList list2 = p2.split(",");
    QPoint pos1(list1.at(0).toInt(), list1.at(1).toInt());
    QPoint pos2(list2.at(0).toInt(), list2.at(1).toInt());
    return WidgetHelper::getRect(pos1, pos2);
}

QRect WidgetHelper::getRect(const QPoint &p1, const QPoint &p2) {
    //qrect参数只支持左上角右下角坐标点参数传入/要先换算好
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    QPoint topleft = QPoint(qMin(x1, x2), qMin(y1, y2));
    QPoint bottomright = QPoint(qMax(x1, x2), qMax(y1, y2));
    return QRect(topleft, bottomright);
}

QRect WidgetHelper::getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth,
                                  const ScaleMode &scaleMode) {
    QSize newSize = imageSize;
    QSize widgetSize = widgetRect.size() - QSize(borderWidth * 1, borderWidth * 1);

    if (scaleMode == ScaleMode_Auto) {
        if (newSize.width() > widgetSize.width() || newSize.height() > widgetSize.height()) {
            newSize.scale(widgetSize, Qt::KeepAspectRatio);
        }
    } else if (scaleMode == ScaleMode_Aspect) {
        newSize.scale(widgetSize, Qt::KeepAspectRatio);
    } else {
        newSize = widgetSize;
    }

    int x = widgetRect.center().x() - newSize.width() / 2;
    int y = widgetRect.center().y() - newSize.height() / 2;
    //不是2的倍数需要偏移1像素
    x += (x % 2 == 0 ? 1 : 0);
    y += (y % 2 == 0 ? 1 : 0);
    return QRect(x, y, newSize.width(), newSize.height());
}

void WidgetHelper::getScaledImage(QImage &image, const QSize &widgetSize, const ScaleMode &scaleMode, bool fast) {
    Qt::TransformationMode mode = fast ? Qt::FastTransformation : Qt::SmoothTransformation;
    if (scaleMode == ScaleMode_Auto) {
        if (image.width() > widgetSize.width() || image.height() > widgetSize.height()) {
            image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
        }
    } else if (scaleMode == ScaleMode_Aspect) {
        image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
    } else {
        image = image.scaled(widgetSize, Qt::IgnoreAspectRatio, mode);
    }
}

void WidgetHelper::rotateSize(int rotate, int &width, int &height) {
    int w = width;
    int h = height;
    if (rotate == 90 || rotate == 270) {
        width = h;
        height = w;
    }
}

QString WidgetHelper::getTextByFormat(const OsdInfo &osd) {
    QString text = osd.text;
    QDateTime now = QDateTime::currentDateTime();
    if (osd.format == OsdFormat_Date) {
        text = now.toString("yyyy-MM-dd");
    } else if (osd.format == OsdFormat_Time) {
        text = now.toString("HH:mm:ss");
    } else if (osd.format == OsdFormat_DateTime) {
        text = now.toString("yyyy-MM-dd HH:mm:ss");
    } else if (osd.format == OsdFormat_DateTimeMs) {
        text = now.toString("yyyy-MM-dd HH:mm:ss.zzz");
    }

    return text;
}

QPoint WidgetHelper::getPointByPosition(const OsdInfo &osd, const QRect &rect, int &flag) {
    //如果存在图片则取图片的尺寸(否则取当前字体下的文字的尺寸)
    int width = 0;
    int height = 0;
    if (osd.position != OsdPosition_Custom) {
        if (osd.format == OsdFormat_Image) {
            width = osd.image.width();
            height = osd.image.height();
        } else {
            QFont font;
            font.setFamily(osd.fontName);
            font.setPixelSize(osd.fontSize);
            QFontMetrics fm(font);
            WidgetHelper::getTextRect(&fm, osd.text, width, height);
        }
    }

    //默认位置在左上角
    flag = Qt::AlignLeft | Qt::AlignTop;
    QPoint point = QPoint(rect.x(), rect.y());
    if (osd.position == OsdPosition_LeftTop) {
        flag = Qt::AlignLeft | Qt::AlignTop;
        point = QPoint(rect.x(), rect.y());
    } else if (osd.position == OsdPosition_LeftBottom) {
        flag = Qt::AlignLeft | Qt::AlignBottom;
        point = QPoint(rect.x(), rect.height() - height);
    } else if (osd.position == OsdPosition_RightTop) {
        flag = Qt::AlignRight | Qt::AlignTop;
        point = QPoint(rect.width() - width, rect.y());
    } else if (osd.position == OsdPosition_RightBottom) {
        flag = Qt::AlignRight | Qt::AlignBottom;
        point = QPoint(rect.width() - width, rect.height() - height);
    } else if (osd.position == OsdPosition_Center) {
        flag = Qt::AlignVCenter | Qt::AlignHCenter;
        point = QPoint(rect.width() / 2 - width / 2, rect.height() / 2 - height / 2);
    } else if (osd.position == OsdPosition_Custom) {
        point = osd.point;
    }

    return point;
}

void WidgetHelper::getTextRect(QFontMetrics *fm, const QString &text, int &fwidth, int &fheight) {
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    fwidth = fm->width(text);
#else
    fwidth = fm->horizontalAdvance(text);
#endif
    fheight = fm->height();
}

void WidgetHelper::drawOsd(QPainter *painter, const OsdInfo &osd, const QRect &rect) {
    //绘制下区域看下效果
    //painter->setPen(Qt::red);
    //painter->drawRect(rect);

    int flag;
    QPoint point = getPointByPosition(osd, rect, flag);

    //不同的格式绘制不同的内容
    if (osd.format == OsdFormat_Image) {
        painter->drawImage(point, osd.image);
    } else {
        //根据格式获取文本
        QString text = getTextByFormat(osd);

        //设置颜色及字号
        QFont font;
        font.setFamily(osd.fontName);
        font.setPixelSize(osd.fontSize);
        font.setBold(osd.fontBold);
        painter->setFont(font);

        //存在背景则先绘制背景
        QRect textRect;
        if (osd.bgColor != Qt::transparent) {
            QFontMetrics fm(font);
            painter->setPen(osd.bgColor);
            painter->setBrush(osd.bgColor);

            int fwidth, fheight;
            WidgetHelper::getTextRect(&fm, text, fwidth, fheight);
            int offset = osd.bgPadding;
            textRect = QRect(point.x() - offset, point.y() - offset, fwidth + (offset * 2), fheight + (offset * 2));
            painter->drawRect(textRect);
        }

        painter->setPen(osd.color);
        if (osd.position == OsdPosition_Custom) {
            //绘制了文字背景则区域取文字背景区域再绘制文字
            if (osd.bgColor != Qt::transparent) {
                painter->drawText(textRect, Qt::AlignCenter, text);
            } else {
                painter->drawText(QRect(point.x(), point.y(), rect.width(), rect.height()), text);
            }
        } else {
            painter->drawText(rect, flag, text);
        }
    }
}

void WidgetHelper::drawRect(QPainter *painter, const QRect &rect, int borderWidth, QColor borderColor, bool angle) {
    painter->setPen(QPen(borderColor, borderWidth));
    //背景颜色
    borderColor.setAlpha(50);
    //painter->setBrush(QBrush(borderColor));

    int x = rect.x();
    int y = rect.y();
    int width = rect.width();
    int height = rect.height();

    if (!angle) {
        painter->drawRect(x, y, width, height);
    } else {
        //绘制四个角
        int offset = 10;
        painter->drawLine(x, y, x + offset, y);
        painter->drawLine(x, y, x, y + offset);
        painter->drawLine(x + width - offset, y, x + width, y);
        painter->drawLine(x + width, y, x + width, y + offset);
        painter->drawLine(x, y + height - offset, x, y + height);
        painter->drawLine(x, y + height, x + offset, y + height);
        painter->drawLine(x + width - offset, y + height, x + width, y + height);
        painter->drawLine(x + width, y + height - offset, x + width, y + height);
    }
}

void WidgetHelper::drawPoints(QPainter *painter, const QList<QPoint> &pts, int borderWidth, QColor borderColor) {
    //至少要两个点
    if (pts.count() < 2) {
        return;
    }

    painter->setPen(QPen(borderColor, borderWidth));
    //背景颜色
    borderColor.setAlpha(50);
    //painter->setBrush(QBrush(borderColor));

    //绘制多边形
    QPainterPath path;
    //先移动到起始点
    path.moveTo(pts.first());
    //逐个连接线条
    int count = pts.count();
    for (int i = 1; i < count; ++i) {
        path.lineTo(pts.at(i));
    }

    //闭合图形
    path.closeSubpath();
    painter->drawPath(path);
}

void WidgetHelper::drawPath(QPainter *painter, QPainterPath path, int borderWidth, QColor borderColor) {
    painter->setPen(QPen(borderColor, borderWidth));
    painter->drawPath(path);
}

QLabel *WidgetHelper::showImage(QLabel *label, QWidget *widget, const QImage &image) {
    //等比缩放下分辨率过大的图片
#ifdef Q_OS_ANDROID
    int maxWidth = widget->width();
    int maxHeight = widget->height();
#else
    int maxWidth = 1280;
    int maxHeight = 720;
#endif
    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmap.width() > maxWidth || pixmap.height() > maxHeight) {
        pixmap = pixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    //实例化截图标签
    if (!label) {
        label = new QLabel;
        label->setWindowTitle("截图预览");
        label->setAlignment(Qt::AlignCenter);
    }

    //设置图片
    label->setPixmap(pixmap);

    //安卓上要用dialog窗体才正常
#ifdef Q_OS_ANDROID
    QDialog dialog;
    dialog.setWindowTitle("截图预览");
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    dialog.setLayout(layout);
    dialog.setGeometry(widget->geometry());
    dialog.exec();
    label = NULL;
#else
    label->resize(pixmap.size());
    label->raise();
    label->show();
#endif
    return label;
}
