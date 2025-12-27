/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : HKCamSearcher.cpp
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#include "HKCamSearcher.h"
#include "appconfig.h"
#include "onvifsearch.h"
#include "onvifdevice.h"
#include "TLog.h"


TF::HKCamSearcher::HKCamSearcher(QObject* parent) : QObject(parent) {
    init();
}

void TF::HKCamSearcher::init() {
    if (mInitialized.load()) {
        return;
    }

    onvifSearch = new OnvifSearch(this);
    connect(onvifSearch, &OnvifSearch::receiveError,
        this, &HKCamSearcher::onReceiveError);
    connect(onvifSearch, &OnvifSearch::receiveDevice,
        this, &HKCamSearcher::onReceiveDevice);


    mInitialized.store(true);
}

void TF::HKCamSearcher::searchDevice() {
    AppConfig::SearchClear = true;
    QString localIP = "192.168.1.8";
    QString deviceIP = "192.168.1.64";

    onvifSearch->setSearchFilter(AppConfig::SearchFilter);
    onvifSearch->setSearchInterval(AppConfig::SearchInterval);
    onvifSearch->search(localIP, deviceIP);
}

void TF::HKCamSearcher::onReceiveError(const QString& data) {
    LOG_F(ERROR, "[HKCamSearcher] receiveError %s", data.toStdString().c_str());
}

void TF::HKCamSearcher::onReceiveDevice(const OnvifDeviceInfo& deviceInfo) {
    if (mDevice) {
        return;
    }

    QString addr = deviceInfo.onvifAddr;
    QString ip = deviceInfo.deviceIp;

    mDevice = new OnvifDevice(this);
    mDevice->setOnvifAddr(addr);
    mDevice->setDeviceInfo(deviceInfo);

    bool ret = false;
    mDevice->setUserInfo("admin", "fire123456");
    ret = mDevice->getServices();
    if (mDevice->getMediaUrl().isEmpty() || mDevice->getPtzUrl().isEmpty()) {
        mDevice->getCapabilities();
    }

    QList<OnvifProfileInfo> profiles = mDevice->getProfiles();
    int countProfile = profiles.count();
    if (countProfile == 0) {
        QString ip = OnvifHelper::getUrlIP(mDevice->getOnvifAddr());
        return;
    }

    auto stream = mDevice->getStreamUri();
    LOG_F(INFO, "Got stream %s and profile count %d.", stream.toStdString().c_str(), profiles.count());
}
