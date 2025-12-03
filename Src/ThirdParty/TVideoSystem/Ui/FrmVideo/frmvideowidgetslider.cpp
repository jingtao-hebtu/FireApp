#include "frmvideowidgetslider.h"
#include "frmvideowidgetslider_ui.h"
#include "qthelper.h"
#include "videoutil.h"
#include "videowidgetx.h"
#include "appdata.h"


frmVideoWidgetSlider::frmVideoWidgetSlider(VideoWidget *videoWidget, QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWidgetSlider)
{
    ui->setupUi(this);

    //传入的视频控件插入到布局上侧
    this->videoWidget = videoWidget;
    ui->verticalLayout->insertWidget(0, videoWidget);    

    isPause = true;
    isStop = false;
    isMuted = false;
    iconSize = AppData::BtnMinWidth / 2.8;
    ui->sliderPosition->setRange(0, 0);

    //设置图形字体图标
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    IconHelper::setIcon(ui->btnStop, 0xeb13, iconSize - 5);
    IconHelper::setIcon(ui->btnMute, 0xe674, iconSize - 10);

    //关联播放开始和完成信号
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
    connect(videoWidget, SIGNAL(sig_receivePlayFinsh()), this, SLOT(receivePlayFinsh()));
}

frmVideoWidgetSlider::~frmVideoWidgetSlider()
{
    delete ui;
}

bool frmVideoWidgetSlider::getIsStop()
{
    return this->isStop;
}

VideoWidget *frmVideoWidgetSlider::getVideoWidget()
{
    return videoWidget;
}

void frmVideoWidgetSlider::receivePlayStart(int)
{
    //关联采集线程信号槽
    VideoThread *videoThread = videoWidget->getVideoThread();
    connect(videoThread, SIGNAL(receiveDuration(qint64)), this, SLOT(receiveDuration(qint64)));
    connect(videoThread, SIGNAL(receivePosition(qint64)), this, SLOT(receivePosition(qint64)));

    //主动获取一次
    if (videoThread->getIsFile()) {
        this->receiveDuration(videoThread->getDuration());
    }

    isStop = false;
    IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
}

void frmVideoWidgetSlider::receivePlayFinsh()
{
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    VideoUtil::resetDuration(ui->labDuration, ui->labPosition, ui->sliderPosition);
}

void frmVideoWidgetSlider::receiveDuration(qint64 duration)
{
    if (this->property("checkDuration").toBool()) {
        duration = VideoUtil::checkDuration(duration);
    }

    VideoUtil::setDuration(ui->labDuration, ui->sliderPosition, duration);
}

void frmVideoWidgetSlider::receivePosition(qint64 position)
{
    VideoUtil::setPosition(ui->labPosition, ui->sliderPosition, position);
}

void frmVideoWidgetSlider::on_btnPlay_clicked()
{
    //运行阶段取真实值
    if (videoWidget->getIsRunning()) {
        isPause = videoWidget->getIsPause();
    }

    if (isPause) {
        videoWidget->next();
        IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
    } else {
        videoWidget->pause();
        IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    }

    isPause = !isPause;
}

void frmVideoWidgetSlider::on_btnStop_clicked()
{
    isStop = true;
    videoWidget->stop();
}

void frmVideoWidgetSlider::on_btnMute_clicked()
{
    //运行阶段取真实值
    if (videoWidget->getIsRunning()) {
        isMuted = videoWidget->getMuted();
    }

    if (isMuted) {
        videoWidget->setMuted(false);
        IconHelper::setIcon(ui->btnMute, 0xe674, iconSize - 10);
    } else {
        videoWidget->setMuted(true);
        IconHelper::setIcon(ui->btnMute, 0xe667, iconSize - 10);
    }

    isMuted = !isMuted;
}

void frmVideoWidgetSlider::on_sliderPosition_clicked()
{
    int value = ui->sliderPosition->value();
    on_sliderPosition_sliderMoved(value);
}

void frmVideoWidgetSlider::on_sliderPosition_sliderMoved(int value)
{
    videoWidget->setPosition(value);
    emit positionChanged(value);
}
