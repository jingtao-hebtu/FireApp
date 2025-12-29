#include "HKCamZmqClient.h"

#include <chrono>
#include <cerrno>
#include <iostream>

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>

namespace
{
    QString JsonToString(const QJsonObject &obj, bool pretty = false)
    {
        const auto json = QJsonDocument(obj).toJson(pretty ? QJsonDocument::Indented : QJsonDocument::Compact);
        return QString::fromUtf8(json);
    }
}

namespace TF
{
    HKCamZmqClient::HKCamZmqClient()
    {
        m_context = std::make_unique<zmq::context_t>(1);
    }

    HKCamZmqClient::~HKCamZmqClient()
    {
        ResetSocket();
    }

    void HKCamZmqClient::ResetSocket()
    {
        if (m_socket)
        {
            try
            {
                m_socket->close();
            }
            catch (...)
            {
            }
            m_socket.reset();
        }
    }

    bool HKCamZmqClient::EnsureSocket(std::string &outError)
    {
        if (!m_context)
        {
            m_context = std::make_unique<zmq::context_t>(1);
        }

        if (m_socket)
        {
            return true;
        }

        try
        {
            m_socket = std::make_unique<zmq::socket_t>(*m_context, zmq::socket_type::req);
            m_socket->set(zmq::sockopt::rcvtimeo, m_timeoutMs);
            m_socket->set(zmq::sockopt::sndtimeo, m_timeoutMs);
            m_socket->set(zmq::sockopt::linger, 0);
            m_socket->connect(m_endpoint);
        }
        catch (const zmq::error_t &e)
        {
            outError = "Failed to create/connect socket: " + std::string(e.what());
            ResetSocket();
            return false;
        }
        catch (const std::exception &e)
        {
            outError = "Failed to create/connect socket: " + std::string(e.what());
            ResetSocket();
            return false;
        }

        return true;
    }

    QJsonObject HKCamZmqClient::BuildRequestObject(const std::string &op, const QJsonObject &params, int requestId) const
    {
        QJsonObject req;
        req.insert("id", requestId);
        req.insert("op", QString::fromStdString(op));
        req.insert("params", params);
        return req;
    }

    RpcResponse HKCamZmqClient::ParseResponse(const std::string &payload, bool &parseOk, std::string &parseError) const
    {
        RpcResponse result;
        result.rawText = QString::fromStdString(payload);
        const auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(payload));
        if (!doc.isObject())
        {
            parseOk = false;
            parseError = "Response is not a JSON object";
            return result;
        }

        parseOk = true;
        const auto obj = doc.object();
        result.id = obj.value("id").toInt();
        result.ok = obj.value("ok").toBool(false);
        if (result.ok)
        {
            if (obj.contains("data"))
            {
                if (obj.value("data").isObject())
                {
                    result.data = obj.value("data").toObject();
                }
                else
                {
                    QJsonObject wrapper;
                    wrapper.insert("value", obj.value("data"));
                    result.data = wrapper;
                }
            }
        }
        else
        {
            if (obj.contains("error") && obj.value("error").isObject())
            {
                const auto errorObj = obj.value("error").toObject();
                result.errorType = errorObj.value("type").toString();
                result.errorMessage = errorObj.value("message").toString();
                result.errorTrace = errorObj.value("trace").toString();
            }
            else
            {
                result.errorMessage = QStringLiteral("Unknown error format");
            }
        }

