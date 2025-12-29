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

namespace TF {

    ExpInfoDialog::ExpInfoDialog(QWidget *parent) : QDialog(parent) {
        setWindowTitle(tr("实验信息"));
        setModal(true);

        setupUi();
        setupConnections();
        updateConfirmState();
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

        mNameEdit->setText(name.trimmed());
        mNameEdit->setCursorPosition(mNameEdit->text().length());
        updateConfirmState();
    }

    void ExpInfoDialog::setupUi() {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(16, 16, 16, 16);
        mainLayout->setSpacing(12);

        auto *nameLabel = new QLabel(tr("实验名称"), this);
        mNameEdit = new QLineEdit(this);
        mNameEdit->setPlaceholderText(tr("请输入实验名称"));

        mainLayout->addWidget(nameLabel);
        mainLayout->addWidget(mNameEdit);

        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(8);
        buttonLayout->addStretch();

        mConfirmButton = new QPushButton(tr("确认"), this);
        mCancelButton = new QPushButton(tr("取消"), this);

        buttonLayout->addWidget(mConfirmButton);
        buttonLayout->addWidget(mCancelButton);

        mainLayout->addLayout(buttonLayout);
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
