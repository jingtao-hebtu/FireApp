#ifndef FIREAPP_MAINMEAPAGE_H
#define FIREAPP_MAINMEAPAGE_H

#include <QWidget>


class VideoWidget;
class QTimer;


namespace TF {
    class FuMainMeaPage_Ui;

    class TFDistClient;

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

        void initMea();

    signals:
        void updateDist(float dist);

    private slots:
        void onMainCamBtnPressed();

        void onThermalCamBtnPressed();

        void onSaveBtnToggled(bool checked);

        void onAiBtnToggled(bool checked);

        // Measurements
        void onUpdateDist(float dist);
        void onDistTimeout();

    private:
        FuMainMeaPage_Ui* mUi;
        VideoWidget* mVideoWid{nullptr};

        std::atomic<bool> mMainCamPlaying{false};
        std::atomic<bool> mRecording{false};

        QString mCurrentUrl;

        // Measurement
        TFDistClient *mDistClient{nullptr};
        QTimer *mDistTimeoutTimer{nullptr};
    };

} // TF

#endif //FIREAPP_MAINMEAPAGE_H
