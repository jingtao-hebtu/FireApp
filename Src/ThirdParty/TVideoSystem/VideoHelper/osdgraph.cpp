#include "osdgraph.h"
#include "widgethelper.h"

void OsdGraph::addOsd(AbstractVideoWidget *widget, int &index) {
    index++;
    if (index > 8) {
        index = 1;
    }

    if (index == 7) {
        widget->clearOsd();
        addOsd2(widget, 100);
        return;
    }

    int fontSize = widget->getVideoWidth() / 30;

    //演示添加多种标签(具体参数参见结构体)
    OsdInfo osd;
    osd.fontSize = fontSize;
    if (index == 1) {
        //左上角文字
        osd.text = "摄像头: 测试";

        //支持 \n 换行以及带上日期时间(可以用多个 \n 换多行)
        //osd.text = "第一行文本\n第二行文本\n%{localtime}";
        //osd.text = "第一行文本\n\n第二行文本\n\n%{localtime}";

        //可以指定字体和图形代码
        //osd.fontName = "iconfont";
        //osd.text = QChar(0xe505);
    } else if (index == 2) {
        //右上角图片
        osd.image = QImage(":/image/bg_novideo.png");
        osd.format = OsdFormat_Image;
        osd.position = OsdPosition_RightTop;

        //如果图片设置的是资源文件则还需要取出资源文件保存到本地
        osd.name = "osd.png";
        QString file = QString("./%1").arg(osd.name);
        osd.image.save(file, "png");
    } else if (index == 3) {
        //指定坐标显示标签
        osd.text = "QPoint(100, 100)";
        osd.color = Qt::white;
        osd.point = QPoint(100, 100);
        osd.position = OsdPosition_Custom;
    } else if (index == 4) {
        //左下角时间
        osd.color = Qt::white;
        //带透明背景颜色
        osd.bgColor = QColor(255, 0, 0, 60);
        //默认背景紧挨着文本/设置个间距更美观
        osd.bgPadding = fontSize / 3;
        osd.format = OsdFormat_Time;
        osd.position = OsdPosition_LeftBottom;
    } else if (index == 5) {
        //右下角日期时间
        osd.color = "#A279C5";
        osd.format = OsdFormat_DateTime;
        osd.position = OsdPosition_RightBottom;
    } else if (index == 6) {
        //中间文字/这里设置显示帧序号
        osd.text = " / 1000";
        osd.format = OsdFormat_FrameNumber;
        osd.position = OsdPosition_Center;
    }

    if (index <= 6) {
        //添加标签
        widget->appendOsd(osd);
    } else {
        //清空所有标签
        widget->clearOsd();
    }
}

void OsdGraph::addOsd2(AbstractVideoWidget *widget, int count) {
    int width = widget->getVideoWidth() - 100;
    int height = widget->getVideoHeight() - 100;
    int fontSize = widget->getVideoWidth() / 50;

    static QStringList colors = QColor::colorNames();
    for (int i = 0; i < count; ++i) {
        OsdInfo osd;
        osd.fontSize = fontSize + rand() % 30;
        osd.name = QString("graph%1").arg(i);
        osd.text = QString("文本%1").arg(i);
        osd.color = colors.at(rand() % colors.count());
        osd.point = QPoint(rand() % width, rand() % height);
        osd.position = OsdPosition_Custom;
        widget->setOsd(osd);
    }
}

void OsdGraph::addGraph(AbstractVideoWidget *widget, int &index) {
    index++;
    if (index > 6) {
        index = 1;
    }

    if (index == 5) {
        widget->clearGraph();
        addGraph2(widget, 100);
        return;
    }

    int borderWidth = WidgetHelper::getBorderWidth(widget);

    //演示添加多种图形(具体参数参见结构体)
    GraphInfo graph;
    graph.borderWidth = borderWidth;
    if (widget->getWidgetPara().graphDrawMode == DrawMode_Source) {
        //随机生成矩形
        int width = widget->width() - 100;
        int height = widget->height() - 100;
        if (widget->getIsRunning()) {
            width = widget->getVideoWidth() - 100;
            height = widget->getVideoHeight() - 100;
        }

        int x = rand() % width;
        int y = rand() % height;
        int w = 80 + rand() % 30;
        int h = 30 + rand() % 40;
        graph.rect = QRect(x, y, w, h);
        static QStringList colors = QColor::colorNames();
        graph.borderColor = colors.at(rand() % colors.count());
    } else {
        if (index == 1) {
            //三个图形一起(矩形/点集合/路径)
            graph.rect = QRect(10, 30, 150, 100);
            graph.points = QList<QPoint>() << QPoint(200, 100) << QPoint(250, 50) << QPoint(300, 150);
            QPainterPath path;
            path.addEllipse(50, 50, 50, 50);
            graph.path = path;
        } else if (index == 2) {
            //矩形
            graph.borderColor = Qt::white;
            graph.rect = QRect(10, 160, 130, 80);
        } else if (index == 3) {
            //点集合
            graph.borderColor = Qt::green;
            graph.points =
                    QList<QPoint>() << QPoint(300, 100) << QPoint(400, 50) << QPoint(500, 150) << QPoint(450, 200)
                                    << QPoint(350, 180);
        } else if (index == 4) {
            //路径(圆形/圆角矩形)
            graph.borderColor = "#22A3A9";
            QPainterPath path;
            path.addEllipse(500, 30, 100, 50);
            path.addRoundedRect(550, 100, 150, 100, 5, 5);
            graph.path = path;
        }
    }

    if (index <= 4) {
        //添加图形
        widget->appendGraph(graph);
    } else {
        //清空所有图形
        widget->clearGraph();
    }
}

