#pragma once

#include <mutex>
#include <optional>
#include <string>

#include <QJsonObject>

#include <zmq.h>
#include <zmq.hpp>

#include "HKCamTypes.h"

namespace TF
{
    class HKCamZmqClient
    {
    public:
        HKCamZmqClient();
        ~HKCamZmqClient();

        void Configure(const std::string &endpoint, int timeoutMs);
        bool Connect(const std::string &endpoint, int timeoutMs, int retries, std::string &outError);

        bool Ping(RpcResponse &outResponse, std::string &outError);
        bool ReconnectDevice(RpcResponse &outResponse, std::string &outError);
        bool GetDeviceInfo(QJsonObject &outInfo, std::string &outError);
        bool GetZoom(double &zoom, QJsonObject &outRaw, std::string &outError);
        bool GetZoomRange(QJsonObject &outRange, std::string &outError);
        bool ZoomStep(double step, std::optional<double> speed, QJsonObject &outRaw, std::string &outError);
        bool SetZoomAbs(double zoom, std::optional<double> speed, QJsonObject &outRaw, std::string &outError);
        bool GetExposure(QJsonObject &outExposure, std::string &outError);
        bool GetExposureRange(QJsonObject &outRange, std::string &outError);
        bool SetExposureByShutter(const std::string &shutter, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError);
        bool SetExposureBySeconds(double exposureSeconds, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError);
        bool SetExposureByMicroseconds(double exposureMicroseconds, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError);
        bool AutoFocusOnce(const QJsonObject &params, QJsonObject &outRaw, std::string &outError);
        bool ZoomStepAutoFocus(double step, std::optional<double> speed, const QJsonObject &extraParams, QJsonObject &outRaw, std::string &outError);
        bool Shutdown(QJsonObject &outData, std::string &outError);

    private:
        bool EnsureSocket(std::string &outError);
        void ResetSocket();
        RpcCallResult Call(const std::string &op, const QJsonObject &params);
        RpcResponse ParseResponse(const std::string &payload, bool &parseOk, std::string &parseError) const;
        QJsonObject BuildRequestObject(const std::string &op, const QJsonObject &params, int requestId) const;

    private:
        std::mutex m_mutex;
        std::unique_ptr<zmq::context_t> m_context;
        std::unique_ptr<zmq::socket_t> m_socket;
        std::string m_endpoint;
        int m_timeoutMs = 3000;
        int m_requestId = 1;
    };
}

