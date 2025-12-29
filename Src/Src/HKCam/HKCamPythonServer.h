#pragma once

#include <atomic>
#include <string>

#include "HKCamZmqClient.h"

namespace TF
{
struct HKCamServerConfig
{
    std::string pythonExe;       // python executable path
    std::string scriptPath;      // onvif_rpc_server.py path
    std::string cameraIp;
    int cameraPort = 80;
    std::string username;
    std::string password;
    int profileIndex = 0;
    std::string endpoint = "tcp://127.0.0.1:5555";
    int timeoutMs = 3000;
    int startRetries = 30;
    int startPollIntervalMs = 200;
    bool createNoWindow = true;
    bool allowStopExternal = false;
};

class HKCamPythonServer
{
public:
    HKCamPythonServer();
    ~HKCamPythonServer();

    bool StartBlocking(const HKCamServerConfig &cfg, std::string &outError);
    void Stop();
    bool IsRunning() const;

private:
    bool CheckExistingServer(std::string &outError);
    bool LaunchProcess(std::string &outError);
    bool WaitForReady(std::string &outError);

private:
    HKCamServerConfig m_config;
    std::atomic<bool> m_running{false};
    bool m_external = false;
    std::string m_lastCommandLine;

#ifdef _WIN32
    void *m_jobHandle = nullptr;
    void *m_processHandle = nullptr;
    void *m_threadHandle = nullptr;
#else
    pid_t m_childPid = -1;
    pid_t m_childPgid = -1;
#endif
};
}

