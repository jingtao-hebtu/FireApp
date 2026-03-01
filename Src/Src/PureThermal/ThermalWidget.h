#ifndef FIREAPP_THERMALWIDGET_H
#define FIREAPP_THERMALWIDGET_H

#include <QWidget>
#include <QImage>
#include "FlameIRMapper.h"

namespace TF {
    class ThermalCamera;

    class ThermalWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit ThermalWidget(ThermalCamera* camera, QWidget* parent = nullptr);

    protected:
        void paintEvent(QPaintEvent* event) override;
        QSize sizeHint() const override { return QSize(640, 480); }

    private slots:
        void onFrameReady(const QImage& image,
                          double minTempC,
                          double maxTempC,
                          double centerTempC);

        void onDetectionResult();

    private:
        QImage m_image;
        double m_minTempC = 0.0;
        double m_maxTempC = 0.0;
        double m_centerTempC = 0.0;

        FlameIRMapper m_flameMapper;

        // Cached IR bbox, updated via detection signal, drawn in paintEvent
        cv::Rect m_cachedIrBbox;
        bool m_hasIrBbox = false;
    };
};

#endif //FIREAPP_THERMALWIDGET_H
