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

        void setCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mFireHeightCurveViewer = viewer;}

        void receiveNewValue(float value);

    private:
        T_QtBase::TSweepCurveViewer *mFireHeightCurveViewer {};

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
