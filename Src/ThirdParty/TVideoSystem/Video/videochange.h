#ifndef VIDEOCHANGE_H
#define VIDEOCHANGE_H

#include <QObject>

class QTimer;

class VideoWidget;

class VideoChange : public QObject {
Q_OBJECT

public:
    explicit VideoChange(QObject *parent = 0);

private:
    //延时执行切换时间
    int sleepTime;
    //定时器切换
    QTimer *timer;

    //当前视频索引和视频控件
    int currentIndex;
    VideoWidget *videoWidget1;
    VideoWidget *videoWidget2;

private slots:

    //执行切换
    void change();

    //播放开始
    void receivePlayStart(int time);

public:
    //获取当前视频索引和视频控件
    int getCurrentIndex();

    VideoWidget *getVideoWidget1();

    VideoWidget *getVideoWidget2();

    //设置延时切换时间
    void setSleepTime(int sleepTime);

    //设置需要交互的视频控件
    void setVideoWidget(VideoWidget *videoWidget1, VideoWidget *videoWidget2);

    //复位
    void reset();

    //打开地址
    void open(const QString &url1, const QString &url2);

signals:

    //执行切换完成
    void changed();
};

#endif // VIDEOCHANGE_H
