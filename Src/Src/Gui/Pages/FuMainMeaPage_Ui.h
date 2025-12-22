#ifndef FIREAPP_MAINMEAPAGE_UI_H
#define FIREAPP_MAINMEAPAGE_UI_H


class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class VideoWidget;
class QComboBox;
class QCheckBox;
class QPushButton;


namespace T_QtBase {
    class TSweepCurveViewer;
};


namespace TF {

    class TechToggleButton;
    class TechActionButton;
    class ThermalCamera;
    class ThermalWidget;

    class FuMainMeaPage_Ui
    {

        friend class FuMainMeaPage;

    public:
        void setupUi(QWidget *wid);
        void setVideoAreaStretch(int mainVideoStretch, int sideColumnStretch = 2);

    private:
        void initVideoArea();
        void initThermalCamera();
        void initStatistics();
        void initCtrlArea();

    private:
        QWidget *mWid{};

        QVBoxLayout *mMainVLayout{};
        VideoWidget *mVideoWid{};
        QWidget *mVideoArea{};
        QHBoxLayout *mVideoHLayout{};
        QWidget *mVideoSideWid{};
        QVBoxLayout *mVideoSideVLayout{};
        QWidget *mThermalWid{};

        QWidget *mCtrlWid{};
        QHBoxLayout *mCtrlHLayout{};

        TechToggleButton* mMainCamToggleBtn{nullptr};
        TechToggleButton* mThermalCamToggleBtn{nullptr};
        TechToggleButton* mAiToggleBtn{nullptr};
        TechToggleButton* mSaveToggleBtn{nullptr};

        int mMainVideoStretch {6};
        int mSideColumnStretch {2};

        // Thermal camera widget
        ThermalCamera *mThermalCamera {nullptr};
        ThermalWidget *mThermalWidget{nullptr};

        // Curves
        QWidget *mStatisticsWid {nullptr};
        QVBoxLayout *mStatisticsVLayout {nullptr};

        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {nullptr};

    };
} // TF

#endif //FIREAPP_MAINMEAPAGE_H
