#include "HKCamPythonServer.h"

#include <chrono>
#include <sstream>
#include <thread>
#include <vector>

#include <QJsonObject>

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
    std::wstring Utf8ToWide(const std::string &input)
    {
        if (input.empty())
        {
            return std::wstring();
        }
        const int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), static_cast<int>(input.size()), nullptr, 0);
        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, input.c_str(), static_cast<int>(input.size()), result.data(), sizeNeeded);
        return result;
    }

    std::string GetLastErrorMessage(DWORD code)
    {
        LPWSTR buffer = nullptr;
        const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        const auto length = FormatMessageW(flags, nullptr, code, 0, reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
        std::string result;
        if (length && buffer)
        {
            int required = WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
            result.resize(required);
            WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(length), result.data(), required, nullptr, nullptr);
        }
        if (buffer)
        {
            LocalFree(buffer);
        }
        return result;
    }

    std::string JoinArgs(const std::vector<std::string> &args)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (i)
            {
                oss << ' ';
            }
            oss << '"' << args[i] << '"';
        }
        return oss.str();
    }
#else
    std::string JoinArgs(const std::vector<std::string> &args)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (i)
            {
                oss << ' ';
            }
            oss << args[i];
        }
        return oss.str();
    }
#endif
}

namespace TF
{
HKCamPythonServer::HKCamPythonServer() = default;

HKCamPythonServer::~HKCamPythonServer()
{
    try
    {
        Stop();
    }
    catch (...)
    {
    }
}

bool HKCamPythonServer::IsRunning() const
{
    return m_running.load();
}

bool HKCamPythonServer::StartBlocking(const HKCamServerConfig &cfg, std::string &outError)
{
    Stop();
    m_config = cfg;
    m_external = false;
    outError.clear();

    if (CheckExistingServer(outError))
    {
        m_running.store(true);
        m_external = true;
        return true;
    }

    if (!outError.empty())
    {
        outError.clear();
    }

    if (!LaunchProcess(outError))
    {
        Stop();
        return false;
    }

    m_running.store(true);

    if (!WaitForReady(outError))
    {
        Stop();
        return false;
    }

    return true;
}

bool HKCamPythonServer::CheckExistingServer(std::string &outError)
{
    HKCamZmqClient client;
    client.Configure(m_config.endpoint, m_config.timeoutMs);

    TF::RpcResponse resp;
    if (client.Ping(resp, outError))
    {
        return true;
    }
    return false;
}

bool HKCamPythonServer::WaitForReady(std::string &outError)
{
    HKCamZmqClient client;
    client.Configure(m_config.endpoint, m_config.timeoutMs);
    TF::RpcResponse resp;

    std::string lastError;
    for (int i = 0; i < m_config.startRetries; ++i)
    {
        if (client.Ping(resp, lastError))
        {
            outError.clear();
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.startPollIntervalMs));
    }

    std::ostringstream oss;
    oss << "Server not ready. cmd=" << m_lastCommandLine
        << " endpoint=" << m_config.endpoint
        << " last_error=" << lastError;
    outError = oss.str();
    return false;
}

