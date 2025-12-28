/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : BmsWorker.cpp
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#include "BmsWorker.h"
#include "TConfig.h"
#include <QSerialPortInfo>
#include <QVector>
#include <QTimer>


static inline quint8 u8(char c) { return static_cast<quint8>(static_cast<unsigned char>(c)); }


TF::BmsWorker::BmsWorker(QObject* parent) : QObject(parent) {
}

TF::BmsWorker::~BmsWorker() {
    onStop();
}

void TF::BmsWorker::onInitialize() {
    // Execute in the worker thread: create the serial port and timer,
    //  and ensure the thread affinity is correctly configured.
    mPortName = GET_STR_CONFIG("Battery", "PortName").c_str();
    mBaudRate = GET_INT_CONFIG("Battery", "Baud");
    mSlaveId = GET_INT_CONFIG("Battery", "SlaveId");

    if (!mSerial) {
        mSerial = new QSerialPort(this);
        mSerial->setReadBufferSize(512);

        connect(mSerial, &QSerialPort::readyRead,
                this, &BmsWorker::onReadyRead);
        connect(mSerial, &QSerialPort::errorOccurred,
                this, &BmsWorker::onErrorOccurred);
    }

    if (!mPollTimer) {
        mPollTimer = new QTimer(this);
        mPollTimer->setInterval(mPollIntervalMs);
        connect(mPollTimer, &QTimer::timeout, this, &BmsWorker::onPollTimeout);
    }

    if (!mResponseTimer) {
        mResponseTimer = new QTimer(this);
        mResponseTimer->setSingleShot(true);
        mResponseTimer->setInterval(mResponseTimeoutMs);
        connect(mResponseTimer, &QTimer::timeout, this, &BmsWorker::onResponseTimeout);
    }

    if (!mReconnectTimer) {
        mReconnectTimer = new QTimer(this);
        mReconnectTimer->setInterval(mReconnectIntervalMs);
        connect(mReconnectTimer, &QTimer::timeout, this, &BmsWorker::tryReconnect);
    }

    // After initialization is complete,
    //  attempt to open and start the process.
    onStart();
}

void TF::BmsWorker::onStart() {
    // Non-blocking mode: initiate the reconnection timer in case of opening failure
    if (openPortSafely()) {
        if (mReconnectTimer) mReconnectTimer->stop();
        if (mPollTimer && !mPollTimer->isActive())
            mPollTimer->start();

        // Trigger a read operation immediately to
        //  enable the interface to display data faster.
        onPollTimeout();
    }
    else {
        if (mPollTimer) mPollTimer->stop();
        if (mReconnectTimer && !mReconnectTimer->isActive())
            mReconnectTimer->start();
    }
}

void TF::BmsWorker::onStop() {
    if (mPollTimer) mPollTimer->stop();
    if (mReconnectTimer) mReconnectTimer->stop();
    if (mResponseTimer) mResponseTimer->stop();

    closePortSafely(QStringLiteral("stop() called"));
}

bool TF::BmsWorker::openPortSafely() {
    if (!mSerial) return false;
    if (mPortName.trimmed().isEmpty()) {
        emit logMessage(QStringLiteral("串口名为空，无法打开"));
        return false;
    }

    if (mSerial->isOpen()) {
        return true;
    }

    // First check the system's available serial port list
    //  to avoid meaningless open
    bool exists = false;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& pi : ports) {
        if (pi.portName() == mPortName) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        emit logMessage(QString("串口 %1 不在可用列表中，等待重连…").arg(mPortName));
        emit connectionStateChanged(false);
        return false;
    }

    mSerial->setPortName(mPortName);
    mSerial->setBaudRate(9600);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    if (!mSerial->open(QIODevice::ReadWrite)) {
        emit logMessage(QString("打开串口失败 %1: %2").arg(mPortName, mSerial->errorString()));
        emit connectionStateChanged(false);
        return false;
    }

    mSerial->clear(QSerialPort::AllDirections);
    mRx.clear();
    mAwaitingResponse = false;
    mConsecutiveErrors = 0;

    emit logMessage(QString("串口已打开: %1, slave=%2").arg(mPortName).arg(mSlaveId));
    emit connectionStateChanged(true);
    return true;
}

void TF::BmsWorker::closePortSafely(const QString& reason) {
    if (!mSerial) return;

    if (mSerial->isOpen()) {
        mSerial->clear(QSerialPort::AllDirections);
        mSerial->close();
        emit logMessage(QString("串口已关闭: %1 (%2)").arg(mPortName, reason));
    }

    mRx.clear();
    mAwaitingResponse = false;

    emit connectionStateChanged(false);
}

void TF::BmsWorker::onPollTimeout() {
    sendReadRequest();
}

