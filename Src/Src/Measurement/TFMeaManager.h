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
        // Widgets
        void setMeaPage(FuMainMeaPage *page) {mMainMeaPage = page;}

        // Statistic curves
        void setHeightCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireHeightCurveViewer = viewer;}
        void setAreaCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireAreaCurveViewer = viewer;}

        void receiveNewValue(float value);
        void receiveHeightValue(float value);
        void receiveAreaValue(float value);
        void receiveStatistics(float height, float area);
        void receiveStatistics(const TFMeaData &data);

        // Distance LDS50
        void updateCurDist(float dist);

        // WitImu
        void updateWitImuData(const WitImuData &data);

        // Flame detection state (set by DetectorWorker)
        void setFlameDetected(bool detected) { mFlameDetected.store(detected); }
        [[nodiscard]] bool isFlameDetected() const { return mFlameDetected.load(); }

        // Flame bbox (set by DetectorWorker, read by ThermalWidget)
        void setFlameBbox(const cv::Rect& bbox) {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            mFlameBbox = bbox;
        }
        cv::Rect flameBbox() const {
            std::lock_guard<std::mutex> lock(mFlameBboxMutex);
            return mFlameBbox;
        }

        [[nodiscard]] float currentDist() const { return mCurDist; }
        [[nodiscard]] float currentTiltAngle() const { return mCurWitImuData.angle.x; }

    private:
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {};
        T_QtBase::TSweepCurveViewer *mFireAreaCurveViewer {};

        FuMainMeaPage *mMainMeaPage {};

        float mCurDist {};

        WitImuData mCurWitImuData {};

        std::atomic<bool> mFlameDetected{false};

        mutable std::mutex mFlameBboxMutex;
        cv::Rect mFlameBbox;

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
