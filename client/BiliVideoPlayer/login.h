#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    void reset();

private slots:
    void onPasswordBtnClicked();    // 选择密码登录
    void onEmailBtnClicked();       // 选择邮箱登录
    void onAuthcodeBtnClicked();    // 获取邮箱验证码
    void onAuthcodeDone(const QString& authcodeId); // 获取邮箱验证码完成
    void onLoginAndRegisterBtnClicked();    // 登录注册
    void onRegisterNowBtnClicked(); // ⽴即注册
    void onLoginNowBtnClicked();    // 密码登录
signals:
    void loginSuccess();
private:
    Ui::Login *ui;
    QString authcodeId;     // 保存从服务器获取的邮箱验证码
};

#endif // LOGIN_H
