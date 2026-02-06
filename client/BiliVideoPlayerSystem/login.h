#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void onPasswordBtnClicked();    // 选择密码登录
    void onMessageBtnClicked();     // 选择短信登录
    void onAuthcodeBtnClicked();    // 获取短信验证码
    void onLoginAndRegisterBtnClicked();    // 登录注册
    void onRegisterNowBtnClicked(); // ⽴即注册
    void onLoginNowBtnClicked();    // 密码登录
signals:
    void loginSuccess();
private:
    Ui::Login *ui;
    QString authcodeId;
};

#endif // LOGIN_H
