#ifndef FIREAPP_TFMEAMANAGER_H
#define FIREAPP_TFMEAMANAGER_H

#include "TSingleton.h"
#include "TFMeaDef.h"
#include "WitImuData.h"
#include <atomic>
#include <mutex>
#include <opencv2/core.hpp>


namespace T_QtBase {
    class TSweepCurveViewer;
};


namespace TF {

    class FuMainMeaPage;

    class TFMeaManager : public TBase::TSingleton<TFMeaManager>
    {
    public:
        void init();

    public:
        // HRR
        void calcHRR(float phys_area, float& hrr);

        void pixelToPhysical(double dist, double pixel_w, double pixel_h, double& phys_w, double& phys_h);

        // Widgets
        void setMeaPage(FuMainMeaPage *page) {mMainMeaPage = page;}

        // Statistic curves
        void setHeightCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireHeightCurveViewer = viewer;}
        void setAreaCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireAreaCurveViewer = viewer;}

        void receiveNewValue(float value);
        void receiveHeightValue(float value);
        void receiveAreaValue(float value);
        void receiveHRRValue(float value);
        void receiveStatistics(float height, float area);
        void receiveStatistics(const TFMeaData &data);

        // Distance LDS50
        void updateCurDist(float dist);

        // WitImu
        void updateWitImuData(const WitImuData &data);

        // Flame detection state (set by DetectorWorker)
        void setFlameDetected(bool detected) {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            mFlameDetected = detected;
        }
        [[nodiscard]] bool isFlameDetected() const {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            return mFlameDetected;
        }

        // Flame bbox (set by DetectorWorker, read by ThermalWidget)
        void setFlameBbox(const cv::Rect& bbox) {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            mFlameBbox = bbox;
        }
        cv::Rect flameBbox() const {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            return mFlameBbox;
        }

        // Atomic combined update: flame state + bbox under one lock
        void updateFlameResult(bool detected, const cv::Rect& bbox) {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            mFlameDetected = detected;
            mFlameBbox = bbox;
        }

        [[nodiscard]] float currentDist() const { return mCurDist; }
        [[nodiscard]] float currentTiltAngle() const { return mCurWitImuData.angle.x; }

    private:
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {};
        T_QtBase::TSweepCurveViewer *mFireAreaCurveViewer {};

        FuMainMeaPage *mMainMeaPage {};

        float mCurDist {};

        WitImuData mCurWitImuData {};

        bool mFlameDetected{false};

        mutable std::mutex mFlameBboxMutex;
        cv::Rect mFlameBbox;

        float mFocalLengthW {};
        float mFocalLengthH {};

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
