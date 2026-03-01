/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DataPubZmqManager.cpp
   Author : tao.jing
   Date   : 2026.03.01
   Brief  : 火焰检测结果ZMQ发布管理
**************************************************************************/
#include "DataPubZmqManager.h"

#include <chrono>
#include <cstring>

#include "TLog.h"


namespace {
    constexpr int  kPubPort  = 25555;
    constexpr char kPubTopic[] = "FlameResult";
}


namespace TF {

    DataPubZmqManager::DataPubZmqManager() = default;

    DataPubZmqManager::~DataPubZmqManager() {
        shutdown();
    }

    bool DataPubZmqManager::init() {
        if (mRunning.load()) {
            return true;
        }

        try {
            mContext = std::make_unique<zmq::context_t>(1);
            mSocket = std::make_unique<zmq::socket_t>(*mContext, zmq::socket_type::pub);
            mSocket->set(zmq::sockopt::sndhwm, 10);
            mSocket->set(zmq::sockopt::linger, 0);

            std::string endpoint = "tcp://*:" + std::to_string(kPubPort);
            mSocket->bind(endpoint);

            LOG_F(INFO, "DataPubZmqManager bound to %s", endpoint.c_str());
        } catch (const zmq::error_t &e) {
            LOG_F(ERROR, "DataPubZmqManager init failed: %s", e.what());
            mSocket.reset();
            mContext.reset();
            return false;
        }

        mRunning.store(true);
        mPubThread = std::thread(&DataPubZmqManager::publishThreadFunc, this);

        LOG_F(INFO, "DataPubZmqManager initialized successfully");
        return true;
    }

    void DataPubZmqManager::shutdown() {
        if (!mRunning.exchange(false)) {
            return;
        }

        mCond.notify_all();

        if (mPubThread.joinable()) {
            mPubThread.join();
        }

        if (mSocket) {
            try { mSocket->close(); } catch (...) {}
            mSocket.reset();
        }

        mContext.reset();

        LOG_F(INFO, "DataPubZmqManager shutdown");
    }

    void DataPubZmqManager::publishResult(const FlameDetectResult &result) {
        if (!mRunning.load()) {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.push(result);
        }
        mCond.notify_one();
    }

    void DataPubZmqManager::publishResult(const std::string &detImagePath,
                                           const std::string &oriImagePath,
                                           const std::string &irImagePath,
                                           float fireHeight,
                                           float fireArea) {
        FlameDetectResult result{};
        safeStrCopy(result.detImagePath, sizeof(result.detImagePath), detImagePath);
        safeStrCopy(result.oriImagePath, sizeof(result.oriImagePath), oriImagePath);
        safeStrCopy(result.irImagePath,  sizeof(result.irImagePath),  irImagePath);
        result.fireHeight  = fireHeight;
        result.fireArea    = fireArea;

        auto now = std::chrono::system_clock::now();
        result.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();

        publishResult(result);
    }

    void DataPubZmqManager::safeStrCopy(char *dst, std::size_t dstSize, const std::string &src) {
        std::size_t len = std::min(src.size(), dstSize - 1);
        std::memcpy(dst, src.data(), len);
        dst[len] = '\0';
    }

    void DataPubZmqManager::publishThreadFunc() {
        LOG_F(INFO, "DataPubZmqManager publish thread started");

        while (mRunning.load()) {
            FlameDetectResult result{};
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mCond.wait(lock, [this] {
                    return !mQueue.empty() || !mRunning.load();
                });

                if (!mRunning.load() && mQueue.empty()) {
                    break;
                }

                if (mQueue.empty()) {
                    continue;
                }

                result = mQueue.front();
                mQueue.pop();
            }

            try {
                // 发送topic帧
                zmq::message_t topicMsg(kPubTopic, std::strlen(kPubTopic));
                mSocket->send(topicMsg, zmq::send_flags::sndmore);

                // 发送数据帧
                zmq::message_t dataMsg(&result, sizeof(FlameDetectResult));
                mSocket->send(dataMsg, zmq::send_flags::none);
            } catch (const zmq::error_t &e) {
                LOG_F(ERROR, "DataPubZmqManager publish failed: %s", e.what());
            }
        }

        LOG_F(INFO, "DataPubZmqManager publish thread stopped");
    }

}
