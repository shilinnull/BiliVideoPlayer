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

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
