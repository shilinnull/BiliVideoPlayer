#ifndef AVATARBUTTON_H
#define AVATARBUTTON_H

#include <QPushButton>
#include <QLabel>

class AvatarButton : public QPushButton
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
