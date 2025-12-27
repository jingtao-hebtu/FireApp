/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FILE_NAME
   Author : tao.jing
   Date   : 2025.12.24
   Brief  :
**************************************************************************/
#include "TFDistClient.h"
#include "TFMeaManager.h"
#include "TConfig.h"
#include "TFException.h"
#include <QTimer>
#include <QDebug>


TF::DistRecvWorker::DistRecvWorker(QObject* parent) : QObject(parent) {}

TF::DistRecvWorker::~DistRecvWorker() {
    closePort();
}

bool TF::DistRecvWorker::openPort(const TF::DistSettings& s) {
    if (mPort == nullptr) {
        mPort = new QSerialPort(this);
    }

    if (mPort->isOpen()) mPort->close();

    mPort->setPortName(s.portName);
    mPort->setBaudRate(s.baudRate);
    mPort->setDataBits(s.dataBits);
    mPort->setParity(s.parity);
    mPort->setStopBits(s.stopBits);
    mPort->setFlowControl(s.flowControl);

    if (!mPort->open(QIODevice::ReadWrite)) {
        return false;
    }

    connect(mPort, &QSerialPort::readyRead, this,
        &DistRecvWorker::onReadyRead, Qt::UniqueConnection);

    mPort->clear(QSerialPort::AllDirections);
    mPort->readAll();
    return true;
}

void TF::DistRecvWorker::closePort() {
    if (mPort->isOpen()) {
        disconnect(mPort, nullptr, this, nullptr);
        mPort->close();
    }
}

qint64 TF::DistRecvWorker::sendRaw(const QByteArray& bytes) {
    if (!mPort->isOpen()) return -1;
    return mPort->write(bytes);
}

void TF::DistRecvWorker::onReadyRead() {
    QByteArray data = mPort->readAll();
    if (!data.isEmpty())
        emit bytesArrived(data);
}


TF::TFDistClient::TFDistClient(QObject* parent) : QObject(parent) {
}

TF::TFDistClient::~TFDistClient() {
    close();
}

bool TF::TFDistClient::open() {
    if (mRunning.load()) return true;

    mMode = GET_STR_CONFIG("Distance", "Mode");

    mFilter.reset();

    DistSettings s;
    s.portName = GET_STR_CONFIG("Distance", "Port").c_str();
    s.baudRate = GET_INT_CONFIG("Distance", "Baud");;

    // 1) Start receiving thread and worker
    mWorker = new DistRecvWorker();
    mWorker->moveToThread(&mRecvThread);
    connect(&mRecvThread, &QThread::finished,
        mWorker, &QObject::deleteLater);

    connect(mWorker, &DistRecvWorker::bytesArrived,
        this, &TFDistClient::onBytesArrived, Qt::QueuedConnection);

    mRecvThread.start();

    bool okOpen = false;
    QMetaObject::invokeMethod(
        mWorker,
        [&] { okOpen = mWorker->openPort(s); },
        Qt::BlockingQueuedConnection
    );
    if (!okOpen) {
        mRecvThread.quit();
        mRecvThread.wait();
        mWorker = nullptr;
        TF_LOG_THROW_PROMPT("距离传感器初始化失败，请检查连接.");
        return false;
    }

    // 2) Send start and continuous measurement cmd
    if (mMode == "Continuous") {
        const QByteArray openCmd = cmdOpenContinuous();
        qint64 nWrite = -1;
        QMetaObject::invokeMethod(
            mWorker,
            [&] { nWrite = mWorker->sendRaw(openCmd); },
            Qt::BlockingQueuedConnection
        );
        if (nWrite != openCmd.size()) {
            close();
            return false;
        }
    }

    // 3) Start parsing thread
    mRunning.store(true);
    mParseThread = std::thread(&TFDistClient::parseLoop, this);

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this] {
        this->triggerOnceReadDistance();
    });
    timer->start(1000);

    return true;
}

void TF::TFDistClient::close() {
    const bool wasRunning = mRunning.exchange(false);

    // 1) Send close cmd
    if (mWorker) {
        const QByteArray closeCmd = cmdClose();
        qint64 nWrite = -1;
        QMetaObject::invokeMethod(
            mWorker,
            [&] { nWrite = mWorker->sendRaw(closeCmd); },
            Qt::BlockingQueuedConnection
        );
        (void)nWrite;
    }

    // 2) Stop parsing thread
    if (wasRunning) {
        mCv.notify_all();
        if (mParseThread.joinable())
            mParseThread.join();
    }

    // 3) Clear data queue
    {
        std::lock_guard<std::mutex> lk(mMtx);
        mQueue.clear();
    }

    // 4) Close port + Stop receiving thread
    if (mWorker) {
        QMetaObject::invokeMethod(mWorker, &DistRecvWorker::closePort, Qt::BlockingQueuedConnection);
        mRecvThread.quit();
        mRecvThread.wait();
        mWorker = nullptr;
    }

    mFilter.reset();
}

