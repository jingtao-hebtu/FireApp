/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : WitImuSerial.cpp
   Author : tao.jing
   Date   : 2025.12.26
   Brief  :
**************************************************************************/
#include "WitImuSerial.h"
#include "TFMeaManager.h"
#include "TConfig.h"
#include <QtEndian>
#include <QDateTime>
#include <QMutexLocker>


constexpr int kFrameSize = 11;
constexpr quint8 kHeader = 0x55;

constexpr quint8 kTypeAcc = 0x51;
constexpr quint8 kTypeGyro = 0x52;
constexpr quint8 kTypeAngle = 0x53;
constexpr quint8 kTypeMag = 0x54;

constexpr float kAccScale = 16.0f / 32768.0f;
constexpr float kGyroScale = 2000.0f / 32768.0f;
constexpr float kAngleScale = 180.0f / 32768.0f;

inline quint8 u8(char c) {
    return static_cast<quint8>(static_cast<unsigned char>(c));
}

Q_DECLARE_METATYPE(WitImuData);


TF::WitImuSerial::WitImuSerial(QObject* parent)
    : QObject(parent) {
    qRegisterMetaType<WitImuData>("WitImuData");

    mSerial = new QSerialPort(this);

    connect(mSerial, &QSerialPort::readyRead, this, &WitImuSerial::onReadyRead);
    connect(mSerial, &QSerialPort::errorOccurred, this, &WitImuSerial::onPortError);
}

TF::WitImuSerial::~WitImuSerial() {
    close();
}

bool TF::WitImuSerial::open() {
    const QString& portName = GET_STR_CONFIG("WitImu", "Port").c_str();
    const qint32 baudRate = GET_INT_CONFIG("WitImu", "Baud");

    if (mSerial->isOpen()) close();

    mSerial->setPortName(portName);
    mSerial->setBaudRate(baudRate);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    if (!mSerial->open(QIODevice::ReadOnly)) {
        emit serialError(QStringLiteral("Open serial failed: %1").arg(mSerial->errorString()));
        return false;
    }

    mRecvArray.clear();

    {
        QMutexLocker locker(&mMtx);
        mData = WitImuData{};
    }

    return true;
}

void TF::WitImuSerial::close() {
    if (mSerial->isOpen())
        mSerial->close();

    mRecvArray.clear();
}

bool TF::WitImuSerial::isOpen() const {
    return mSerial->isOpen();
}

WitImuData TF::WitImuSerial::latest() const {
    QMutexLocker locker(&mMtx);
    return mData;
}

void TF::WitImuSerial::onRequestWitImuOpen() {
    open();
}

void TF::WitImuSerial::onRequestWitImuClose() {
    close();
}

void TF::WitImuSerial::onReadyRead() {
    mRecvArray.append(mSerial->readAll());
    parseRxBuffer();

    if (mRecvArray.size() > 8192)
        mRecvArray.remove(0, mRecvArray.size() - 2048);
}

void TF::WitImuSerial::onPortError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) return;
    emit serialError(mSerial->errorString());
}

bool TF::WitImuSerial::checkFrame11(const QByteArray& frame) {
    if (frame.size() != kFrameSize) return false;
    if (u8(frame[0]) != kHeader) return false;

    quint8 sum = 0;
    for (int i = 0; i < 10; ++i)
        sum = static_cast<quint8>(sum + u8(frame[i]));

    return sum == u8(frame[10]);
}

void TF::WitImuSerial::handleFrame11(const QByteArray& frame) {
    const quint8 type = u8(frame[1]);

    const uchar* p = reinterpret_cast<const uchar*>(frame.constData() + 2);
    const qint16 v0 = qFromLittleEndian<qint16>(p + 0);
    const qint16 v1 = qFromLittleEndian<qint16>(p + 2);
    const qint16 v2 = qFromLittleEndian<qint16>(p + 4);

    WitImuData snapshot;

    {
        QMutexLocker locker(&mMtx);

        mData.timestampMs = QDateTime::currentMSecsSinceEpoch();

        switch (type) {
        case kTypeAcc:
            mData.acc.x = static_cast<float>(v0) * kAccScale;
            mData.acc.y = static_cast<float>(v1) * kAccScale;
            mData.acc.z = static_cast<float>(v2) * kAccScale;
            mData.accValid = true;
            break;

        case kTypeGyro:
            mData.gyro.x = static_cast<float>(v0) * kGyroScale;
            mData.gyro.y = static_cast<float>(v1) * kGyroScale;
            mData.gyro.z = static_cast<float>(v2) * kGyroScale;
            mData.gyroValid = true;
            break;

        case kTypeAngle:
            mData.angle.x = static_cast<float>(v0) * kAngleScale;
            mData.angle.y = static_cast<float>(v1) * kAngleScale;
            mData.angle.z = static_cast<float>(v2) * kAngleScale;
            mData.angleValid = true;
            break;

        case kTypeMag:
            mData.mag.x = static_cast<float>(v0);
            mData.mag.y = static_cast<float>(v1);
            mData.mag.z = static_cast<float>(v2);
            mData.magValid = true;
            break;

        default:
            break;
        }

        snapshot = mData;
    }

    //emit dataUpdated(snapshot);
    TFMeaManager::instance().updateWitImuData(snapshot);
}

void TF::WitImuSerial::parseRxBuffer() {
    while (true) {
        const int idx = mRecvArray.indexOf(char(kHeader));
        if (idx < 0) {
            mRecvArray.clear();
            return;
        }
        if (idx > 0)
            mRecvArray.remove(0, idx);

        if (mRecvArray.size() < kFrameSize)
            return;

        const QByteArray frame = mRecvArray.left(kFrameSize);
        if (!checkFrame11(frame)) {
            mRecvArray.remove(0, 1);
            continue;
        }

        handleFrame11(frame);
        mRecvArray.remove(0, kFrameSize);
    }
}
