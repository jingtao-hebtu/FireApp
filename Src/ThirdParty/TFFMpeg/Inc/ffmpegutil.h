#ifndef FFMPEGUTIL_H
#define FFMPEGUTIL_H

#include "ffmpeghelper.h"

class FFmpegUtil {
public:
    //获取编码解码格式 https://blog.csdn.net/xu13879531489/article/details/80703465
    static void getCodec(QStringList &listEncoderVideo, QStringList &listEncoderAudio, QStringList &listEncoderOther,
                         QStringList &listDecoderVideo, QStringList &listDecoderAudio, QStringList &listDecoderOther);

    //获取支持的输入输出协议
    static void getProtocol(QStringList &listProtocolsIn, QStringList &listProtocolsOut);

    //获取对应库编译参数
    static QString getConfiguration();

    //获取硬解码字符串 ffmpeg -hwaccels
    static QStringList getHardware();

    //打印各种信息
    static void debugAll(bool codec = true, bool protocol = true, bool other = true);

    //打印编解码器和流相关参数
    static void debugPara(AVCodecContext *codecCtx);

    static void debugPara(AVStream *stream);

    //视频帧旋转
    static void rotateFrame(int rotate, AVFrame *frameSrc, AVFrame *frameDst);

    //获取文件时长
    static qint64 getDuration(const QString &fileName, quint64 *useTime = NULL);

    //判断文件是否加密
    static bool checkEncry(const QString &fileName);

    //判断是否含有B帧
    static bool hasB(const QString &fileName, int maxFrame = 10);

    //2进制字符串转10进制
    static int binToDecimal(const QString &bin);

    //10进制转2进制字符串
    static QString decimalToBin(int decimal);

    //两个字节转int数据
    static int dataToInt(quint8 data1, quint8 data2);

    //解析sps/pps
    static void getExtraData(AVCodecContext *codecCtx);

    //测试函数
    static void test(const QString &fileName = QString());

    //数据转字符串
    static QString dataToString(char *data, qint64 len);

    static QStringList dataToStringList(char *data, qint64 len);
};

#endif // FFMPEGUTIL_H
