#ifndef STARTUPPAGE_H
#define STARTUPPAGE_H

#include <QDialog>
#include <QLabel>
#include <QTimer>

class StartupPage : public QDialog
{
    Q_OBJECT
public:
    explicit StartupPage(QDialog *parent = nullptr);

    void startTimer();
private:
    bool loginSuccess = false;		// 标记临时用户登录是否成功
};

#endif // STARTUPPAGE_H
