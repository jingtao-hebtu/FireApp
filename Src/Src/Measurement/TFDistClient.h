/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FILE_NAME
   Author : tao.jing
   Date   : 2025年12月24日
   Brief  :
**************************************************************************/
#ifndef FIREAPP_TFDISTCLIENT_H
#define FIREAPP_TFDISTCLIENT_H

#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <deque>


namespace TF {
    typedef struct s_DistSettings
    {
        QString portName;
        int baudRate = 9600;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    } DistSettings;


    struct DistanceFilter {
        static constexpr int N = 7;      // Window size: 5/7/9
        float alpha = 0.2f;              // EMA coeff: 0.1 - 0.3
        float minValid = 1e-6f;          // Low limitation; <=0 will be discarded
        float maxValid = 120;            // Up limitation

        std::array<float, N> buf{};
        int pos = 0;
        bool inited = false;
        float ema = 0.0f;

        void reset() {
            pos = 0;
            inited = false;
            ema = 0.0f;
        }

        // Return: this sample if valid, return filtered value;
        // if this sample not valid, discard
        std::optional<float> update(float x) {
            if (!std::isfinite(x)) return std::nullopt;
            if (x <= minValid) return std::nullopt;   // meters==0 discard
            if (x > maxValid) return std::nullopt;

            if (!inited) {
                buf.fill(x);
                ema = x;
                inited = true;
                pos = 0;
                return ema;
            }

            // Write ring buffer
            buf[pos] = x;
            pos = (pos + 1) % N;

            // Copy and sort
            auto tmp = buf;
            std::sort(tmp.begin(), tmp.end());

            // Remove extreme value and calc average
            float sum = 0.0f;
            for (int i = 1; i < N - 1; ++i) sum += tmp[i];
            const float robust = sum / float(N - 2);

            // EMA smooth
            ema += alpha * (robust - ema);
            return ema;
        }
    };


    class DistRecvWorker : public QObject
    {
        Q_OBJECT

    public:
        explicit DistRecvWorker(QObject* parent = nullptr);
        ~DistRecvWorker() override;

        Q_INVOKABLE bool openPort(const TF::DistSettings &s);
        Q_INVOKABLE void closePort();

        Q_INVOKABLE qint64 sendRaw(const QByteArray &bytes);

    signals:
        void bytesArrived(const QByteArray& bytes);

    private slots:
        void onReadyRead();

    private:
        QSerialPort *mPort {nullptr};
    };

    class TFDistClient : public QObject
    {
        Q_OBJECT

    public:
        explicit TFDistClient(QObject* parent = nullptr);
        ~TFDistClient() override;

        bool open();
        void close();

    signals:
        void distanceUpdated(float meters);

    private slots:
        void onBytesArrived(const QByteArray& bytes);

    private:
        void parseLoop();

        static uint16_t crc16Rtu(const uint8_t* data, size_t len);
        static bool checkFrameCrc9(const uint8_t frame[9]);

        static constexpr uint8_t H0 = 0x01;
        static constexpr uint8_t H1 = 0x03;
        static constexpr uint8_t H2 = 0x04;

        static QByteArray cmdOpenContinuous(); // 01 06 00 11 00 02 58 0E
        static QByteArray cmdClose(); // 01 06 00 11 00 00 D9 CF

    private:
        // worker thread
        QThread mRecvThread;
        DistRecvWorker* mWorker = nullptr;

        // byte queue (producer-consumer)
        std::mutex mMtx;
        std::condition_variable mCv;
        std::deque<uint8_t> mQueue;

        std::atomic_bool mRunning{false};
        std::thread mParseThread;

        DistanceFilter mFilter;
    };
};


#endif //FIREAPP_TFDISTCLIENT_H
