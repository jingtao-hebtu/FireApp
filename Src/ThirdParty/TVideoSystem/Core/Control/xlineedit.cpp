#pragma execution_character_set("utf-8")

#include "xlineedit.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qfontdatabase.h"
#include "qevent.h"
#include "qpainter.h"
#include "qdebug.h"

XLineEdit::XLineEdit(QWidget *parent) : QLineEdit(parent)
{
    doubleShowPwd = false;
    pwdButtonEnable = false;
    pwdButtonWidth = 28;

    this->initFont();
    this->initButton();
    this->initMargins();

    //安装事件过滤器
    this->installEventFilter(this);
}

bool XLineEdit::eventFilter(QObject *watched, QEvent *event)
{
    //双击显示特性在失去焦点立即切换到密文
    if (doubleShowPwd) {
        int type = event->type();
        if (type == QEvent::MouseButtonDblClick) {
            this->setPwdVisible(true);
        } else if (type == QEvent::FocusOut) {
            this->setPwdVisible(false);
        }
    }

    return QLineEdit::eventFilter(watched, event);
}

void XLineEdit::initFont()
{
    //判断图形字体是否存在/不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/font/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    iconFont.setPixelSize(18);
}

void XLineEdit::initButton()
{
    //实例化密码按钮
    pwdButton = new QPushButton;
    connect(pwdButton, SIGNAL(clicked(bool)), this, SLOT(clicked()));
    pwdButton->setFont(iconFont);
    pwdButton->setText(QChar(0xf070));
    pwdButton->setMaximumWidth(pwdButtonWidth);
    pwdButton->setVisible(pwdButtonEnable);
    pwdButton->setStyleSheet("QPushButton{border:0px solid #ff0000;background:transparent;margin:0px;padding:0px;}");

    //实例化布局用于摆放按钮
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 9, 1);
    layout->addWidget(pwdButton, 0, Qt::AlignRight);
}

void XLineEdit::initMargins()
{
    //设置文本的外边距/空出距离放置按钮
    int margin = (pwdButtonEnable ? pwdButtonWidth : 0);
    this->setTextMargins(0, 0, margin, 0);
}

void XLineEdit::clicked()
{
    this->setPwdVisible(this->echoMode() == QLineEdit::Password);
}

void XLineEdit::setPwdVisible(bool visible)
{
    if (visible) {
        this->setEchoMode(QLineEdit::Normal);
        pwdButton->setText(QChar(0xf06e));
    } else {
        this->setEchoMode(QLineEdit::Password);
        pwdButton->setText(QChar(0xf070));
    }
}

bool XLineEdit::getDoubleShowPwd() const
{
    return this->doubleShowPwd;
}

void XLineEdit::setDoubleShowPwd(bool doubleShowPwd)
{
    this->doubleShowPwd = doubleShowPwd;
    this->setPwdVisible(!doubleShowPwd);
}

bool XLineEdit::getPwdButtonEnable() const
{
    return this->pwdButtonEnable;
}

void XLineEdit::setPwdButtonEnable(bool pwdButtonEnable)
{
    this->pwdButtonEnable = pwdButtonEnable;
    this->pwdButton->setVisible(pwdButtonEnable);
    this->setPwdVisible(!pwdButtonEnable);
    this->initMargins();
}
