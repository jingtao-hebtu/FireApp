/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : BmsWorker.h
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#ifndef FIREAPP_BMSWORKER_H
#define FIREAPP_BMSWORKER_H

#include "BmsData.h"
#include <QSerialPort>


namespace TF {
    class BmsWorker : public QObject
    {
        Q_OBJECT

    public:
        explicit BmsWorker(QObject* parent = nullptr);
        ~BmsWorker() override;

    signals:
        void statusUpdated(const BmsStatus& status);
        void connectionStateChanged(bool connected);
        void logMessage(const QString& msg);

    public slots :
        void onInitialize();

        void onStart();
        void onStop();

    private slots :
        void onPollTimeout();
        void onReadyRead();
        void onErrorOccurred(QSerialPort::SerialPortError error);
        void onResponseTimeout();
        void tryReconnect();

    private:
        bool openPortSafely();
        void closePortSafely(const QString& reason = QString());

        void sendReadRequest();
        void parseRxBuffer();

        QByteArray buildRead03(quint8 slave, quint16 startAddr, quint16 count) const;
        static quint16 crc16Modbus(const QByteArray& data);

        static double decodeTotalVoltageV(quint16 raw);
        static double decodeCapacityAh(quint16 raw);
        static double decodeTempC(quint16 raw);
        static double decodeCurrentA(quint16 raw);

    private:
        QString mPortName;
        int mBaudRate {9600};
        quint8 mSlaveId = 1;

        QSerialPort* mSerial = nullptr;
        QTimer* mPollTimer = nullptr;
        QTimer* mReconnectTimer = nullptr;
        QTimer* mResponseTimer = nullptr;

        QByteArray mRx;
        bool mAwaitingResponse = false;
        int mConsecutiveErrors = 0;

        const quint16 mReadStart = 18;
        const quint16 mReadCount = 21;

        int mPollIntervalMs = 4000;
        int mResponseTimeoutMs = 1200;
        int mReconnectIntervalMs = 6000;
    };
};


#endif //FIREAPP_BMSWORKER_H
