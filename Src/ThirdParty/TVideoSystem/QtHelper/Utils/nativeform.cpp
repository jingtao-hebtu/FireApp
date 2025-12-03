#include "nativeform.h"

#ifdef Q_OS_WIN

#include "windows.h"

#endif

NativeForm::NativeForm(QWidget *parent) : QWidget(parent) {

}

void NativeForm::exitAll() {
    //如果不采用暴力退出可能会打印出现 QThread: Destroyed while thread is still running
    //如果在关闭的时候出现崩溃现象,建议启用下面这行代码
    //有些msvc套件暴力退出的时候会提示: 快速异常检测失败,将不会调用异常处理程序
#ifndef Q_CC_MSVC
    exit(0);
#endif
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#ifdef Q_OS_WIN
bool NativeForm::winEvent(MSG *message, long *result)
{
    return nativeEvent("windows_generic_MSG", message, result);
}
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

bool NativeForm::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool NativeForm::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif

{
    if (eventType == "windows_generic_MSG") {
#ifdef Q_OS_WIN
        MSG *msg = static_cast<MSG *>(message);
        //qDebug() << TIMEMS << msg->wParam << msg->message;
        if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST) {
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC) {
            //休眠唤醒后自动打开
            this->showNormal();
        }
#endif
    } else if (eventType == "NSEvent") {
#ifdef Q_OS_MACOS
#endif
    } else if (eventType == "xcb_generic_event_t") {
#ifdef Q_OS_LINUX
#endif
    }

    return false;
}
