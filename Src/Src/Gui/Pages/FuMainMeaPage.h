#ifndef FIREAPP_MAINMEAPAGE_H
#define FIREAPP_MAINMEAPAGE_H

#include <QWidget>


class VideoWidget;


namespace TF {
    class FuMainMeaPage_Ui;

    class FuMainMeaPage : public QWidget
    {
        Q_OBJECT

    public:
        explicit FuMainMeaPage(QWidget* parent = 0);
        ~FuMainMeaPage() override;

    private:
        void initActions();

        void initForm();

    private slots:
        void onMainCamBtnPressed();

        void onSaveBtnToggled(bool checked);

        void onAiBtnToggled(bool checked);

    private:
        FuMainMeaPage_Ui* mUi;
        VideoWidget* mVideoWid{nullptr};

        std::atomic<bool> mMainCamPlaying{false};
        std::atomic<bool> mRecording{false};

        QString mCurrentUrl;
    };

} // TF

#endif //FIREAPP_MAINMEAPAGE_H
