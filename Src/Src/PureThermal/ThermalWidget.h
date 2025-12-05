#ifndef FIREAPP_THERMALWIDGET_H
#define FIREAPP_THERMALWIDGET_H

#include <QWidget>
#include <QImage>

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

    private:
        QImage m_image;
        double m_minTempC = 0.0;
        double m_maxTempC = 0.0;
        double m_centerTempC = 0.0;
    };
};

#endif //FIREAPP_THERMALWIDGET_H
