#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QString>

#include "login.h"

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);
    ~BarrageEdit();
    void onSendBulletScreenBtn();			// 发送按钮点击
    void mousePressEvent(QMouseEvent *event) override; // 处理编辑框点击

signals:
    void onSendScreenBtn(const QString& text);

private:
    QPushButton* sendBSBtn;
    Login* login;
};

#endif // BARRAGEEDIT_H
