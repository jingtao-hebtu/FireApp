#include "ThermalWidget.h"
#include "ThermalCamera.h"
#include "DetectManager.h"
#include "DetectorWorkerManager.h"
#include "TFMeaManager.h"

#include <QFile>
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>


namespace TF {

    // H matrix calibrated for the rotated IR image coordinate system
    static const double H_DEFAULT[9] = {
         8.10858805e-02,  3.05423619e-03, -1.46287700e+01,
         1.05866057e-03,  8.91527766e-02,  2.84689397e+01,
        -1.35057002e-05,  8.91962303e-05,  1.00000000e+00
    };

    ThermalWidget::ThermalWidget(ThermalCamera* camera, QWidget* parent)
        : QWidget(parent) {
        connect(camera, &ThermalCamera::frameReady,
                this, &ThermalWidget::onFrameReady,
                Qt::QueuedConnection);

        // When detection produces a new result, update the cached IR bbox
        connect(&DetectorWorkerManager::instance(),
                &DetectorWorkerManager::frameProcessed,
                this, &ThermalWidget::onDetectionResult,
                Qt::QueuedConnection);

        // Initialize IR mapper: 120 wide x 160 tall (after 90-degree rotation of 160x120)
        m_flameMapper.init(H_DEFAULT, 120, 160);
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

    void ThermalWidget::onDetectionResult() {
        // Read the latest flame bbox from TFMeaManager and map to IR coordinates.
        // This is called each time the detection worker produces a new result,
        // ensuring the IR bbox tracks the visible-light bbox in real-time.
        m_hasIrBbox = false;

        if (!m_flameMapper.isReady()) {
            return;
        }
        if (!TFDetectManager::instance().isDetecting()) {
            return;
        }
        if (!TFMeaManager::instance().isFlameDetected()) {
            return;
        }

        cv::Rect visBbox = TFMeaManager::instance().flameBbox();
        if (visBbox.area() <= 0) {
            return;
        }

        cv::Rect irBbox;
        if (m_flameMapper.mapBbox(visBbox, irBbox)) {
            m_cachedIrBbox = irBbox;
            m_hasIrBbox = true;
        }

        update(); // 触发重绘以显示最新bbox
    }

    void ThermalWidget::paintEvent(QPaintEvent* event) {
        Q_UNUSED(event);

        if (!m_image.isNull()) {
            QPainter p(this);

            // Use a local variable for rotation to avoid corrupting m_image
            // when paintEvent fires multiple times between onFrameReady calls
            QImage rotated = m_image.transformed(QTransform().rotate(90));
            //QImage scaled = rotated.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QImage scaled = rotated.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            const QPoint topLeft((width() - scaled.width()) / 2,
                                 (height() - scaled.height()) / 2);
            p.drawImage(topLeft, scaled);

            // Draw cached IR flame bbox
            if (m_hasIrBbox && TFDetectManager::instance().isDetecting()) {
                const double sx = static_cast<double>(scaled.width())  / rotated.width();
                const double sy = static_cast<double>(scaled.height()) / rotated.height();

                QRect displayRect(
                    topLeft.x() + static_cast<int>(m_cachedIrBbox.x * sx),
                    topLeft.y() + static_cast<int>(m_cachedIrBbox.y * sy),
                    static_cast<int>(m_cachedIrBbox.width * sx),
                    static_cast<int>(m_cachedIrBbox.height * sy)
                );

                p.setPen(QPen(QColor(255, 0, 0), 2));
                p.setBrush(Qt::NoBrush);
                p.drawRect(displayRect);
            }

            p.setPen(QColor(95, 217, 126));
            const QString text = QStringLiteral("Min: %1 °C   Max: %2 °C   Center: %3 °C")
                                 .arg(m_minTempC, 0, 'f', 1)
                                 .arg(m_maxTempC, 0, 'f', 1)
                                 .arg(m_centerTempC, 0, 'f', 1);

            p.drawText(10, height() - 10, text);
        }

    }
};
