/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : CamConfigWid.h
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#ifndef FIREAPP_CamConfigWid_H
#define FIREAPP_CamConfigWid_H

#include <QWidget>

class QGridLayout;
class QLineEdit;
class QRect;

namespace TF {

    class CamConfigWid : public QWidget {
        Q_OBJECT

    public:
        explicit CamConfigWid(QWidget *parent = nullptr);

        void showAt(const QRect &targetRect);

    private slots:
        void onConnectClicked();
        void onFocusIncrease();
        void onFocusDecrease();
        void onBrightnessIncrease();
        void onBrightnessDecrease();

    private:
        void setupUi();
        QWidget *createInfoPanel();
        QWidget *createButtonPanel();
        QLineEdit *createInfoField(const QString &labelText, QGridLayout *layout, int row);
        void updateInfoDisplay();
        void adjustFocus(int delta);
        void adjustBrightness(int delta);

    private:
        QLineEdit *mFocusEdit {nullptr};
        QLineEdit *mBrightnessEdit {nullptr};
        QLineEdit *mApertureEdit {nullptr};
        QLineEdit *mExposureEdit {nullptr};

        int mFocus {0};
        int mBrightness {0};
        int mAperture {0};
        int mExposure {0};
    };
}

#endif //FIREAPP_CamConfigWid_H

