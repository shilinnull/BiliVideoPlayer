#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);
    void onSendBulletScreenBtn();			// 发送按钮点击

signals:
    void onSendScreenBtn(const QString& text);

private:
    QPushButton* sendBSBtn;
};

#endif // BARRAGEEDIT_H
