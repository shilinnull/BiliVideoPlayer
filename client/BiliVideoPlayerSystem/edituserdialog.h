#ifndef EDITUSERDIALOG_H
#define EDITUSERDIALOG_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class EditUserDialog;
}

class EditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditUserDialog(QWidget *parent = nullptr, const QString& text = "");
    ~EditUserDialog();

private slots:
    void onCancelBtnClicked();
    void onSubmitBtnClicked();

private:
    Ui::EditUserDialog *ui;
};

#endif // EDITUSERDIALOG_H
