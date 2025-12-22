#include "FuVideoButtons.h"

#include <QFile>
#include <QSizePolicy>

namespace {
    QString loadVideoButtonQss() {
        static QString cached;
        static bool loaded = false;

        if (!loaded) {
            QFile qssFile(":/qss/FuVideoButtons.css");
            if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
                cached = QString::fromUtf8(qssFile.readAll());
            }
            loaded = true;
        }

        return cached;
    }
}

QString TF::TechButtonBase::buttonStyleSheet() {
    return loadVideoButtonQss();
}

void TF::TechButtonBase::applySharedSetup() {
    setCursor(Qt::PointingHandCursor);
    setMinimumSize(120, 120);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    setSizePolicy(sizePolicy);
    setStyleSheet(buttonStyleSheet());
}

TF::TechButtonBase::TechButtonBase(const QString& text, QWidget* parent)
    : QPushButton(text, parent) {
    applySharedSetup();
}

TF::TechToggleButton::TechToggleButton(const QString& text, QWidget* parent)
    : TechButtonBase(text, parent) {
    setCheckable(true);
}

TF::TechActionButton::TechActionButton(const QString& text, QWidget* parent)
    : TechButtonBase(text, parent) {
    setCheckable(false);
}
