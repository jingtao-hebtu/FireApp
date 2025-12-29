#include <iostream>
#include <optional>
#include <string>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "HKCamPythonServer.h"
#include "HKCamZmqClient.h"

using TF::HKCamZmqClient;
using TF::HKCamPythonServer;
using TF::HKCamServerConfig;

namespace
{
    void PrintJson(const QJsonObject &obj)
    {
        std::cout << QJsonDocument(obj).toJson(QJsonDocument::Indented).toStdString() << std::endl;
    }

    void PrintResponse(const std::string &title, bool success, const QJsonObject &data, const std::string &error)
    {
        std::cout << "==== " << title << " ====" << std::endl;
        if (success)
        {
            PrintJson(data);
        }
        else
        {
            std::cout << "Failed: " << error << std::endl;
        }
    }

    std::optional<double> ReadOptionalDouble(const std::string &prompt)
    {
        std::cout << prompt << " (empty to skip): ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty())
        {
            return std::nullopt;
        }
        try
        {
            return std::stod(input);
        }
        catch (...)
        {
            std::cout << "Invalid number, skipping" << std::endl;
            return std::nullopt;
        }
    }

    void PrintMenu()
    {
        std::cout << "1 ping\n"
                  << "2 device_info\n"
                  << "3 get_zoom\n"
                  << "4 get_zoom_range\n"
                  << "5 zoom_step\n"
                  << "6 set_zoom_abs\n"
                  << "7 get_exposure_range\n"
                  << "8 get_exposure\n"
                  << "9 set_exposure\n"
                  << "10 reconnect\n"
                  << "11 autofocus_once\n"
                  << "12 zoom_step_af\n"
                  << "99 shutdown_server\n"
                  << "q quit\n";
    }
}

