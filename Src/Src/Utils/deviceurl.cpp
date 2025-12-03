#include "deviceurl.h"
#include "urlutil.h"
#include "dbdata.h"
#include "videomanage.h"
#include "appconfig.h"


QString DeviceUrl::UrlFile = "url.txt";
QStringList DeviceUrl::MediaUrls = QStringList();

int DeviceUrl::initUrls()
{
    int count = 0;
    if (AppConfig::SaveUrl) {
        UrlUtil::readUrls(DeviceUrl::UrlFile, DeviceUrl::MediaUrls);
        //地址要求在摄像机表中存在并且处于开启状态
        for (int i = 0; i < DeviceUrl::MediaUrls.count(); ++i) {
            QString url = DeviceUrl::MediaUrls.at(i);
            if (url.isEmpty()) {
                continue;
            }

            //这里可以增加只对rtsp等开头的地址过滤(也可以注释掉不启用这个机制)
            count++;
            if (!url.startsWith("rtsp")) {
                continue;
            }

            int index = DbData::getIpcIndex(url);
            if (index < 0 || DbData::IpcInfo_IpcEnable.at(index) == "禁用") {
                DeviceUrl::MediaUrls[i] = "";
            }
        }
    }

    return count;
}

void DeviceUrl::saveUrls()
{
    if (AppConfig::SaveUrl) {
        UrlUtil::writeUrls(DeviceUrl::UrlFile, DeviceUrl::MediaUrls);
    }

    //将url地址设置到视频管理类中
    VideoManage::Instance()->setMediaUrls(DeviceUrl::MediaUrls);
    //重新启动管理线程(有可能外面停止了线程)
    VideoManage::Instance()->start();
}

void DeviceUrl::clearUrls()
{
    QFile::remove(DeviceUrl::UrlFile);
}

void DeviceUrl::changeUrls()
{
    //当摄像机管理中修改删除清空设备后要重新设置下主界面视频通道
    int count = DeviceUrl::MediaUrls.count();
    for (int i = 0; i < count; ++i) {
        QString url = DeviceUrl::MediaUrls.at(i);
        if (url.isEmpty()) {
            continue;
        }

        //对应地址不在则清空地址并关闭视频
        int index = DbData::getIpcIndex(url);
        if (index < 0) {
            DeviceUrl::MediaUrls[i] = "";
            VideoWidget *videoWidget = VideoManage::Instance()->getVideoWidget(url, url);
            if (videoWidget && videoWidget->getIsRunning()) {
                videoWidget->stop();
            }
        }
    }

    DeviceUrl::saveUrls();
}

int DeviceUrl::getFreeChannel()
{
    //找到末尾一个有视频的通道
    int index = -1;
    for (int i = 0; i < AppConfig::VideoCount; ++i) {
        QString url = DeviceUrl::MediaUrls.at(i);
        if (!url.isEmpty()) {
            index = i;
        }
    }

    //最后一个没有视频的通道=末尾一个有视频的通道+1
    return index + 1;
}

QStringList DeviceUrl::getSelectUrls(QListWidget *listWidget, bool checked)
{
    //拿到所有勾选的文件集合
    QStringList urls;
    int count = listWidget->count();
    for (int row = 0; row < count; row++) {
        QListWidgetItem *item = listWidget->item(row);
        QString url = item->data(Qt::UserRole).toString();
        if (!checked) {
            urls << url;
        } else if (item->checkState() == Qt::Checked) {
            urls << url;
        }
    }

    return urls;
}
