#pragma once

#include <QString>
#include <QJsonObject>
#include <optional>

namespace TF
{
    struct RpcResponse
    {
        bool ok = false;
        int id = 0;
        QJsonObject data;
        QString errorType;
        QString errorMessage;
        QString errorTrace;
        QString rawText;
    };

    struct RpcCallResult
    {
        bool success = false;
        QString errorReason;
        RpcResponse response;
    };
}

