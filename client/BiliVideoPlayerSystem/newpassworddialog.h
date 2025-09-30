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

private:
    Ui::NewPasswordDialog *ui;
};

#endif // NEWPASSWORDDIALOG_H
