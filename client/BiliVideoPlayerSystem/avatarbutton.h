#ifndef AVATARBUTTON_H
#define AVATARBUTTON_H

#include <QLabel>
#include <QPushButton>
#include <QEnterEvent>
#include <QEvent>

class MyPushButton : public QPushButton {
    Q_OBJECT
public:
    explicit MyPushButton(QWidget* parent = nullptr);
    void setClickedStatus(bool enable);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool clickedStatus;     // 标记按钮是否可以被点击
};

class AvatarButton : public MyPushButton
{
    Q_OBJECT
public:
    explicit AvatarButton(QWidget *parent = nullptr);
    // 是否显示遮罩
    void changeMod(bool showMask);
protected:
    void enterEvent(QEnterEvent* event) override ;
    void leaveEvent(QEvent* event) override;

signals:
private:
    QLabel* mask;   // 遮罩层
    bool showMask;  // 是否显示
};

#endif // AVATARBUTTON_H
