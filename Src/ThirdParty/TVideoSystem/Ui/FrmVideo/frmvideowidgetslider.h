#ifndef FRMVIDEOWIDGETSLIDER_H
#define FRMVIDEOWIDGETSLIDER_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmVideoWidgetSlider;
}

class frmVideoWidgetSlider : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoWidgetSlider(VideoWidget *videoWidget, QWidget *parent = 0);
    ~frmVideoWidgetSlider();

    bool getIsStop();
    VideoWidget *getVideoWidget();

private:
    Ui::frmVideoWidgetSlider *ui;

    //是否暂停
    bool isPause;
    //是否停止
    bool isStop;
    //是否静音
    bool isMuted;

    //图标大小
    int iconSize;    
    //视频控件
    VideoWidget *videoWidget;

private slots:
    //播放开始
    void receivePlayStart(int);
    //播放结束
    void receivePlayFinsh();

    //文件时长
    void receiveDuration(qint64 duration);
    //播放时长
    void receivePosition(qint64 position);

private slots:
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnMute_clicked();

    void on_sliderPosition_clicked();
    void on_sliderPosition_sliderMoved(int value);

signals:
    void positionChanged(int value);
};

#endif // FRMVIDEOWIDGETSLIDER_H
