#include "audioplayer.h"
#include "widgethead.h"

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent) {
    //注册数据类型
    qRegisterMetaType<const char *>("const char *");
}

AudioPlayer::~AudioPlayer() {

}

void AudioPlayer::open() {

}

void AudioPlayer::close() {

}

bool AudioPlayer::setValue(const char *name, const QVariant &value) {
    return this->setProperty(name, value);
}

bool AudioPlayer::getIsOk() const {
    return false;
}

int AudioPlayer::getVolume() const {
    return 100;
}

void AudioPlayer::setVolume(int volume) {

}

bool AudioPlayer::getMuted() const {
    return false;
}

void AudioPlayer::setMuted(bool muted) {

}

bool AudioPlayer::getLevel() const {
    return false;
}

void AudioPlayer::setLevel(bool level) {

}

float AudioPlayer::getSpeed() const {
    return 1;
}

void AudioPlayer::setSpeed(float speed) {

}

void AudioPlayer::readyRead() {

}

void AudioPlayer::openAudioInput(int sampleRate, int channelCount, int sampleSize) {

}

void AudioPlayer::openAudioInput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize) {

}

void AudioPlayer::closeAudioInput() {

}

void AudioPlayer::openAudioOutput(int sampleRate, int channelCount, int sampleSize) {

}

void AudioPlayer::openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize) {

}

void AudioPlayer::closeAudioOutput() {

}

void AudioPlayer::openSonic(bool useSonic, int sampleRate, int channelCount, float speed, float pitch, float rate,
                            float volume) {

}

void AudioPlayer::closeSonic() {

}

void AudioPlayer::playSonicData(const char *data, qint64 len) {

}

void AudioPlayer::playAudioData(const char *data, qint64 len) {

}
