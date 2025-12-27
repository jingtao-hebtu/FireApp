/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : WitImuSerial.h
   Author : tao.jing
   Date   : 2025.12.26
   Brief  :
**************************************************************************/
#ifndef FIREAPP_WITIMUSERIAL_H
#define FIREAPP_WITIMUSERIAL_H

#include "WitImuData.h"
#include <QSerialPort>
#include <QByteArray>
#include <QMutex>


namespace TF {
    class WitImuSerial final : public QObject
    {
        Q_OBJECT

    public:
        explicit WitImuSerial(QObject* parent = nullptr);
        ~WitImuSerial() override;

        bool open();
        void close();
        bool isOpen() const;

        // Get latest data
        WitImuData latest() const;

    signals:
        void dataUpdated(const WitImuData& data);

        void serialError(const QString& errorText);

    public slots:
        void onRequestWitImuOpen();
        void onRequestWitImuClose();

    private slots:
        void onReadyRead();
        void onPortError(QSerialPort::SerialPortError error);

    private:
        void parseRxBuffer();
        static bool checkFrame11(const QByteArray& frame);
        void handleFrame11(const QByteArray& frame);

    private:
        mutable QMutex mMtx;
        QSerialPort *mSerial {nullptr};
        QByteArray mRecvArray;

        WitImuData mData;
    };
};


#endif //FIREAPP_WITIMUSERIAL_H
