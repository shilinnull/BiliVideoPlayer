#include "barrageedit.h"

BarrageEdit::BarrageEdit(QWidget *parent)
    : QLineEdit{parent}
{
    setFixedSize(1030, 32);
    setPlaceholderText("这个视频在说的是什么...");
    setMaxLength(30);		// 最长30个字符
    setTextMargins(12, 6, 0, 7);

    // 初始化发送按钮
    sendBSBtn = new QPushButton(this);
    sendBSBtn->setFixedSize(58, 28);
    sendBSBtn->setText("发送");
    sendBSBtn->setCursor(QCursor(Qt::ArrowCursor));
    sendBSBtn->setStyleSheet("background-color: #FF6699;"
                             "border-radius : 4px;"
                             "color: #FFFFFF;"
                             "font-size : 14px;"
                             "font-family : 微软雅黑;");
    sendBSBtn->move(width() - sendBSBtn->width() - 2, 2);

    connect(sendBSBtn, &QPushButton::clicked, this, &BarrageEdit::onSendBulletScreenBtn);
}

void BarrageEdit::onSendBulletScreenBtn()
{
    emit onSendScreenBtn(this->text());
}
