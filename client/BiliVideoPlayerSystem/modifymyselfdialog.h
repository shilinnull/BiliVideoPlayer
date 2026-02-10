#ifndef MODIFYMYSELFDIALOG_H
#define MODIFYMYSELFDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ModifyMyselfDialog;
}

class ModifyMyselfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyMyselfDialog(QWidget *parent = nullptr);
    ~ModifyMyselfDialog();

private slots:
    void onSubmitBtnClicked();
    void onCancelBtnClicked();
    void showPasswordDlg();

private:
    Ui::ModifyMyselfDialog *ui;
    QString newPassword;        // 修改新的密码，通过 NewPasswordDialog 传进来的
};

#endif // MODIFYMYSELFDIALOG_H
