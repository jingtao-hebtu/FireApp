#ifndef FIREAPP_THERMALCAMERA_H
#define FIREAPP_THERMALCAMERA_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QByteArray>
#include <QStringList>
#include <atomic>
#include <thread>


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

        // 保存原始温度数据（uint16 Y16）到二进制文件
        void saveRawFrame(const QString& filename, const uvc_frame* frame);

        // 获取最新的伪彩色图像快照
        QImage latestImage() const;

        // 获取最新原始帧数据快照（含 width/height 头 + uint16 数据）
        QByteArray latestRawData() const;

        // 获取最新红外测量的最低/最高温度 (°C)
        double latestMinTemp() const;
        double latestMaxTemp() const;

    signals:
        void frameReady(const QImage& image,
                        double minTempC,
                        double maxTempC,
                        double centerTempC);

    private:
        bool mIsSim {false};
        std::string mSimDatFolder;

        static void frameCallback(uvc_frame* frame, void* user);
        void handleFrame(uvc_frame* frame);

        // 仿真线程循环
        void simLoop();

        uvc_context* m_ctx = nullptr;
        uvc_device* m_dev = nullptr;
        uvc_device_handle* m_devh = nullptr;
        uvc_stream_ctrl* m_ctrl = nullptr;
        bool m_running = false;

        // 仿真相关
        std::atomic<bool> m_simRunning {false};
        std::thread m_simThread;
        QStringList m_simFileList;

        mutable QMutex m_mutex;

        // 缓存最新帧数据，供实验保存时使用
        QImage m_lastImage;
        QByteArray m_lastRawData;   // width(int32) + height(int32) + uint16[]
        double m_lastMinTempC = 0.0;
        double m_lastMaxTempC = 0.0;
    };
};


#endif //FIREAPP_THERMALCAMERA_H
