#include "ThermalCamera.h"
#include "libuvc/libuvc.h"
#include "TConfig.h"
#include "TLog.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <QFile>
#include <QDir>
#include <QFileInfo>


static constexpr uint16_t PT_VID = 0x1e4e; // PureThermal / GroupGets
static constexpr uint16_t PT_PID = 0x0100;

namespace TF {

    ThermalCamera::ThermalCamera(QObject* parent)
        : QObject(parent) {
    }

    ThermalCamera::~ThermalCamera() {
        stop();
    }

    bool ThermalCamera::start(int width, int height, int fps) {
        if (m_running)
            return true;

        mIsSim = GET_BOOL_CONFIG("ThermalCam", "Sim");

        // Todo: judge if sim
        if (mIsSim) {
            mSimDatFolder = GET_STR_CONFIG("ThermalCam", "SimDatFolder");
            // Todo: judge if exist this folder
        }

        uvc_error_t res;

        // 1. 初始化 libuvc
        res = uvc_init(&m_ctx, nullptr);
        if (res < 0) {
            LOG_F(ERROR, "uvc_init failed (%d): %s.", res, uvc_strerror(res));
            m_ctx = nullptr;
            return false;
        }

        // 2. 查找设备
        // 简单写法：第一个 UVC 设备。你可以改成指定 VID/PID（PureThermal 通常 0x1e4e / 0x0100 一类）
        res = uvc_find_device(m_ctx, &m_dev, PT_VID, PT_PID, nullptr);
        if (res < 0) {
            LOG_F(ERROR, "uvc_find_device failed (%d): %s.", res, uvc_strerror(res));
            uvc_exit(m_ctx);
            m_ctx = nullptr;
            return false;
        }

        // 3. 打开设备
        res = uvc_open(m_dev, &m_devh);
        if (res < 0) {
            LOG_F(ERROR, "uvc_open failed (%d): %s.", res, uvc_strerror(res));
            uvc_unref_device(m_dev);
            m_dev = nullptr;
            uvc_exit(m_ctx);
            m_ctx = nullptr;
            return false;
        }

        // 4. 配置 Y16 流参数
        m_ctrl = new uvc_stream_ctrl_t;
        res = uvc_get_stream_ctrl_format_size(
            m_devh,
            m_ctrl,
            UVC_FRAME_FORMAT_Y16, // 16-bit 灰度（保留 radiometric 数据）
            width,
            height,
            fps
        );
        if (res < 0) {
            LOG_F(ERROR, "uvc_get_stream_ctrl_format_size failed (%d): %s.", res, uvc_strerror(res));
            delete m_ctrl;
            m_ctrl = nullptr;
            uvc_close(m_devh);
            m_devh = nullptr;
            uvc_unref_device(m_dev);
            m_dev = nullptr;
            uvc_exit(m_ctx);
            m_ctx = nullptr;
            return false;
        }

        // 5. 开始流
        res = uvc_start_streaming(m_devh, m_ctrl,
                                  &ThermalCamera::frameCallback,
                                  this,
                                  0 // flags
        );
        if (res < 0) {
            LOG_F(ERROR, "uvc_start_streaming failed (%d): %s.", res, uvc_strerror(res));
            delete m_ctrl;
            m_ctrl = nullptr;
            uvc_close(m_devh);
            m_devh = nullptr;
            uvc_unref_device(m_dev);
            m_dev = nullptr;
            uvc_exit(m_ctx);
            m_ctx = nullptr;
            return false;
        }

        m_running = true;

        // 可选：自动曝光
        uvc_set_ae_mode(m_devh, 1);

        LOG_F(INFO, "ThermalCamera started.");
        return true;
    }

    void ThermalCamera::stop() {
        if (!m_ctx)
            return;

        if (m_devh) {
            uvc_stop_streaming(m_devh);
            uvc_close(m_devh);
            m_devh = nullptr;
        }

        if (m_dev) {
            uvc_unref_device(m_dev);
            m_dev = nullptr;
        }

        if (m_ctrl) {
            delete m_ctrl;
            m_ctrl = nullptr;
        }

        uvc_exit(m_ctx);
        m_ctx = nullptr;
        m_running = false;

        LOG_F(INFO, "ThermalCamera stopped.");
    }

    void ThermalCamera::frameCallback(uvc_frame* frame, void* user) {
        auto* self = static_cast<ThermalCamera*>(user);
        if (!self)
            return;
        self->handleFrame(frame);
    }

