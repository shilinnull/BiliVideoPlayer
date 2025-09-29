#ifndef MODIFYMYSELFDIALOG_H
#define MODIFYMYSELFDIALOG_H

#include <QDialog>

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
};

#endif // MODIFYMYSELFDIALOG_H
