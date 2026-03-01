#include "TFMeaManager.h"
#include "TSweepCurveViewer.h"
#include "FuMainMeaPage.h"
#include "TConfig.h"


void TF::TFMeaManager::init() {
    mFocalLengthW = GET_FLOAT_CONFIG("RGBCam", "FocalLengthW");
    mFocalLengthH = GET_FLOAT_CONFIG("RGBCam", "FocalLengthH");
}

void TF::TFMeaManager::pixelToPhysical(double dist, double pixel_w, double pixel_h,
                     double& phys_w, double& phys_h) {
    phys_w = pixel_w * dist / mFocalLengthW;
    phys_h = pixel_h * dist / mFocalLengthH;
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

void TF::TFMeaManager::updateCurDist(float dist) {
    mCurDist = dist;
    if (mMainMeaPage) {
        emit mMainMeaPage->updateDist(dist);
    }
}

void TF::TFMeaManager::updateWitImuData(const WitImuData& data) {
    mCurWitImuData = data;
    if (mMainMeaPage) {
        emit mMainMeaPage->updateWitImuData(data);
    }
}
