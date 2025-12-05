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

        void setCurveViewer(T_QtBase::TSweepCurveViewer *viewer) {mCurveViewer = viewer;}

        void receiveNewValue(float value);

    private:
        T_QtBase::TSweepCurveViewer *mCurveViewer {};

    };
};

#endif //FIREAPP_TFMEAMANAGER_H
