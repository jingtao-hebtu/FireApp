/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : CamConfigWid.h
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#ifndef FIREAPP_CamConfigWid_H
#define FIREAPP_CamConfigWid_H

#include <QWidget>

#include "HKCamZmqClient.h"

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QJsonObject>
#include <initializer_list>

class QLabel;
class QTimer;


class QGridLayout;
class QLineEdit;
class QRect;

namespace TF {

    class TechActionButton;

    class CamConfigWid : public QWidget {
        Q_OBJECT

    public:
        explicit CamConfigWid(QWidget *parent = nullptr);

        void showAt(const QRect &targetRect);

    protected:
        void showEvent(QShowEvent *event) override;

    private slots:
        void onConnectClicked();
        void onFocusIncrease();
        void onFocusDecrease();
        void onFocusReset();
        void onExposureIncrease();
        void onExposureDecrease();
        void onExposureAuto();

    private:
        enum AdjustAction {
            FocusIncreaseAction = 0,
            FocusDecreaseAction,
            ExposureIncreaseAction,
            ExposureDecreaseAction
        };

        void setupUi();
        QWidget *createConnectionPanel();
        QWidget *createInfoPanel();
        QWidget *createButtonPanel();

        void updateConnectionStatus();
        void updateControlsEnabled();
        void updateInfoDisplay();

        bool ensureConnected();
        void startConnectionAttempt();
        void updateConnectionProgress();
        void handleConnectFinished();
        void handleConnectTimeout();
        void cleanupConnectUi();
        void loadRanges();
        void refreshCurrentValues();
        void refreshFocus();
        void refreshExposure();

        void startContinuousAdjust(int action);
        void stopContinuousAdjust();
        void applyAdjustment(int action);

        void adjustFocus(double delta);
        void setFocusNormalized(double value);
        void adjustExposure(double delta);
        void setExposureNormalized(double value);
        void setExposureAutoInternal();

        double mapToDisplay(double normalized, double min, double max) const;
        double clampNormalized(double value, double min, double max) const;
        double readDoubleFromKeys(const QJsonObject &obj, const std::initializer_list<QString> &keys, double defaultValue) const;

    private:
        QLabel *mConnectionStatusLabel {nullptr};

        QLabel *mFocusMinLabel {nullptr};
        QLabel *mFocusCurrentLabel {nullptr};
        QLabel *mFocusMaxLabel {nullptr};

        QLabel *mExposureMinLabel {nullptr};
        QLabel *mExposureCurrentLabel {nullptr};
        QLabel *mExposureMaxLabel {nullptr};

        TechActionButton *mFocusIncBtn {nullptr};
        TechActionButton *mFocusDecBtn {nullptr};
        TechActionButton *mFocusResetBtn {nullptr};
        TechActionButton *mExposureIncBtn {nullptr};
        TechActionButton *mExposureDecBtn {nullptr};
        TechActionButton *mExposureAutoBtn {nullptr};

        QTimer *mAdjustTimer {nullptr};
        QTimer *mConnectDialogTimer {nullptr};
        int mCurrentAction {-1};
        bool mApplyingAdjustment {false};
        bool mConnecting {false};
        bool mConnectTimedOut {false};

        HKCamZmqClient mClient;
        bool mConnected {false};
        bool mRangesLoaded {false};
        bool mAutoConnectTried {false};

        struct ConnectResult {
            bool success {false};
            std::string error;
        };

        QFutureWatcher<ConnectResult> *mConnectWatcher {nullptr};
        QWidget *mConnectDialog {nullptr};
        QLabel *mConnectDialogLabel {nullptr};
        QElapsedTimer mConnectElapsed;

        double mFocusMinNormalized {0.0};
        double mFocusMaxNormalized {1.0};
        double mFocusValue {0.0};

        double mExposureMinNormalized {0.0};
        double mExposureMaxNormalized {1.0};
        double mExposureValue {0.0};

        // Display ranges for mapping normalized values to actual units
        double mFocusDisplayMin {5.8};
        double mFocusDisplayMax {30.0};
        double mExposureDisplayMin {0.0};
        double mExposureDisplayMax {1.0};
    };
}

#endif //FIREAPP_CamConfigWid_H

