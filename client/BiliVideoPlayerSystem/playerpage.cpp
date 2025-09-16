#include "playerpage.h"
#include "ui_playerpage.h"

PlayerPage::PlayerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_ShowModal, true);       // 设置窗口为模态
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);
}

PlayerPage::~PlayerPage()
{
    delete ui;
}

void PlayerPage::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)) {
        if(event->button() == Qt::LeftButton) {
            dragPos = event->globalPosition().toPoint() - this->geometry().topLeft();
            return ;
        }
    }
    return QWidget::mousePressEvent(event);
}

void PlayerPage::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)) {
        if(event->buttons() == Qt::LeftButton) {
            this->move(event->globalPosition().toPoint() - dragPos);
            return ;
        }
    }
    return QWidget::mouseMoveEvent(event);
}