void TF::BmsWorker::sendReadRequest() {
    if (!mSerial || !mSerial->isOpen()) {
        // Port not opened: start reconnection.
        if (mPollTimer) mPollTimer->stop();
        if (mReconnectTimer && !mReconnectTimer->isActive())
            mReconnectTimer->start();
        return;
    }

    if (mAwaitingResponse) {
        // Prev frame pending: do not request accumulation, skip current polling.
        return;
    }

    const QByteArray req = buildRead03(mSlaveId, mReadStart, mReadCount);

    // Parse only current request's response, reduce noise interference.
    mRx.clear();
    const qint64 n = mSerial->write(req);

    if (n != req.size()) {
        // Write fail/incomplete write: handle as critical error.
        emit logMessage(QString("串口写入失败: %1").arg(mSerial->errorString()));
        closePortSafely(QStringLiteral("write failed"));
        if (mReconnectTimer && !mReconnectTimer->isActive())
            mReconnectTimer->start();
        return;
    }

    mAwaitingResponse = true;
    if (mResponseTimer) mResponseTimer->start();
}

void TF::BmsWorker::onReadyRead() {
    if (!mSerial) return;
    mRx.append(mSerial->readAll());
    parseRxBuffer();
}

void TF::BmsWorker::parseRxBuffer() {
    // Expected:
    // Normal： [addr][0x03][byteCount][data...][crcLo][crcHi]
    // Abnormal： [addr][0x83][excCode][crcLo][crcHi]
    while (mRx.size() >= 5) {
        // 1) First align slave address.
        while (!mRx.isEmpty() && u8(mRx.at(0)) != mSlaveId) {
            mRx.remove(0, 1);
        }
        if (mRx.size() < 5) return;

        const quint8 func = u8(mRx.at(1));

        // 2) Exceptional response
        if (func == (0x03 | 0x80)) {
            if (mRx.size() < 5) return;
            const QByteArray frame = mRx.left(5);

            const quint16 crcRecv = static_cast<quint16>(u8(frame.at(3)) | (u8(frame.at(4)) << 8));
            const quint16 crcCalc = crc16Modbus(frame.left(3));
            if (crcRecv != crcCalc) {
                // CRC error: discard 1 byte and resynchronize.
                mRx.remove(0, 1);
                continue;
            }

            const quint8 exc = u8(frame.at(2));
            mRx.remove(0, 5);

            if (mResponseTimer) mResponseTimer->stop();
            mAwaitingResponse = false;

            BmsStatus st;
            st.mTimestamp = QDateTime::currentDateTime();
            st.ok = false;
            st.error = QString("从机异常响应: func=0x%1 exc=0x%2")
                       .arg(func, 2, 16, QLatin1Char('0'))
                       .arg(exc, 2, 16, QLatin1Char('0'));
            emit statusUpdated(st);

            mConsecutiveErrors++;
            return;
        }

        // 3) Normal response
        if (func == 0x03) {
            if (mRx.size() < 3) return;
            const quint8 byteCount = u8(mRx.at(2));
            const int frameLen = 3 + byteCount + 2;

            if (mRx.size() < frameLen) return; // Wait for more data

            const QByteArray frame = mRx.left(frameLen);
            const quint16 crcRecv =
                static_cast<quint16>(u8(frame.at(frameLen - 2)) | (u8(frame.at(frameLen - 1)) << 8));
            const quint16 crcCalc = crc16Modbus(frame.left(frameLen - 2));

            if (crcRecv != crcCalc) {
                // CRC error: discard 1 byte and resynchronize.
                mRx.remove(0, 1);
                mConsecutiveErrors++;
                return;
            }

            if ((byteCount % 2) != 0) {
                mRx.remove(0, frameLen);
                mConsecutiveErrors++;
                return;
            }

            const int regCount = byteCount / 2;
            QVector<quint16> regs;
            regs.reserve(regCount);

            for (int i = 0; i < regCount; ++i) {
                const int off = 3 + i * 2;
                const quint16 v = static_cast<quint16>((u8(frame.at(off)) << 8) | u8(frame.at(off + 1)));
                regs.push_back(v);
            }

            mRx.remove(0, frameLen);

            if (mResponseTimer) mResponseTimer->stop();
            mAwaitingResponse = false;
            mConsecutiveErrors = 0;

            // Address mapping: start=18
            if (regs.size() < static_cast<int>(mReadCount)) {
                // Incomplete data frame (should not happen in theory) — mark as an error.
                BmsStatus st;
                st.mTimestamp = QDateTime::currentDateTime();
                st.ok = false;
                st.error = QString("寄存器数量不足: got=%1 need=%2").arg(regs.size()).arg(mReadCount);
                emit statusUpdated(st);
                return;
            }

            const quint16 rawV = regs.at(0); // 18
            const quint16 rawI = regs.at(1); // 19
            const quint16 rawAh = regs.at(2); // 20
            const quint16 rawT1 = regs.at(37 - 18); // 37
            const quint16 rawT2 = regs.at(38 - 18); // 38

            BmsStatus st;
            st.mTimestamp = QDateTime::currentDateTime();
            st.ok = true;
            st.mTotalVoltage_V = decodeTotalVoltageV(rawV);
            st.mCurrent_A = decodeCurrentA(rawI);
            st.mRemainCapacity_Ah = decodeCapacityAh(rawAh);
            st.mTemp1_C = decodeTempC(rawT1);
            st.mTemp2_C = decodeTempC(rawT2);

            emit statusUpdated(st);
            return;
        }

        // 4) Unexpected func code: drop 1 byte, resync.
        mRx.remove(0, 1);
    }
}

