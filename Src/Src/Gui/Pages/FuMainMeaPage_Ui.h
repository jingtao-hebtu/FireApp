#ifndef FIREAPP_MAINMEAPAGE_UI_H
#define FIREAPP_MAINMEAPAGE_UI_H


class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class VideoWidget;
class QCheckBox;
class QPushButton;
class QString;
class QLabel;
class QProgressBar;
class QFrame;


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
        void initBatteryInfoArea();
        T_QtBase::TSweepCurveViewer* createCurveViewer(const QString &name,
                                                       int x_min,
                                                       int x_max,
                                                       int y_min,
                                                       int y_max);
        void updateBatteryLevelVisuals(int level);
        void showBatteryPlaceholders();
        void setExperimentName(const QString &name);
        void setRecordingStatus(bool recording);

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

        // Control area
        TechToggleButton* mMainCamToggleBtn{nullptr};
        TechToggleButton* mThermalCamToggleBtn{nullptr};
        TechToggleButton* mAiToggleBtn{nullptr};
        //TechToggleButton* mAiSaveToggleBtn{nullptr};
        TechToggleButton* mSaveToggleBtn{nullptr};
        TechActionButton* mCamConfigBtn{nullptr};

        QFrame* mMetricsPanel {nullptr};
        QLabel* mDistValueLabel {nullptr};
        QLabel* mTiltAngleValueLabel {nullptr};

        QFrame* mExperimentPanel {nullptr};
        QLabel* mExperimentNameLabel {nullptr};
        QLabel* mExperimentStatusLabel {nullptr};

        QFrame* mBatteryPanel {nullptr};
        QProgressBar* mBatteryLevelBar {nullptr};
        QLabel* mBatteryStatusLabel {nullptr};
        QWidget* mChargeInfoWidget {nullptr};
        QLabel* mBatteryChargeIcon {nullptr};
        QLabel* mBatteryChargeCurrent {nullptr};
        QLabel* mBatteryTempLabel {nullptr};

        int mMainVideoStretch {6};
        int mSideColumnStretch {2};

        // Thermal camera widget
        ThermalCamera *mThermalCamera {nullptr};
        ThermalWidget *mThermalWidget{nullptr};

        // Curves
        QWidget *mStatisticsWid {nullptr};
        QVBoxLayout *mStatisticsVLayout {nullptr};
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {nullptr};
        T_QtBase::TSweepCurveViewer *mFireAreaCurveViewer {nullptr};

    };
} // TF

#endif //FIREAPP_MAINMEAPAGE_H