void OsdGraph::addGraph2(AbstractVideoWidget *widget, int count) {
    int width = widget->getVideoWidth() - 100;
    int height = widget->getVideoHeight() - 100;
    int borderWidth = WidgetHelper::getBorderWidth(widget);
    int wx = widget->getVideoWidth() / 50;
    int hx = widget->getVideoHeight() / 50;

    static QStringList colors = QColor::colorNames();
    for (int i = 0; i < count; ++i) {
        GraphInfo graph;
        graph.name = QString("graph%1").arg(i);
        int x = rand() % width;
        int y = rand() % height;
        int w = wx + rand() % 40;
        int h = hx + rand() % 30;
        graph.rect = QRect(x, y, w, h);
        graph.borderWidth = borderWidth;
        graph.borderColor = colors.at(rand() % colors.count());
        widget->setGraph(graph);
    }
}

QList<OsdInfo> OsdGraph::getTestOsd(int height) {
    //高度为0则默认720P
    if (height == 0) {
        height = 720;
    }

    QList<OsdInfo> osds;
    OsdInfo osd;
    osd.name = "DateTime";
    osd.fontSize = height / 10;
    osd.format = OsdFormat_DateTime;
    osd.position = OsdPosition_RightTop;
    osds << osd;

    osd.name = "PtsTime";
    osd.color = "#FDCD72";
    osd.fontSize = height / 13;
    osd.format = OsdFormat_PtsTime;
    osd.position = OsdPosition_LeftBottom;
    osds << osd;

#if 0
    //演示显示当前帧序号/总帧数需要通过text传入
    osd.name = "FrameNumber";
    osd.text = " / 1000";
    osd.color = "#753775";
    osd.fontSize = height / 13;
    osd.format = OsdFormat_FrameNumber;
    osd.position = OsdPosition_LeftTop;
    osds << osd;
#endif

#if 0
    //这里演示设置的是资源文件中的图片文件/还需要保存到本地
    osd.name = "osd.png";
    osd.image = QImage(":/image/bg_novideo.png");
    osd.format = OsdFormat_Image;
    osd.position = OsdPosition_RightTop;
    osd.image.save("./osd.png", "png");
    osds << osd;
#endif

    osd.name = "Text";
    osd.fontSize = height / 15;
    osd.text = "QQ: 517216493  WX: feiyangqingyun";
    osd.color = "#FFFFFF";
    osd.format = OsdFormat_Text;
    osd.position = OsdPosition_RightBottom;
    osds << osd;
    return osds;
}

void OsdGraph::testOsd(AbstractVideoWidget *widget) {
    int height = widget->getVideoHeight();
    QList<OsdInfo> osds = OsdGraph::getTestOsd(height);
            foreach (OsdInfo osd, osds) {
            widget->setOsd(osd);
        }
}

void OsdGraph::addDateTimeOsd(AbstractVideoWidget *widget, const QString &name, int size, OsdPosition position) {
    OsdInfo osd;
    osd.name = name;
    osd.fontSize = size;
    osd.color = "#FFFFFF";
    osd.format = OsdFormat_DateTime;
    osd.position = position;
    widget->setOsd(osd);
}

int OsdGraph::getOsdSide(const QRect &rect, quint8 position, quint8 side, int offset, int height) {
    //根据边缘切换是否在里边显示文本
    if (side == 0) {
        //默认显示到外边
        if (height == 0) {
            return 2;
        }

        //文本位置在顶部则区域靠近顶部需要里边显示文本
        //文本位置在底部则区域靠近底部需要里边显示文本
        if ((position == 1 || position == 2)) {
            if (rect.y() < offset) {
                side = 1;
            }
        } else if (position == 3 || position == 4) {
            if (height - (rect.y() + rect.height()) < offset) {
                side = 1;
            }
        }
    }

    return side;
}

