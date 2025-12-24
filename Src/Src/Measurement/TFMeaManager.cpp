#include "TFMeaManager.h"
#include "TSweepCurveViewer.h"


void TF::TFMeaManager::init() {

}

void TF::TFMeaManager::receiveNewValue(float value) {
    receiveHeightValue(value);
}

void TF::TFMeaManager::receiveHeightValue(float value) {
    if (mFireHeightCurveViewer) {
        emit mFireHeightCurveViewer->receiveNextValue(value);
    }
}

void TF::TFMeaManager::receiveAreaValue(float value) {
    if (mFireAreaCurveViewer) {
        emit mFireAreaCurveViewer->receiveNextValue(value);
    }
}

void TF::TFMeaManager::receiveStatistics(float height, float area) {
    receiveHeightValue(height);
    receiveAreaValue(area);
}

void TF::TFMeaManager::receiveStatistics(const TFMeaData& data) {
    receiveHeightValue(data.mFireHeight);
    receiveAreaValue(data.mFireArea);
}
