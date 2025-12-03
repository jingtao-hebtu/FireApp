#ifndef NATIVEFORM_H
#define NATIVEFORM_H

#include <QWidget>

class NativeForm : public QWidget {
Q_OBJECT

public:
    explicit NativeForm(QWidget *parent = 0);

protected:
    //彻底退出
    void exitAll();

    //Qt4的写法
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif
#endif

    //拦截系统事件用于修复系统休眠后唤醒程序的bug
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);

#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif
};

#endif // NATIVEFORM_H