void TF::BmsWorker::onResponseTimeout() {
    if (!mAwaitingResponse)
        return;

    mAwaitingResponse = false;
    mConsecutiveErrors++;

    BmsStatus st;
    st.mTimestamp = QDateTime::currentDateTime();
    st.ok = false;
    st.error = QString("等待响应超时（%1ms），连续错误=%2").arg(mResponseTimeoutMs).arg(mConsecutiveErrors);
    emit statusUpdated(st);

    // Continuous timeouts/too many errors: mark link as invalid, close and reconnect.
    if (mConsecutiveErrors >= 3) {
        emit logMessage(QStringLiteral("连续超时/错误达到阈值，触发重连"));
        closePortSafely(QStringLiteral("too many timeouts"));
        if (mPollTimer) mPollTimer->stop();
        if (mReconnectTimer && !mReconnectTimer->isActive())
            mReconnectTimer->start();
    }
}

void TF::BmsWorker::onErrorOccurred(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError)
        return;

    // 某些平台会频繁抛出非致命错误，这里做一下分级处理
    const bool fatal =
        (error == QSerialPort::ResourceError) ||
        (error == QSerialPort::DeviceNotFoundError) ||
        (error == QSerialPort::PermissionError) ||
        (error == QSerialPort::OpenError) ||
        (error == QSerialPort::NotOpenError) ||
        (error == QSerialPort::ReadError) ||
        (error == QSerialPort::WriteError);

    emit logMessage(QString("串口错误: %1 (code=%2)")
                    .arg(mSerial ? mSerial->errorString() : QStringLiteral("null"))
                    .arg(static_cast<int>(error)));

    if (fatal) {
        closePortSafely(QStringLiteral("fatal error"));
        if (mPollTimer) mPollTimer->stop();
        if (mReconnectTimer && !mReconnectTimer->isActive())
            mReconnectTimer->start();
    }
}

void TF::BmsWorker::tryReconnect() {
    if (mSerial && mSerial->isOpen()) {
        if (mReconnectTimer) mReconnectTimer->stop();
        if (mPollTimer && !mPollTimer->isActive())
            mPollTimer->start();
        return;
    }

    if (openPortSafely()) {
        if (mReconnectTimer) mReconnectTimer->stop();
        if (mPollTimer && !mPollTimer->isActive())
            mPollTimer->start();
        onPollTimeout();
    }
}

QByteArray TF::BmsWorker::buildRead03(quint8 slave, quint16 startAddr, quint16 count) const {
    QByteArray pdu;
    pdu.reserve(8);
    pdu.append(char(slave));
    pdu.append(char(0x03));
    pdu.append(char((startAddr >> 8) & 0xFF));
    pdu.append(char(startAddr & 0xFF));
    pdu.append(char((count >> 8) & 0xFF));
    pdu.append(char(count & 0xFF));

    const quint16 crc = crc16Modbus(pdu);
    pdu.append(char(crc & 0xFF)); // CRC Lo
    pdu.append(char((crc >> 8) & 0xFF)); // CRC Hi
    return pdu;
}

quint16 TF::BmsWorker::crc16Modbus(const QByteArray& data) {
    quint16 crc = 0xFFFF;
    for (char c : data) {
        crc ^= static_cast<quint16>(u8(c));
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

// -------- 换算（与说明书一致） --------
double TF::BmsWorker::decodeTotalVoltageV(quint16 raw) { return raw * 0.01; } // 10mV -> 0.01V
double TF::BmsWorker::decodeCapacityAh(quint16 raw) { return raw * 0.01; } // 10mAh -> 0.01Ah
double TF::BmsWorker::decodeTempC(quint16 raw) { return (static_cast<int>(raw) - 2731) / 10.0; }

// 电流：10mA；充电为正；放电为负；放电示例：raw = 65535 - 1000
double TF::BmsWorker::decodeCurrentA(quint16 raw) {
    if (raw <= 0x7FFF) return raw * 0.01;
    const quint16 abs10mA = static_cast<quint16>(0xFFFF - raw);
    return -(abs10mA * 0.01);
}