        return result;
    }

    RpcCallResult HKCamZmqClient::Call(const std::string &op, const QJsonObject &params)
    {
        RpcCallResult callResult;
        std::lock_guard<std::mutex> guard(m_mutex);

        std::string socketError;
        if (!EnsureSocket(socketError))
        {
            callResult.success = false;
            callResult.errorReason = QString::fromStdString(socketError);
            return callResult;
        }

        const int requestId = m_requestId++;
        const auto reqObj = BuildRequestObject(op, params, requestId);
        const auto reqString = JsonToString(reqObj).toStdString();

        try
        {
            zmq::message_t reqMsg(reqString.begin(), reqString.end());
            const bool sent = m_socket->send(reqMsg, zmq::send_flags::none);
            if (!sent)
            {
                callResult.errorReason = QStringLiteral("Failed to send request");
                ResetSocket();
                return callResult;
            }

            zmq::message_t reply;
            const bool received = m_socket->recv(reply, zmq::recv_flags::none);
            if (!received)
            {
                callResult.errorReason = QStringLiteral("Failed to receive reply");
                ResetSocket();
                return callResult;
            }

            const std::string payload(static_cast<char *>(reply.data()), reply.size());
            bool parseOk = false;
            std::string parseError;
            auto resp = ParseResponse(payload, parseOk, parseError);
            if (!parseOk)
            {
                callResult.errorReason = QString::fromStdString(parseError);
                ResetSocket();
                return callResult;
            }

            if (resp.id != requestId)
            {
                callResult.errorReason = QStringLiteral("Mismatched response id");
                ResetSocket();
                return callResult;
            }

            if (!resp.ok)
            {
                callResult.errorReason = QStringLiteral("Server error: ") + resp.errorMessage;
                callResult.response = resp;
                callResult.success = false;
                return callResult;
            }

            callResult.success = true;
            callResult.response = resp;
        }
        catch (const zmq::error_t &e)
        {
            if (e.num() == EAGAIN)
            {
                callResult.errorReason = QStringLiteral("Timeout during request, socket reset");
                ResetSocket();
            }
            else
            {
                callResult.errorReason = QString::fromLatin1(e.what());
            }
        }
        catch (const std::exception &e)
        {
            callResult.errorReason = QString::fromLatin1(e.what());
        }

        return callResult;
    }

    bool HKCamZmqClient::Connect(const std::string &endpoint, int timeoutMs, int retries, std::string &outError)
    {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_endpoint = endpoint;
            m_timeoutMs = timeoutMs;
            m_requestId = 1;
            ResetSocket();
        }

        for (int i = 0; i < retries; ++i)
        {
            RpcResponse resp;
            std::string err;
            if (!Ping(resp, err))
            {
                outError = "Ping failed: " + err;
                continue;
            }

            if (!ReconnectDevice(resp, err))
            {
                outError = "Reconnect failed: " + err;
                continue;
            }

            QJsonObject deviceInfo;
            if (!GetDeviceInfo(deviceInfo, err))
            {
                outError = "Device info failed: " + err;
                continue;
            }

            outError.clear();
            return true;
        }

        if (outError.empty())
        {
            outError = "Connect retries exhausted";
        }

        return false;
    }

    bool HKCamZmqClient::Ping(RpcResponse &outResponse, std::string &outError)
    {
        const auto result = Call("ping", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outResponse = result.response;
        return true;
    }

    bool HKCamZmqClient::ReconnectDevice(RpcResponse &outResponse, std::string &outError)
    {
        const auto result = Call("reconnect", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outResponse = result.response;
        return true;
    }

    bool HKCamZmqClient::GetDeviceInfo(QJsonObject &outInfo, std::string &outError)
    {
        const auto result = Call("device_info", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outInfo = result.response.data;
        return true;
    }

    bool HKCamZmqClient::GetZoom(double &zoom, QJsonObject &outRaw, std::string &outError)
    {
        const auto result = Call("get_zoom", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }

        outRaw = result.response.data;
        if (result.response.data.contains("value"))
        {
            zoom = result.response.data.value("value").toDouble();
        }
        else if (result.response.data.contains("zoom"))
        {
            zoom = result.response.data.value("zoom").toDouble();
        }
        else
        {
            zoom = 0.0;
        }
        return true;
    }

    bool HKCamZmqClient::GetZoomRange(QJsonObject &outRange, std::string &outError)
    {
        const auto result = Call("get_zoom_range", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRange = result.response.data;
        return true;
    }

    bool HKCamZmqClient::ZoomStep(double step, std::optional<double> speed, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params;
        params.insert("step", step);
        if (speed.has_value())
        {
            params.insert("speed", speed.value());
        }

        const auto result = Call("zoom_step", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::SetZoomAbs(double zoom, std::optional<double> speed, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params;
        params.insert("zoom", zoom);
        if (speed.has_value())
        {
            params.insert("speed", speed.value());
        }

        const auto result = Call("set_zoom_abs", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::GetExposure(QJsonObject &outExposure, std::string &outError)
    {
        const auto result = Call("get_exposure", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outExposure = result.response.data;
        return true;
    }

    bool HKCamZmqClient::GetExposureRange(QJsonObject &outRange, std::string &outError)
    {
        const auto result = Call("get_exposure_range", QJsonObject());
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRange = result.response.data;
        return true;
    }

    bool HKCamZmqClient::SetExposureByShutter(const std::string &shutter, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params;
        params.insert("shutter", QString::fromStdString(shutter));
        params.insert("persist", persist);
        params.insert("clamp", clamp);

        const auto result = Call("set_exposure", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::SetExposureBySeconds(double exposureSeconds, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params;
        params.insert("exposure_s", exposureSeconds);
        params.insert("persist", persist);
        params.insert("clamp", clamp);

        const auto result = Call("set_exposure", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::SetExposureByMicroseconds(double exposureMicroseconds, bool persist, bool clamp, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params;
        params.insert("exposure_us", exposureMicroseconds);
        params.insert("persist", persist);
        params.insert("clamp", clamp);

        const auto result = Call("set_exposure", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::AutoFocusOnce(const QJsonObject &params, QJsonObject &outRaw, std::string &outError)
    {
        const auto result = Call("autofocus_once", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }

    bool HKCamZmqClient::ZoomStepAutoFocus(double step, std::optional<double> speed, const QJsonObject &extraParams, QJsonObject &outRaw, std::string &outError)
    {
        QJsonObject params = extraParams;
        params.insert("step", step);
        if (speed.has_value())
        {
            params.insert("speed", speed.value());
        }

        const auto result = Call("zoom_step_af", params);
        if (!result.success)
        {
            outError = result.errorReason.toStdString();
            return false;
        }
        outRaw = result.response.data;
        return true;
    }
}

