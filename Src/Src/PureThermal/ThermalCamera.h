#ifndef FIREAPP_THERMALCAMERA_H
#define FIREAPP_THERMALCAMERA_H

#include <QObject>
#include <QImage>
#include <QMutex>


struct uvc_context;
struct uvc_device;
struct uvc_device_handle;
struct uvc_stream_ctrl;
struct uvc_frame;


namespace TF {
    class ThermalCamera : public QObject
    {
        Q_OBJECT

    public:
        explicit ThermalCamera(QObject* parent = nullptr);
        ~ThermalCamera() override;

        bool start(int width = 160, int height = 120, int fps = 9);
        void stop();

        bool isRunning() const { return m_running; }

    signals:
        void frameReady(const QImage& image,
                        double minTempC,
                        double maxTempC,
                        double centerTempC);

    private:
        static void frameCallback(uvc_frame* frame, void* user);
        void handleFrame(uvc_frame* frame);

        uvc_context* m_ctx = nullptr;
        uvc_device* m_dev = nullptr;
        uvc_device_handle* m_devh = nullptr;
        uvc_stream_ctrl* m_ctrl = nullptr;
        bool m_running = false;

        mutable QMutex m_mutex;
    };
};


#endif //FIREAPP_THERMALCAMERA_H
