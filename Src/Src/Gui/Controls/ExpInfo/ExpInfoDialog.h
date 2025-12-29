/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : ExpInfoDialog.h
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREAPP_EXPINFODIALOG_H
#define FIREAPP_EXPINFODIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

namespace TF {

    class ExpInfoDialog : public QDialog {
        Q_OBJECT
    public:
        explicit ExpInfoDialog(QWidget *parent = nullptr);

        [[nodiscard]] QString experimentName() const;
        void setExperimentName(const QString &name);

    private slots:
        void onConfirmClicked();
        void onNameChanged(const QString &text);

    private:
        void setupUi();
        void setupConnections();
        void updateConfirmState();

    private:
        QLineEdit *mNameEdit {nullptr};
        QPushButton *mConfirmButton {nullptr};
        QPushButton *mCancelButton {nullptr};
    };
}

#endif //FIREAPP_EXPINFODIALOG_H