QPoint OsdGraph::getOsdPoint(const QRect &rect, quint8 position, quint8 side, int padding, int fwidth, int fheight) {
    int offset = padding;
    int x = rect.x();
    int y = rect.y();
    int w = rect.width();
    int h = rect.height();

    //根据位置上下左右/里面还是外边/计算出文本摆放的位置
    QPoint point;
    if (side == 1) {
        if (position == 1) {
            point.setX(x + offset);
            point.setY(y + offset);
        } else if (position == 2) {
            point.setX(x + w - fwidth - offset);
            point.setY(y + offset);
        } else if (position == 3) {
            point.setX(x + offset);
            point.setY(y + h - fheight - offset);
        } else if (position == 4) {
            point.setX(x + w - fwidth - offset);
            point.setY(y + h - fheight - offset);
        }
    } else {
        if (position == 1) {
            point.setX(x + offset);
            point.setY(y - fheight - (offset / 2));
        } else if (position == 2) {
            point.setX(x + w - fwidth - offset);
            point.setY(y - fheight - (offset / 2));
        } else if (position == 3) {
            point.setX(x + offset);
            point.setY(y + h + offset);
        } else if (position == 4) {
            point.setX(x + w - fwidth - offset);
            point.setY(y + h + offset);
        }
    }

    return point;
}

void OsdGraph::mergeOsdGraph(const QList<QString> &texts, const QList<QColor> &textColors, int fontSize,
                             const QList<QRect> &rects, const QList<QColor> &borderColors, int borderWidth,
                             QList<OsdInfo> &osds, QList<GraphInfo> &graphs, int videoHeight,
                             quint8 position, quint8 side) {
    int count1 = rects.count();
    int count2 = texts.count();
    if (count1 == 0 || count1 != count2) {
        return;
    }

    //用于计算对应字体文本尺寸
    QFont font;
    font.setFamily("wenquanyi");
    font.setPixelSize(fontSize);
    QFontMetrics fm(font);
    int fwidth;
    int fheight = fm.height();

    //一个文本对应一个矩形区域
    for (int i = 0; i < count1; ++i) {
        QString text = texts.at(i);
        QRect rect = rects.at(i);
        int sidex = OsdGraph::getOsdSide(rect, position, side, fheight + 5, videoHeight);
        WidgetHelper::getTextRect(&fm, text, fwidth, fheight);

        //添加文本
        OsdInfo osd;
        osd.name = QString("osd_%1").arg(i);
        osd.text = text;
        osd.format = OsdFormat_Text;
        osd.color = textColors.at(i);
        osd.bgColor = borderColors.at(i);
        osd.bgPadding = borderWidth;
        osd.fontSize = fontSize;
        osd.position = OsdPosition_Custom;
        osd.point = OsdGraph::getOsdPoint(rect, position, sidex, osd.bgPadding, fwidth, fheight);
        osds << osd;

        //添加图形
        GraphInfo graph;
        graph.name = QString("graph_%1").arg(i);
        graph.rect = rect;
        graph.borderColor = borderColors.at(i);
        graph.borderWidth = borderWidth;
        graphs << graph;
    }
}

void OsdGraph::testOsdGraph(AbstractVideoWidget *widget, int fontSize, quint8 position, quint8 side) {
    //为了自适应各种分辨率/多个参数直接根据视频尺寸来计算
    int videoWidth = widget->getVideoWidth();
    int videoHeight = widget->getVideoHeight();
    int borderWidth = WidgetHelper::getBorderWidth(widget);
    if (videoWidth == 0 || videoHeight == 0) {
        return;
    }

    //每个文本可以指定不同的颜色
    QList<QString> texts = QList<QString>() << "张三" << "这是李四";
    QList<QColor> textColors = QList<QColor>() << "#ffffff" << "#000000";

    //每个方框可以指定不同的颜色
    QList<QRect> rects = QList<QRect>() << QRect(videoWidth / 30, videoHeight / 35, videoWidth / 5, videoHeight / 5)
                                        << QRect(videoWidth / 3, videoHeight / 10, videoWidth / 3, videoHeight / 2);
    QList<QColor> borderColors = QList<QColor>() << "#ff0000" << "#00ff00";

    //合并文本和区域
    QList<OsdInfo> osds;
    QList<GraphInfo> graphs;
    OsdGraph::mergeOsdGraph(texts, textColors, fontSize, rects, borderColors, borderWidth, osds, graphs, videoHeight,
                            position, side);

    int count = osds.count();
    for (int i = 0; i < count; ++i) {
        widget->setOsd(osds.at(i));
        widget->setGraph(graphs.at(i));
    }
}
