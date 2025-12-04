#ifndef FIREAPP_MAINMEAPAGE_UI_H
#define FIREAPP_MAINMEAPAGE_UI_H


class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class VideoWidget;
class QComboBox;
class QCheckBox;
class QPushButton;


namespace TF {

    class TechToggleButton;
    class TechActionButton;

    class FuMainMeaPage_Ui
    {

        friend class FuMainMeaPage;

    public:
        void setupUi(QWidget *wid);

    private:
        void initVideoArea();
        void initCtrlArea();

    private:
        QWidget *mWid{};

        QVBoxLayout *mMainVLayout{};
        VideoWidget *mVideoWid{};
        QWidget *mVideoArea{};
        QHBoxLayout *mVideoHLayout{};
        QWidget *mVideoSideWid{};
        QVBoxLayout *mVideoSideVLayout{};
        QWidget *mInfraredVideoWid{};
        QWidget *mStatisticsWid{};

        QWidget *mCtrlWid{};
        QHBoxLayout *mCtrlHLayout{};

        TechToggleButton* mMainCamToggleBtn{nullptr};
        TechToggleButton* mAiToggleBtn{nullptr};
        TechToggleButton* mSaveToggleBtn{nullptr};
        TechActionButton* mRefreshOnceBtn{nullptr};

    };
} // TF

#endif //FIREAPP_MAINMEAPAGE_H
