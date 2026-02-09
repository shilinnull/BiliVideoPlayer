#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QDialog>
#include <QLabel>

class ConfirmDialog : public QDialog
{
public:
    explicit ConfirmDialog(QWidget *parent = nullptr);
    bool isConfirmPress()const;
    // 取消关注、审核时修改视频状态需要用到
    void setOperatorText(const QString& text);
private:
    bool isConfirm = false;     // 是否点击了确认
    QLabel* textLabel;
};

#endif // CONFIRMDIALOG_H
