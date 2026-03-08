#include "avatarbutton.h"

MyPushButton::MyPushButton(QWidget *parent)
    :QPushButton(parent)
{}

void MyPushButton::setClickedStatus(bool enable)
{
    this->clickedStatus = enable;
}

void MyPushButton::mousePressEvent(QMouseEvent *event)
{
    if(clickedStatus) {
        QPushButton::mousePressEvent(event);
    }
}

AvatarButton::AvatarButton(QWidget *parent)
    : MyPushButton{parent}
    , mask(nullptr)
    , showMask(true)
{
    mask = new QLabel(this);
    mask->setObjectName("mask");
    mask->setGeometry(0, 0, 60, 60);
    mask->setStyleSheet("#mask{"
                        "color: #FFFFFF;"
                        "background-color: rgba(0, 0, 0, 0.5);"
                        "border-radius: 30px;}");
    mask->setText("修改头像");
    mask->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mask->hide();

    // 点击 mask 也会落到 mask 背后的按钮上
    mask->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void AvatarButton::changeMod(bool showMask)
{
    this->showMask = showMask;
}

void AvatarButton::enterEvent(QEnterEvent *event)
{
    if(showMask) {
        mask->show();
    }
}

void AvatarButton::leaveEvent(QEvent *event)
{
    mask->hide();
}
