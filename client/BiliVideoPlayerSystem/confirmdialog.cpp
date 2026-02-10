#include "confirmdialog.h"

#include <QFrame>
#include <QPushButton>

#include "bilivideoplayer.h"

ConfirmDialog::ConfirmDialog(QWidget *parent)
    :QDialog(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(1450, 860);

    // BiliVideoPlayer* biliplayer = BiliVideoPlayer::getInstance();
    // QPoint point = biliplayer->mapToGlobal(QPoint(0, 0));
    // move(point);

    // 设置父窗口--将来窗口就是基于父窗口位置显示的
    this->setParent(BiliVideoPlayer::getInstance());
    this->move(0, 0);  // 移动到父窗口的左上角

    // 遮罩层
    QWidget* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->setFixedSize(this->maximumSize());
    bg->setStyleSheet("#bg{background-color : rgba(0, 0, 0, 0.3);}");

    // 白色窗口区域
    QFrame* frame = new QFrame(bg);
    frame->setObjectName("container");
    frame->setFixedSize(358, 169);
    frame->move(546, 346);
    frame->setStyleSheet("#container{"
                         "background-color : #FFFFFF;"
                         "border-radius : 10px;}"
                         "*{font-size : 16px;"
                         "font-family : 微软雅黑;}");

    // 窗口文本
    textLabel = new QLabel("确认退出吗？", frame);
    textLabel->setStyleSheet("QLabel{color : #000000;}");
    textLabel->move(115, 40);

    // 确认和取消按钮
    QPushButton* confirmBtn = new QPushButton("确认", frame);
    QPushButton* cancelBtn = new QPushButton("取消", frame);
    confirmBtn->setStyleSheet("background-color : #FF6699;"
                              "border-radius : 8px;"
                              "color : #FFFFFF;");
    confirmBtn->setFixedSize(100, 38);
    confirmBtn->move(194, 111);
    connect(confirmBtn, &QPushButton::clicked, this, [=]{
        isConfirm = true;
        confirmBtn->setStyleSheet("background-color : #FF6699;"
                                  "border-radius : 8px;"
                                  "color : #FFFFFF;");
        cancelBtn->setStyleSheet("border-radius : 8px;"
                                 "border : 1px solid #DEDEDE;");
        close();
    });

    cancelBtn->setStyleSheet("border-radius : 8px;"
                             "border : 1px solid #DEDEDE;");
    cancelBtn->setFixedSize(100, 38);
    cancelBtn->move(64, 111);
    connect(cancelBtn, &QPushButton::clicked, this, [=]{
        isConfirm = false;
        cancelBtn->setStyleSheet("background-color : #FF6699;"
                                 "border-radius : 8px;"
                                 "color : #FFFFFF;");
        confirmBtn->setStyleSheet("border-radius : 8px;"
                                  "border : 1px solid #DEDEDE;");
        close();
    });
}

bool ConfirmDialog::isConfirmPress() const
{
    return isConfirm;
}

void ConfirmDialog::setOperatorText(const QString &text)
{
    textLabel->setText(text);
}