void TF::TFDistClient::onBytesArrived(const QByteArray& bytes) {
    {
        std::lock_guard<std::mutex> lk(mMtx);
        for (auto ch : bytes) {
            mQueue.push_back(static_cast<uint8_t>(ch));
        }
    }
    mCv.notify_one();
}

void TF::TFDistClient::parseLoop() {
while (mRunning.load()) {
        std::unique_lock<std::mutex> lk(mMtx);
        mCv.wait(lk, [&] { return !mRunning.load() || !mQueue.empty(); });
        if (!mRunning.load()) break;

        // Header is longer than 3
        if (mQueue.size() < 3) continue;

        auto findHeader = [&]() -> int {
            const int n = static_cast<int>(mQueue.size());
            for (int i = 0; i <= n - 3; ++i) {
                if (mQueue[i] == H0 && mQueue[i + 1] == H1 && mQueue[i + 2] == H2)
                    return i;
            }
            return -1;
        };

        int idx = findHeader();
        if (idx < 0) {
            // No header found, keep the last 2 bytes
            while (mQueue.size() > 2) mQueue.pop_front();
            continue;
        }

        // Discard the noisy headers
        for (int i = 0; i < idx; ++i) mQueue.pop_front();

        // Got 01 03 04, but may not enough 9 bytes
        if (mQueue.size() < 9) {
            // Wait for more data
            mCv.wait_for(lk, std::chrono::milliseconds(50), [&] { return !mRunning.load() || mQueue.size() >= 9; });
            continue;
        }

        uint8_t frame[9];
        for (int i = 0; i < 9; ++i) frame[i] = mQueue[i];

        if (!checkFrameCrc9(frame)) {
            // CRC check failed
            for (int i = 0; i < 3 && !mQueue.empty(); ++i) mQueue.pop_front();
            continue;
        }

        // CRC passed, pop this frame
        for (int i = 0; i < 9; ++i) mQueue.pop_front();
        lk.unlock();

        // Parse data: 01 03 04 [D0 D1 D2 D3] CRC_L CRC_H
        const uint32_t raw =
            (static_cast<uint32_t>(frame[3]) << 24) |
            (static_cast<uint32_t>(frame[4]) << 16) |
            (static_cast<uint32_t>(frame[5]) << 8)  |
            (static_cast<uint32_t>(frame[6]) << 0);

        const float meters = static_cast<float>(raw) / 10000.0;

        if (auto filtered = mFilter.update(meters)) {
            //qDebug() << QString("LDS distance = %1 m (raw=%2)").arg(meters, 0, 'f', 4).arg(raw);
            //emit distanceUpdated(*filtered);
            TFMeaManager::instance().updateCurDist(meters);
        } else {
            // Discard this sample
        }
    }
}

uint16_t TF::TFDistClient::crc16Rtu(const uint8_t* data, size_t len) {
    // RTU CRC16：0xA001, initial value 0xFFFF
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool TF::TFDistClient::checkFrameCrc9(const uint8_t frame[9]) {
    // frame[0..6] CRC, frame[7]=CRC_L, frame[8]=CRC_H
    const uint16_t crcCalc = crc16Rtu(frame, 7);
    const uint16_t crcRecv = static_cast<uint16_t>(frame[7]) | (static_cast<uint16_t>(frame[8]) << 8);
    return crcCalc == crcRecv;
}

QByteArray TF::TFDistClient::cmdOpenContinuous() {
    // 01 06 00 11 00 02 58 0E
    return QByteArray::fromHex("010600110002580E");
}

QByteArray TF::TFDistClient::cmdReadDistanceOnce() {
    // 01 03 00 15 00 02 D5 CF
    return QByteArray::fromHex("010300150002D5CF");
}

QByteArray TF::TFDistClient::cmdClose() {
    // 01 06 00 11 00 00 D9 CF
    return QByteArray::fromHex("010600110000D9CF");
}

bool TF::TFDistClient::triggerOnceReadDistance()
{
    if (!mWorker) return false;

    const QByteArray readCmd = cmdReadDistanceOnce();

    qint64 nWrite = -1;
    QMetaObject::invokeMethod(
        mWorker,
        [&] { nWrite = mWorker->sendRaw(readCmd); },
        Qt::BlockingQueuedConnection
    );

    return (nWrite == readCmd.size());
}