int HKCamTest(int argc, char **argv)
{
    HKCamServerConfig serverCfg;
    serverCfg.endpoint = "tcp://127.0.0.1:5555";
    serverCfg.timeoutMs = 3000;
    int retries = 3;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--endpoint" && i + 1 < argc)
        {
            serverCfg.endpoint = argv[++i];
        }
        else if (arg == "--timeout" && i + 1 < argc)
        {
            serverCfg.timeoutMs = std::stoi(argv[++i]);
        }
        else if (arg == "--retries" && i + 1 < argc)
        {
            retries = std::stoi(argv[++i]);
        }
        else if (arg == "--python" && i + 1 < argc)
        {
            serverCfg.pythonExe = argv[++i];
        }
        else if (arg == "--script" && i + 1 < argc)
        {
            serverCfg.scriptPath = argv[++i];
        }
        else if (arg == "--ip" && i + 1 < argc)
        {
            serverCfg.cameraIp = argv[++i];
        }
        else if (arg == "--user" && i + 1 < argc)
        {
            serverCfg.username = argv[++i];
        }
        else if (arg == "--password" && i + 1 < argc)
        {
            serverCfg.password = argv[++i];
        }
        else if (arg == "--port" && i + 1 < argc)
        {
            serverCfg.cameraPort = std::stoi(argv[++i]);
        }
        else if (arg == "--profile-index" && i + 1 < argc)
        {
            serverCfg.profileIndex = std::stoi(argv[++i]);
        }
    }

    HKCamPythonServer server;
    std::string serverErr;
    if (!server.StartBlocking(serverCfg, serverErr))
    {
        std::cerr << "Failed to start python server: " << serverErr << std::endl;
        return 1;
    }

    auto &client = HKCamZmqClient::instance();
    std::string connectErr;
    if (!client.Connect(serverCfg.endpoint, serverCfg.timeoutMs, retries, connectErr))
    {
        std::cerr << "Connect failed: " << connectErr << std::endl;
        server.Stop();
        return 1;
    }
    std::cout << "Connected to " << serverCfg.endpoint << std::endl;

    while (true)
    {
        PrintMenu();
        std::cout << "Select command: ";
        std::string cmd;
        if (!std::getline(std::cin, cmd))
        {
            break;
        }

        if (cmd == "q" || cmd == "Q")
        {
            break;
        }

        int choice = 0;
        try
        {
            choice = std::stoi(cmd);
        }
        catch (...)
        {
            std::cout << "Invalid input" << std::endl;
            continue;
        }

        bool ok = false;
        std::string err;
        QJsonObject data;

        switch (choice)
        {
            case 1:
            {
                TF::RpcResponse resp;
                ok = client.Ping(resp, err);
                data = resp.data;
                PrintResponse("ping", ok, data, err);
                break;
            }
            case 2:
            {
                ok = client.GetDeviceInfo(data, err);
                PrintResponse("device_info", ok, data, err);
                break;
            }
            case 3:
            {
                double zoom = 0.0;
                ok = client.GetZoom(zoom, data, err);
                if (ok)
                {
                    data.insert("zoom", zoom);
                }
                PrintResponse("get_zoom", ok, data, err);
                break;
            }
            case 4:
            {
                ok = client.GetZoomRange(data, err);
                PrintResponse("get_zoom_range", ok, data, err);
                break;
            }
            case 5:
            {
                std::cout << "step: ";
                std::string stepStr;
                std::getline(std::cin, stepStr);
                const double step = std::stod(stepStr);
                auto speed = ReadOptionalDouble("speed");
                ok = client.ZoomStep(step, speed, data, err);
                PrintResponse("zoom_step", ok, data, err);
                break;
            }
            case 6:
            {
                std::cout << "zoom: ";
                std::string zoomStr;
                std::getline(std::cin, zoomStr);
                const double zoom = std::stod(zoomStr);
                auto speed = ReadOptionalDouble("speed");
                ok = client.SetZoomAbs(zoom, speed, data, err);
                PrintResponse("set_zoom_abs", ok, data, err);
                break;
            }
            case 7:
            {
                ok = client.GetExposureRange(data, err);
                PrintResponse("get_exposure_range", ok, data, err);
                break;
            }
            case 8:
            {
                ok = client.GetExposure(data, err);
                PrintResponse("get_exposure", ok, data, err);
                break;
            }
            case 9:
            {
                std::cout << "Choose exposure mode: 1=shutter,2=seconds,3=microseconds: ";
                std::string modeStr;
                std::getline(std::cin, modeStr);
                int mode = std::stoi(modeStr);
                bool persist = true;
                bool clamp = true;
                std::cout << "persist (1/0, default 1): ";
                std::string persistStr;
                std::getline(std::cin, persistStr);
                if (!persistStr.empty())
                {
                    persist = persistStr != "0";
                }
                std::cout << "clamp (1/0, default 1): ";
                std::string clampStr;
                std::getline(std::cin, clampStr);
                if (!clampStr.empty())
                {
                    clamp = clampStr != "0";
                }

                if (mode == 1)
                {
                    std::cout << "shutter (e.g. 1/60): ";
                    std::string shutter;
                    std::getline(std::cin, shutter);
                    ok = client.SetExposureByShutter(shutter, persist, clamp, data, err);
                }
                else if (mode == 2)
                {
                    std::cout << "exposure seconds: ";
                    std::string expStr;
                    std::getline(std::cin, expStr);
                    ok = client.SetExposureBySeconds(std::stod(expStr), persist, clamp, data, err);
                }
                else if (mode == 3)
                {
                    std::cout << "exposure microseconds: ";
                    std::string expStr;
                    std::getline(std::cin, expStr);
                    ok = client.SetExposureByMicroseconds(std::stod(expStr), persist, clamp, data, err);
                }
                else
                {
                    std::cout << "Unknown mode" << std::endl;
                }
                PrintResponse("set_exposure", ok, data, err);
                break;
            }
            case 10:
            {
                TF::RpcResponse resp;
                ok = client.ReconnectDevice(resp, err);
                data = resp.data;
                PrintResponse("reconnect", ok, data, err);
                break;
            }
            case 11:
            {
                QJsonObject params;
                auto wait = ReadOptionalDouble("wait (1/0)");
                if (wait.has_value())
                {
                    params.insert("wait", wait.value() != 0.0);
                }
                auto timeout = ReadOptionalDouble("timeout_s");
                if (timeout.has_value())
                {
                    params.insert("timeout_s", timeout.value());
                }
                auto restore = ReadOptionalDouble("restore (1/0)");
                if (restore.has_value())
                {
                    params.insert("restore", restore.value() != 0.0);
                }
                auto toggleManual = ReadOptionalDouble("toggle_manual (1/0)");
                if (toggleManual.has_value())
                {
                    params.insert("toggle_manual", toggleManual.value() != 0.0);
                }
                auto persist = ReadOptionalDouble("persist (1/0)");
                if (persist.has_value())
                {
                    params.insert("persist", persist.value() != 0.0);
                }
                auto fallback = ReadOptionalDouble("fallback_sleep_s");
                if (fallback.has_value())
                {
                    params.insert("fallback_sleep_s", fallback.value());
                }

                ok = client.AutoFocusOnce(params, data, err);
                PrintResponse("autofocus_once", ok, data, err);
                break;
            }
            case 12:
            {
                std::cout << "step: ";
                std::string stepStr;
                std::getline(std::cin, stepStr);
                double step = std::stod(stepStr);
                auto speed = ReadOptionalDouble("speed");

                QJsonObject params;
                auto zoomWait = ReadOptionalDouble("zoom_wait (1/0)");
                if (zoomWait.has_value())
                {
                    params.insert("zoom_wait", zoomWait.value() != 0.0);
                }
                auto zoomTimeout = ReadOptionalDouble("zoom_timeout_s");
                if (zoomTimeout.has_value())
                {
                    params.insert("zoom_timeout_s", zoomTimeout.value());
                }
                auto afWait = ReadOptionalDouble("af_wait (1/0)");
                if (afWait.has_value())
                {
                    params.insert("af_wait", afWait.value() != 0.0);
                }
                auto afTimeout = ReadOptionalDouble("af_timeout_s");
                if (afTimeout.has_value())
                {
                    params.insert("af_timeout_s", afTimeout.value());
                }
                auto afRestore = ReadOptionalDouble("af_restore (1/0)");
                if (afRestore.has_value())
                {
                    params.insert("af_restore", afRestore.value() != 0.0);
                }

                ok = client.ZoomStepAutoFocus(step, speed, params, data, err);
                PrintResponse("zoom_step_af", ok, data, err);
                break;
            }
            case 99:
            {
                server.Stop();
                std::cout << "Shutdown requested" << std::endl;
                break;
            }
            default:
                std::cout << "Unknown command" << std::endl;
                break;
        }
    }

    server.Stop();
    return 0;
}