bool HKCamPythonServer::LaunchProcess(std::string &outError)
{
    std::vector<std::string> args = {
        m_config.pythonExe,
        m_config.scriptPath,
        "--ip",
        m_config.cameraIp,
        "--port",
        std::to_string(m_config.cameraPort),
        "--user",
        m_config.username,
        "--password",
        m_config.password,
        "--profile-index",
        std::to_string(m_config.profileIndex),
        "--bind",
        m_config.endpoint};

    const auto commandLine = JoinArgs(args);
    m_lastCommandLine = commandLine;

#ifdef _WIN32
    std::wstring wideCmd = Utf8ToWide(commandLine);
    std::wstring wideExe = Utf8ToWide(m_config.pythonExe);
    std::vector<wchar_t> cmdBuffer(wideCmd.begin(), wideCmd.end());
    cmdBuffer.push_back(L'\0');

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (m_config.createNoWindow)
    {
        si.dwFlags |= STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    DWORD creationFlags = 0;
    if (m_config.createNoWindow)
    {
        creationFlags |= CREATE_NO_WINDOW;
    }

    const BOOL created = CreateProcessW(wideExe.c_str(), cmdBuffer.data(), nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr,
                                        &si, &pi);
    if (!created)
    {
        const DWORD errCode = GetLastError();
        std::ostringstream oss;
        oss << "CreateProcess failed. cmd=" << commandLine << " endpoint=" << m_config.endpoint
            << " error_code=" << errCode << " message=" << GetLastErrorMessage(errCode);
        outError = oss.str();
        return false;
    }

    HANDLE hJob = CreateJobObjectW(nullptr, nullptr);
    if (hJob)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
        AssignProcessToJobObject(hJob, pi.hProcess);
    }

    m_jobHandle = hJob;
    m_processHandle = pi.hProcess;
    m_threadHandle = pi.hThread;
#else
    pid_t child = fork();
    if (child < 0)
    {
        std::ostringstream oss;
        oss << "fork failed. cmd=" << commandLine << " endpoint=" << m_config.endpoint
            << " errno=" << errno << " message=" << strerror(errno);
        outError = oss.str();
        return false;
    }

    if (child == 0)
    {
        setpgid(0, 0);
        prctl(PR_SET_PDEATHSIG, SIGTERM);

        std::vector<char *> argv;
        argv.reserve(args.size() + 1);
        for (const auto &arg : args)
        {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(nullptr);

        if (m_config.pythonExe.find('/') != std::string::npos)
        {
            execv(m_config.pythonExe.c_str(), argv.data());
        }
        else
        {
            execvp(m_config.pythonExe.c_str(), argv.data());
        }
        _exit(127);
    }

    m_childPid = child;
    m_childPgid = child;
#endif

    return true;
}

void HKCamPythonServer::Stop()
{
    if (!m_running.load())
    {
        return;
    }

    if (m_external && !m_config.allowStopExternal)
    {
        m_running.store(false);
        return;
    }

    HKCamZmqClient client;
    client.Configure(m_config.endpoint, m_config.timeoutMs);
    QJsonObject shutdownData;
    std::string shutdownErr;
    client.Shutdown(shutdownData, shutdownErr);

#ifdef _WIN32
    if (m_processHandle)
    {
        const DWORD waitRes = WaitForSingleObject(static_cast<HANDLE>(m_processHandle), m_config.timeoutMs);
        if (waitRes == WAIT_TIMEOUT)
        {
            TerminateProcess(static_cast<HANDLE>(m_processHandle), 1);
            WaitForSingleObject(static_cast<HANDLE>(m_processHandle), INFINITE);
        }
    }

    if (m_threadHandle)
    {
        CloseHandle(static_cast<HANDLE>(m_threadHandle));
        m_threadHandle = nullptr;
    }
    if (m_processHandle)
    {
        CloseHandle(static_cast<HANDLE>(m_processHandle));
        m_processHandle = nullptr;
    }
    if (m_jobHandle)
    {
        CloseHandle(static_cast<HANDLE>(m_jobHandle));
        m_jobHandle = nullptr;
    }
#else
    if (m_childPid > 0)
    {
        int status = 0;
        const auto start = std::chrono::steady_clock::now();
        while (true)
        {
            const pid_t res = waitpid(m_childPid, &status, WNOHANG);
            if (res == m_childPid)
            {
                break;
            }

            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
            if (elapsed.count() > m_config.timeoutMs)
            {
                if (m_childPgid > 0)
                {
                    killpg(m_childPgid, SIGTERM);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    killpg(m_childPgid, SIGKILL);
                }
                waitpid(m_childPid, &status, 0);
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    m_childPid = -1;
    m_childPgid = -1;
#endif

    m_running.store(false);
}
}

