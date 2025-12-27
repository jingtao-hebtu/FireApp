#ifndef FIREAPP_MAINMEAPAGE_H
#define FIREAPP_MAINMEAPAGE_H

#include "WitImuData.h"
#include <QWidget>

#include "HKCamSearcher.h"


class VideoWidget;
class QTimer;



namespace TF {
    class FuMainMeaPage_Ui;

    class TFDistClient;

    class WitImuSerial;

    class HKCamSearcher;

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

    private slots:
        void onMainCamBtnPressed();

        void onThermalCamBtnPressed();

        void onSaveBtnToggled(bool checked);

        void onAiBtnToggled(bool checked);

        // Measurements
        void onUpdateDist(float dist);
        void onDistTimeout();

        void onUpdateWitImuData(const WitImuData& data);

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
    };

} // TF

#endif //FIREAPP_MAINMEAPAGE_H
