#include "barrageedit.h"
#include "toast.h"
#include "model/datacenter.h"
BarrageEdit::BarrageEdit(QWidget *parent)
    : QLineEdit{parent}
{
    setFixedSize(1030, 32);
    setPlaceholderText("点击发送弹幕");
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
    this->setTextMargins(12, 6, 0, 7);
    connect(sendBSBtn, &QPushButton::clicked, this, &BarrageEdit::onSendBulletScreenBtn);

    login = new Login();
    connect(login, &Login::loginSuccess, this, [=]{
        setPlaceholderText("发个友善的弹幕见证当下");
        sendBSBtn->setText("发送");
    });
}

BarrageEdit::~BarrageEdit()
{
    delete login;
}

void BarrageEdit::onSendBulletScreenBtn()
{
    if("登录" == sendBSBtn->text()) {
        Toast::showMessage("请先登录/注册再进行发送弹幕！", login);
    } else {
        QString barrageText = this->text();
        if(barrageText.isEmpty()) {
            Toast::showMessage("请输入要发送的弹幕~~~");
            return ;
        }
        // 发送弹幕
        emit onSendScreenBtn(barrageText);
        setText("");
    }
}

void BarrageEdit::mousePressEvent(QMouseEvent *event)
{
    // 当点击编辑框的时候需要检查用户是否登录
    auto dataCenter = model::DataCenter::getInstance();
    auto mySelf = dataCenter->getMyselfInfo();
    if(mySelf->isTempUser())
    {
        if(placeholderText() != "请先登录或注册") {
            setPlaceholderText("请先登录或注册");
            sendBSBtn->setText("登录");
        }else{
            Toast::showMessage("请先登录或注册", login);
        }
    }
}
