#include "ThermalWidget.h"
#include "ThermalCamera.h"

#include <QFile>
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>


namespace TF {
    ThermalWidget::ThermalWidget(ThermalCamera* camera, QWidget* parent)
        : QWidget(parent) {
        connect(camera, &ThermalCamera::frameReady,
                this, &ThermalWidget::onFrameReady,
                Qt::QueuedConnection);
    }

    void ThermalWidget::onFrameReady(const QImage& image,
                                     double minTempC,
                                     double maxTempC,
                                     double centerTempC) {
        // 拷贝一份，避免和采集线程共享同一内存
        m_image = image.copy();
        m_minTempC = minTempC;
        m_maxTempC = maxTempC;
        m_centerTempC = centerTempC;

        update(); // 触发重绘
    }



    void ThermalWidget::paintEvent(QPaintEvent* event) {
        Q_UNUSED(event);

        if (!m_image.isNull()) {
            QPainter p(this);

            QImage scaled = m_image.scaled(size(), Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
            const QPoint topLeft((width() - scaled.width()) / 2,
                                 (height() - scaled.height()) / 2);
            p.drawImage(topLeft, scaled);

            p.setPen(QColor(95, 217, 126));
            const QString text = QStringLiteral("Min: %1 °C   Max: %2 °C   Center: %3 °C")
                                 .arg(m_minTempC, 0, 'f', 1)
                                 .arg(m_maxTempC, 0, 'f', 1)
                                 .arg(m_centerTempC, 0, 'f', 1);

            p.drawText(10, height() - 10, text);
        }

    }
};
