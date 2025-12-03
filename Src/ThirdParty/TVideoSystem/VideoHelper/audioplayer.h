#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

class AudioPlayer : public QObject {
Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = 0);

    ~AudioPlayer();

public:
    //打开和关闭
    Q_INVOKABLE void open();

    Q_INVOKABLE void close();

    //设置弱属性/避免跨线程调用导致崩溃
    Q_INVOKABLE bool setValue(const char *name, const QVariant &value);

    //获取是否已经运行
    bool getIsOk() const;

    //获取和设置音量
    int getVolume() const;

    void setVolume(int volume);

    //获取和设置静音
    bool getMuted() const;

    void setMuted(bool muted);

    //获取和设置音频振幅
    bool getLevel() const;

    void setLevel(bool audioLevel);

    //获取和设置播放速度
    float getSpeed() const;

    void setSpeed(float speed);

private slots:

    //读取音频输入设备数据
    void readyRead();

public slots:

    //打开和关闭音频输入
    void openAudioInput(int sampleRate, int channelCount, int sampleSize);

    void openAudioInput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize);

    void closeAudioInput();

    //打开和关闭音频输出
    void openAudioOutput(int sampleRate, int channelCount, int sampleSize);

    void openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize);

    void closeAudioOutput();

    //打开和关闭变声库
    void openSonic(bool useSonic, int sampleRate, int channelCount, float speed, float pitch = 1.0, float rate = 1.0,
                   float volume = 1.0);

    void closeSonic();

    //播放音频数据
    void playSonicData(const char *data, qint64 len);

    void playAudioData(const char *data, qint64 len);

signals:

    //音频输入数据
    void receiveInputData(const QByteArray &data);

    //音频输出数据
    void receiveOutputData(const char *data, qint64 len);

    //音量大小
    void receiveVolume(int volume);

    //静音状态
    void receiveMuted(bool muted);

    //音频数据振幅
    void receiveLevel(qreal leftLevel, qreal rightLevel);
};

#endif // AUDIOPLAYER_H
