#include "TFMeaManager.h"
#include "TSweepCurveViewer.h"


void TF::TFMeaManager::init() {

}

void TF::TFMeaManager::receiveNewValue(float value) {
    if (!mFireHeightCurveViewer) {
        return;
    }

    emit mFireHeightCurveViewer->receiveNextValue(value);
}

