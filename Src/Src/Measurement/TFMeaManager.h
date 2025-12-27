#ifndef FIREAPP_TFMEAMANAGER_H
#define FIREAPP_TFMEAMANAGER_H

#include "TSingleton.h"
#include "TFMeaDef.h"
#include "WitImuData.h"


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

    private:
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {};
        T_QtBase::TSweepCurveViewer *mFireAreaCurveViewer {};

        FuMainMeaPage *mMainMeaPage {};

        float mCurDist {};

        WitImuData mCurWitImuData {};

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
