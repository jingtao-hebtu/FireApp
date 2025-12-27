/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : HKCamSearcher.h
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#ifndef FIREAPP_HKCAMSEARCHER_H
#define FIREAPP_HKCAMSEARCHER_H

#include <QObject>
#include <atomic>

struct OnvifDeviceInfo;
class OnvifDevice;
class OnvifSearch;


namespace TF {

    class HKCamSearcher : public QObject
    {
        Q_OBJECT

    public:
        HKCamSearcher(QObject *parent);

        void searchDevice();

    public slots:
        void onReceiveError(const QString &data);

        void onReceiveDevice(const OnvifDeviceInfo &deviceInfo);

    private:
        void init();

    private:
        std::atomic<bool> mInitialized {false};

        OnvifDevice *mDevice {nullptr};
        OnvifSearch *onvifSearch {nullptr};

    };

};



#endif //FIREAPP_HKCAMSEARCHER_H