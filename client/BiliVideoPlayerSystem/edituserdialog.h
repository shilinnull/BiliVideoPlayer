#ifndef EDITUSERDIALOG_H
#define EDITUSERDIALOG_H

#include <QDialog>
#include <QString>

#include "model/data.h"

namespace Ui {
class EditUserDialog;
}

class EditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditUserDialog(QWidget *parent, const QString& text, model::AdminInfo& adminInfo);
    ~EditUserDialog();
    bool getCommitResult()const;     // 确认用户是否提交
    void setEmailEditReadOnly(bool isReadOnly = true);  // 设置邮箱是否只读

private slots:
    void onCancelBtnClicked();
    void onSubmitBtnClicked();

private:
    Ui::EditUserDialog *ui;
    model::AdminInfo& adminInfo;
    bool isCommit = false;
};

#endif // EDITUSERDIALOG_H
