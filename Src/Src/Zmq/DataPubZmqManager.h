/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DataPubZmqManager.h
   Author : tao.jing
   Date   : 2026.03.01
   Brief  : 火焰检测结果ZMQ发布管理
**************************************************************************/
#ifndef FIREAPP_DATAPUBZMQMANAGER_H
#define FIREAPP_DATAPUBZMQMANAGER_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <zmq.h>
#include <zmq.hpp>
#include <TSingleton.h>


namespace TF {

    static constexpr int kFlameResultPathLen = 256;

    // 火焰检测结果发布结构体，可扩展其他字段
    #pragma pack(push, 1)
    struct FlameDetectResult {
        char detImagePath[kFlameResultPathLen]{};     // 检测后图像路径
        char oriImagePath[kFlameResultPathLen]{};     // 检测前(原始)图像路径
        char irImagePath[kFlameResultPathLen]{};      // 红外图像路径
        float fireHeight{0.0f};                       // 火焰高度(像素)
        float fireArea{0.0f};                         // 火焰面积(像素)
        int64_t timestampMs{0};                       // 时间戳(毫秒)
    };
    #pragma pack(pop)

    class DataPubZmqManager : public TBase::TSingleton<DataPubZmqManager>
    {
    public:
        ~DataPubZmqManager();

        bool init();

        void shutdown();

        void publishResult(const FlameDetectResult &result);

        void publishResult(const std::string &detImagePath,
                           const std::string &oriImagePath,
                           const std::string &irImagePath,
                           float fireHeight,
                           float fireArea);

    private:
        friend class TBase::TSingleton<DataPubZmqManager>;
        DataPubZmqManager();

        void publishThreadFunc();

        static void safeStrCopy(char *dst, std::size_t dstSize, const std::string &src);

    private:
        std::unique_ptr<zmq::context_t> mContext;
        std::unique_ptr<zmq::socket_t>  mSocket;

        int mPubPort {25555};
        std::string mPubTopic {"FlameResult"};

        std::thread              mPubThread;
        std::mutex               mMutex;
        std::condition_variable  mCond;
        std::queue<FlameDetectResult> mQueue;
        std::atomic<bool>        mRunning{false};
    };

};


#endif //FIREAPP_DATAPUBZMQMANAGER_H