    void ThermalCamera::handleFrame(uvc_frame* frame) {
        if (!frame || !frame->data)
            return;

        const int w = static_cast<int>(frame->width);
        const int h = static_cast<int>(frame->height);
        const int pixelCount = w * h;

        const size_t expectedBytes = static_cast<size_t>(pixelCount) * 2;
        if (frame->data_bytes < expectedBytes)
            return;

        const auto* src = static_cast<const uint16_t*>(frame->data);

        // 1. 找到原始 16bit 数据的 min/max，用于对比度拉伸
        uint16_t minVal = (std::numeric_limits<uint16_t>::max)();
        uint16_t maxVal = 0;

        for (int i = 0; i < pixelCount; ++i) {
            const uint16_t v = src[i];
            if (v < minVal) minVal = v;
            if (v > maxVal) maxVal = v;
        }

        double range = static_cast<double>(maxVal - minVal);
        if (range < 1.0)
            range = 1.0;

        // 2. 创建伪彩色图像（RGB32）
        QImage image(w, h, QImage::Format_RGB32);
        auto* dst = reinterpret_cast<QRgb*>(image.bits());

        double minTempC = std::numeric_limits<double>::infinity();
        double maxTempC = -std::numeric_limits<double>::infinity();
        double centerTempC = 0.0;
        const int centerIndex = (h / 2) * w + (w / 2);

        for (int i = 0; i < pixelCount; ++i) {
            const uint16_t raw = src[i];

            // 2.1 归一化到 0..1
            double norm = (raw - minVal) / range;
            if (norm < 0.0) norm = 0.0;
            if (norm > 1.0) norm = 1.0;

            // 2.2 伪彩色映射：蓝 → 青 → 绿 → 黄 → 红
            // 用 HSV 做一个简单色带：0.66（蓝）到 0.0（红）
            const double hHue = (1.0 - norm) * 0.66; // 0.66 ≈ 240°（蓝）
            const double sSat = 1.0;
            const double vVal = 1.0;

            const QColor color = QColor::fromHsvF(hHue, sSat, vVal);
            dst[i] = color.rgb();

            // 3. 温度计算（保持之前的逻辑）
            const double tempC = raw / 100.0 - 273.15; // T(°C) = raw/100 - 273.15

            if (tempC < minTempC) minTempC = tempC;
            if (tempC > maxTempC) maxTempC = tempC;
            if (i == centerIndex)
                centerTempC = tempC;
        }

        // 缓存最新帧数据（伪彩色图像 + 原始 uint16 数据）
        {
            QMutexLocker lk(&m_mutex);
            m_lastImage = image.copy();

            // 序列化格式: width(int32) + height(int32) + raw uint16 data
            const qsizetype headerSize = static_cast<qsizetype>(sizeof(int) * 2);
            const qsizetype dataSize = static_cast<qsizetype>(expectedBytes);
            m_lastRawData.resize(headerSize + dataSize);
            char* p = m_lastRawData.data();
            std::memcpy(p, &w, sizeof(int));
            std::memcpy(p + sizeof(int), &h, sizeof(int));
            std::memcpy(p + headerSize, src, expectedBytes);
        }

        emit frameReady(image, minTempC, maxTempC, centerTempC);
    }

    void ThermalCamera::saveRawFrame(const QString& filename, const uvc_frame* frame) {
        if (!frame || !frame->data)
            return;

        const int w = static_cast<int>(frame->width);
        const int h = static_cast<int>(frame->height);
        const size_t expectedBytes = static_cast<size_t>(w * h) * 2;
        if (frame->data_bytes < expectedBytes)
            return;

        QDir dir(QFileInfo(filename).absolutePath());
        if (!dir.exists())
            dir.mkpath(".");

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            LOG_F(ERROR, "Failed to open raw frame file: %s", filename.toStdString().c_str());
            return;
        }

        // 写入头: width(int32) + height(int32)
        file.write(reinterpret_cast<const char*>(&w), sizeof(int));
        file.write(reinterpret_cast<const char*>(&h), sizeof(int));
        // 写入原始 uint16 温度数据
        file.write(static_cast<const char*>(frame->data),
                   static_cast<qint64>(expectedBytes));
        file.close();
    }

    QImage ThermalCamera::latestImage() const {
        QMutexLocker lk(&m_mutex);
        return m_lastImage;
    }

    QByteArray ThermalCamera::latestRawData() const {
        QMutexLocker lk(&m_mutex);
        return m_lastRawData;
    }
}
