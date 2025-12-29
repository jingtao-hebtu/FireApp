/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuSideTabBar_Ui.cpp
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "FuSideTabBar_Ui.h"

#include <QObject>
#include <QIcon>
#include <QSize>
#include <QLabel>
#include <QFrame>
#include <QFont>
#include <QEvent>
#include <QtGlobal>

namespace TF {

    namespace {
        class AutoCenterIconFilter : public QObject {
        public:
            explicit AutoCenterIconFilter(QToolButton *button)
                : QObject(button), mButton(button) {}

            bool eventFilter(QObject *watched, QEvent *event) override {
                if (watched == mButton && event->type() == QEvent::Resize) {
                    updateIconSize();
                }
                return QObject::eventFilter(watched, event);
            }

            void updateIconSize() const {
                if (mButton == nullptr) {
                    return;
                }
                const int iconSide = qMax(16, static_cast<int>(qMin(mButton->width(), mButton->height()) * 0.55));
                mButton->setIconSize(QSize(iconSide, iconSide));
            }

        private:
            QToolButton *mButton;
        };

        QToolButton* createTabButton(QWidget *parent,
                                     const QIcon &icon,
                                     const QString &tooltip,
                                     bool checkable = true,
                                     const QByteArray &role = QByteArrayLiteral("sideTab")) {
            auto *button = new QToolButton(parent);
            button->setCheckable(checkable);
            button->setAutoExclusive(checkable);
            button->setIcon(icon);
            button->setToolTip(tooltip);
            button->setText(tooltip);
            button->setFixedSize(60, 56);
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            button->setFont(QFont("Noto Sans", 9, QFont::DemiBold));
            button->setProperty(role.constData(), true);

            auto *autoCenterFilter = new AutoCenterIconFilter(button);
            button->installEventFilter(autoCenterFilter);
            autoCenterFilter->updateIconSize();
            return button;
        }
    }

    void FuSideTabBar_Ui::setupUi(QWidget *wid) {
        mWid = wid;
        mWid->setObjectName("SideTabBar");

        mLayout = new QVBoxLayout(mWid);
        mLayout->setContentsMargins(6, 8, 6, 8);
        mLayout->setSpacing(10);

        auto *brandLabel = new QLabel(QObject::tr("AI"), mWid);
        brandLabel->setAlignment(Qt::AlignCenter);
        brandLabel->setObjectName("SideBrand");

        auto *divider = new QFrame(mWid);
        divider->setObjectName("SideDivider");
        divider->setFrameShape(QFrame::HLine);
        divider->setFrameShadow(QFrame::Plain);

        struct TabInfo {
            QString iconPath;
            QString tooltip;
        };

        const QVector<TabInfo> tabs = {
                {QStringLiteral(":/icons/nav-dashboard.svg"), QObject::tr("概览")},
                {QStringLiteral(":/icons/nav-control.svg"), QObject::tr("采集")},
                {QStringLiteral(":/icons/nav-logs.svg"), QObject::tr("记录")},
                {QStringLiteral(":/icons/nav-status.svg"), QObject::tr("状态")},
        };

        for (const auto &tab : tabs) {
            mButtons.append(createTabButton(mWid, QIcon(tab.iconPath), tab.tooltip));
        }

        mLayout->addWidget(brandLabel);
        mLayout->addWidget(divider);

        for (auto *button : mButtons) {
            mLayout->addWidget(button);
        }

        mLayout->addStretch();

        auto *actionDivider = new QFrame(mWid);
        actionDivider->setObjectName("SideActionDivider");
        actionDivider->setFrameShape(QFrame::HLine);
        actionDivider->setFrameShadow(QFrame::Plain);

        auto *actionPanel = new QWidget(mWid);
        auto *actionLayout = new QVBoxLayout(actionPanel);
        actionLayout->setContentsMargins(2, 2, 2, 2);
        actionLayout->setSpacing(8);

        mNewExperimentButton = createTabButton(mWid,
                                               QIcon(QStringLiteral(":/icons/nav-logs.svg")),
                                               QObject::tr("新建实验"),
                                               false,
                                               QByteArrayLiteral("sideAction"));
        mNewExperimentButton->setObjectName("NewExperimentButton");

        mCamConfigButton = createTabButton(mWid,
                                           QIcon(QStringLiteral(":/icons/nav-camera.svg")),
                                           QObject::tr("相机配置"),
                                           false,
                                           QByteArrayLiteral("sideAction"));
        mCamConfigButton->setObjectName("CamConfigButton");

        mShutdownButton = createTabButton(mWid,
                                          QIcon(QStringLiteral(":/icons/nav-shutdown.svg")),
                                          QObject::tr("关机"),
                                          false,
                                          QByteArrayLiteral("sideAction"));
        mShutdownButton->setObjectName("ShutdownButton");

        actionLayout->addWidget(mNewExperimentButton, 0, Qt::AlignHCenter);
        actionLayout->addWidget(mCamConfigButton, 0, Qt::AlignHCenter);
        actionLayout->addWidget(mShutdownButton, 0, Qt::AlignHCenter);

        mLayout->addWidget(actionDivider);
        mLayout->addWidget(actionPanel);
    }
}
