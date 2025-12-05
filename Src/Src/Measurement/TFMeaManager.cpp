#include "TFMeaManager.h"
#include "TSweepCurveViewer.h"


void TF::TFMeaManager::init() {

}

void TF::TFMeaManager::receiveNewValue(float value) {
    if (!mCurveViewer) {
        return;
    }

    emit mCurveViewer->receiveNextValue(value);
}

