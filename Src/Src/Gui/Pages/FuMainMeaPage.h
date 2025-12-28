#ifndef FIREAPP_MAINMEAPAGE_H
#define FIREAPP_MAINMEAPAGE_H

#include "WitImuData.h"
#include "BmsData.h"
#include <QWidget>

#include "HKCamSearcher.h"


class VideoWidget;
class QTimer;



namespace TF {
    class FuMainMeaPage_Ui;

    class TFDistClient;

    class WitImuSerial;

    class HKCamSearcher;

    class CamConfigWid;

    class BmsWorker;

    class FuMainMeaPage : public QWidget
    {
        Q_OBJECT

    public:
        explicit FuMainMeaPage(QWidget* parent = 0);
        ~FuMainMeaPage() override;

        void initAfterDisplay();

    private:
        void initActions();

        void initForm();

        void initHardware();

        void initMea();

        void deinitMea();

    signals:
        void updateDist(float dist);

        void requestWitImuOpen();
        void requestWitImuClose();
        void updateWitImuData(const WitImuData& data);

        // BMS
        void initBms();
        void startBms();
        void stopBms();

        void batteryStatusChanged(int level, float current, float voltage, float temp);

    private slots:
        void onMainCamBtnPressed();
        void onThermalCamBtnPressed();
        void onSaveBtnToggled(bool checked);
        void onAiBtnToggled(bool checked);

        // Measurements
        void onUpdateDist(float dist);
        void onDistTimeout();
        void onUpdateWitImuData(const WitImuData& data);

        // Cam
        void onCamConfigBtnPressed();

        // Bms
        void onBmsStatusUpdated(const BmsStatus& status);
        static void onBmsConnectionStateChanged(bool connected);
        void onBatteryStatusChanged(int level, const QString& current, const QString& voltage, QString temp);

    private:
        FuMainMeaPage_Ui* mUi;
        VideoWidget* mVideoWid{nullptr};

        std::atomic<bool> mMainCamPlaying{false};
        std::atomic<bool> mRecording{false};

        QString mCurrentUrl;

        // Measurement
        TFDistClient *mDistClient{nullptr};
        QTimer *mDistTimeoutTimer{nullptr};

        WitImuSerial *mWitImuSerial{nullptr};
        QThread *mWitImuSerialThread{nullptr};
        WitImuData mImuData;

        // HK cam
        HKCamSearcher* mCamSearcher{nullptr};
        CamConfigWid *mCamConfigWid{nullptr};

        // BMS
        QThread   *mBmsThread{nullptr};
        BmsWorker *mBmsWorker{nullptr};
    };

} // TF

#endif //FIREAPP_MAINMEAPAGE_H
