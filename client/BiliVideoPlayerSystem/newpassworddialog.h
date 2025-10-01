#ifndef NEWPASSWORDDIALOG_H
#define NEWPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class NewPasswordDialog;
}

class NewPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewPasswordDialog(QWidget *parent = nullptr);
    ~NewPasswordDialog();

    QString getNewPassword() const;
private slots:
    void onSubmitBtnClicked();
    void onEdittingFinished();

private:
    bool checkPasswordEdit();
    QString passwordValid(const QString& password);

private:
    Ui::NewPasswordDialog *ui;

    QString newPassword;        // 保存用户修改后的密码
};

#endif // NEWPASSWORDDIALOG_H
