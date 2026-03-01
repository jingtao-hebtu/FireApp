#include "ThermalWidget.h"
#include "ThermalCamera.h"
#include "DetectManager.h"
#include "TFMeaManager.h"
#include "PathConfig.h"
#include "TConfig.h"
#include "TSysUtils.h"

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

        mEnablePlotBBox = GET_BOOL_CONFIG("ThermalCam", "EnablePlotBBox");
        mBBoxWOffset = GET_FLOAT_CONFIG("ThermalCam", "BBoxWOffset");
        mBBoxHOffset = GET_FLOAT_CONFIG("ThermalCam", "BBoxHOffset");

        // Initialize IR mapper: 120 wide x 160 tall (after 90-degree rotation of 160x120)
        //m_flameMapper.init(H_DEFAULT, 120, 160);

        auto app_config_dir = TFPathParam("AppConfigDir");
        auto pt_config_path = TBase::joinPath({app_config_dir, "PTConfig", "calibration_result.json"});
        m_flameMapper.loadFromJson(pt_config_path);
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

            // Use a local variable for rotation to avoid corrupting m_image
            // when paintEvent fires multiple times between onFrameReady calls
            QImage rotated = m_image.transformed(QTransform().rotate(90));
            //QImage scaled = rotated.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QImage scaled = rotated.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            const QPoint topLeft((width() - scaled.width()) / 2,
                                 (height() - scaled.height()) / 2);
            p.drawImage(topLeft, scaled);

            // Poll bbox from TFMeaManager on each thermal frame.
            // Only draw when AI is active AND flame is currently detected.
            if (mEnablePlotBBox) {
                if (m_flameMapper.isReady()
               && TFDetectManager::instance().isDetecting()
               && TFMeaManager::instance().isFlameDetected()) {

                    cv::Rect visBbox = TFMeaManager::instance().flameBbox();
                    cv::Rect irBbox;
                    if (visBbox.area() > 0 && m_flameMapper.mapBbox(visBbox, irBbox)) {
                        const double sx = static_cast<double>(scaled.width())  / rotated.width();
                        const double sy = static_cast<double>(scaled.height()) / rotated.height();

                        const int bboxW = static_cast<int>(irBbox.width  * sx);
                        const int bboxH = static_cast<int>(irBbox.height * sy);
                        QRect displayRect(
                            topLeft.x() + static_cast<int>(irBbox.x * sx) + static_cast<int>(bboxW * mBBoxWOffset),
                            topLeft.y() + static_cast<int>(irBbox.y * sy) + static_cast<int>(bboxH * mBBoxHOffset),
                            bboxW,
                            bboxH
                        );

                        p.setPen(QPen(QColor(255, 0, 0), 2));
                        p.setBrush(Qt::NoBrush);
                        p.drawRect(displayRect);
                    }
               }
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
