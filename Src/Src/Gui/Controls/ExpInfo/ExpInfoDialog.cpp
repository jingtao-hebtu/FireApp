/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : ExpInfoDialog.cpp
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "ExpInfoDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTime>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QSize>

namespace TF {

    ExpInfoDialog::ExpInfoDialog(QWidget *parent) : QDialog(parent) {
        setWindowTitle(tr("实验信息"));
        setModal(true);
        setObjectName("ExpInfoDialog");
        setWindowFlag(Qt::FramelessWindowHint, true);
        setAttribute(Qt::WA_TranslucentBackground, true);

        setupUi();
        setupConnections();

        const auto defaultName = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"));
        setExperimentName(defaultName);
    }

    QString ExpInfoDialog::experimentName() const {
        if (!mNameEdit) {
            return QString();
        }
        return mNameEdit->text().trimmed();
    }

    void ExpInfoDialog::setExperimentName(const QString &name) {
        if (!mNameEdit) {
            return;
        }

        const auto trimmedName = name.trimmed();
        if (trimmedName.isEmpty()) {
            return;
        }

        mNameEdit->setText(trimmedName);
        mNameEdit->setCursorPosition(mNameEdit->text().length());
        updateConfirmState();
    }

    void ExpInfoDialog::setupUi() {
        auto *shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(20);
        shadowEffect->setOffset(0, 6);
        shadowEffect->setColor(QColor(14, 26, 48, 180));

        auto *panel = new QWidget(this);
        panel->setObjectName("ExpInfoPanel");
        panel->setGraphicsEffect(shadowEffect);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 12, 12, 12);
        mainLayout->addWidget(panel);

        auto *panelLayout = new QVBoxLayout(panel);
        panelLayout->setContentsMargins(20, 20, 20, 16);
        panelLayout->setSpacing(16);

        auto *nameLabel = new QLabel(tr("实验名称"), panel);
        nameLabel->setObjectName("ExpInfoLabel");

        mNameEdit = new QLineEdit(panel);
        mNameEdit->setPlaceholderText(tr("请输入实验名称"));
        mNameEdit->setMinimumHeight(44);
        mNameEdit->setFont(QFont(QStringLiteral("微软雅黑"), 14));

        panelLayout->addWidget(nameLabel);
        panelLayout->addWidget(mNameEdit);

        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(12);
        buttonLayout->addStretch();

        mConfirmButton = new QPushButton(tr("确认"), panel);
        mCancelButton = new QPushButton(tr("取消"), panel);

        const QSize buttonSize(120, 44);
        mConfirmButton->setMinimumSize(buttonSize);
        mCancelButton->setMinimumSize(buttonSize);
        mConfirmButton->setFont(QFont(QStringLiteral("微软雅黑"), 13, QFont::DemiBold));
        mCancelButton->setFont(QFont(QStringLiteral("微软雅黑"), 13, QFont::DemiBold));

        buttonLayout->addWidget(mConfirmButton);
        buttonLayout->addWidget(mCancelButton);

        panelLayout->addLayout(buttonLayout);

        setStyleSheet(QStringLiteral(
            "#ExpInfoDialog {background: transparent;}"
            "#ExpInfoPanel {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0f1b2f, stop:1 #0b1222);"
            "  border: 1px solid #1f2c46;"
            "  border-radius: 16px;"
            "}"
            "#ExpInfoLabel {"
            "  color: #e5edff;"
            "  font-size: 18px;"
            "  font-weight: 700;"
            "}"
            "QLineEdit {"
            "  background: rgba(10, 19, 34, 200);"
            "  border: 1px solid #2b3d5d;"
            "  border-radius: 10px;"
            "  padding: 8px 10px;"
            "  color: #e6f0ff;"
            "}"
            "QLineEdit:focus {"
            "  border-color: #4ac7ff;"
            "}"
            "QPushButton {"
            "  border: 1px solid rgba(93, 150, 255, 120);"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(18, 36, 66, 200), stop:1 rgba(15, 27, 52, 210));"
            "  border-radius: 12px;"
            "  color: #e3ecff;"
            "  letter-spacing: 0.6px;"
            "}"
            "QPushButton:enabled:hover {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(58, 128, 255, 120), stop:1 rgba(25, 69, 130, 160));"
            "  border-color: #6be9ff;"
            "  color: #f3fbff;"
            "}"
            "QPushButton:enabled:pressed {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(31, 67, 118, 200), stop:1 rgba(20, 39, 74, 220));"
            "  border-color: #4ac7ff;"
            "}"
            "QPushButton:disabled {"
            "  color: rgba(227, 236, 255, 120);"
            "  border-color: rgba(93, 150, 255, 60);"
            "  background: rgba(18, 36, 66, 140);"
            "}"
        ));
    }

    void ExpInfoDialog::setupConnections() {
        connect(mConfirmButton, &QPushButton::clicked, this, &ExpInfoDialog::onConfirmClicked);
        connect(mCancelButton, &QPushButton::clicked, this, &ExpInfoDialog::reject);
        connect(mNameEdit, &QLineEdit::textChanged, this, &ExpInfoDialog::onNameChanged);
    }

    void ExpInfoDialog::updateConfirmState() {
        if (!mConfirmButton || !mNameEdit) {
            return;
        }

        const bool hasText = !mNameEdit->text().trimmed().isEmpty();
        mConfirmButton->setEnabled(hasText);
    }

    void ExpInfoDialog::onConfirmClicked() {
        updateConfirmState();
        if (mConfirmButton && mConfirmButton->isEnabled()) {
            accept();
        }
    }

    void ExpInfoDialog::onNameChanged(const QString &text) {
        Q_UNUSED(text);
        updateConfirmState();
    }
}
