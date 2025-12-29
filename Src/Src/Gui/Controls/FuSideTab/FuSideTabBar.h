/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuSideTabBar.h
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_FUSIDETABBAR_H
#define FIREUI_FUSIDETABBAR_H

#include <QWidget>
#include <QButtonGroup>

namespace TF {

    class FuSideTabBar_Ui;

    class FuSideTabBar : public QWidget {
        Q_OBJECT
    public:
        explicit FuSideTabBar(QWidget *parent = nullptr);
        ~FuSideTabBar() override;

        [[nodiscard]] int currentIndex() const;
        void setCurrentIndex(int index);

    signals:
        void tabSelected(int index);
        void cameraConfigRequested();
        void newExperimentRequested();

    private:
        void setupUi();
        void setupConnections();
        void confirmShutdown();

    private:
        FuSideTabBar_Ui *mUi {nullptr};
        QButtonGroup *mButtonGroup {nullptr};
    };
}

#endif //FIREUI_FUSIDETABBAR_H
