#ifndef FIREAPP_TFMEAMANAGER_H
#define FIREAPP_TFMEAMANAGER_H

#include "TSingleton.h"


namespace T_QtBase {
    class TSweepCurveViewer;
};


namespace TF {
    class TFMeaManager : public TBase::TSingleton<TFMeaManager>
    {
    public:
        void init();

        void setHeightCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireHeightCurveViewer = viewer;}
        void setAreaCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireAreaCurveViewer = viewer;}

        void receiveNewValue(float value);
        void receiveHeightValue(float value);
        void receiveAreaValue(float value);
        void receiveStatistics(float height, float area);

    private:
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {};
        T_QtBase::TSweepCurveViewer *mFireAreaCurveViewer {};

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
