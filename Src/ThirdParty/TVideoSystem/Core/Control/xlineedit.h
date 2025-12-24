#ifndef XLINEEDIT_H
#define XLINEEDIT_H

/**
 * 多功能文本框 作者:feiyangqingyun(QQ:517216493) 2023-08-06
 * 1. 可设置双击明文显示。
 * 2. 可设置显示密文按钮。
 */

#include <QLineEdit>
class QPushButton;

#ifdef quc
class Q_DECL_EXPORT XLineEdit : public QLineEdit
#else
class XLineEdit : public QLineEdit
#endif

{
    Q_OBJECT

    Q_PROPERTY(bool doubleShowPwd READ getDoubleShowPwd WRITE setDoubleShowPwd)
    Q_PROPERTY(bool pwdButtonEnable READ getPwdButtonEnable WRITE setPwdButtonEnable)

public:
    explicit XLineEdit(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QFont iconFont;             //图形字体
    QPushButton *pwdButton;     //密码按钮

    bool doubleShowPwd;         //双击明文显示
    bool pwdButtonEnable;       //密码按钮可用
    int pwdButtonWidth;         //密码按钮宽度

private:
    //初始化图形字体
    void initFont();
    //初始化按钮
    void initButton();
    //初始化边距
    void initMargins();

private slots:
    //按钮单击处理
    void clicked();
    //切换密码显示
    void setPwdVisible(bool visible);

public Q_SLOTS:
    //设置双击明文显示
    bool getDoubleShowPwd() const;
    void setDoubleShowPwd(bool doubleShowPwd);

    //设置密码按钮可用
    bool getPwdButtonEnable() const;
    void setPwdButtonEnable(bool pwdButtonEnable);
};

#endif // XLINEEDIT_H
