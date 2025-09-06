#include "bilivideoplayer.h"
#include "ui_bilivideoplayer.h"

BiliVideoPlayer::BiliVideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BiliVideoPlayer)
{
    ui->setupUi(this);

    // 初始化UI
    initUI();
    // 初始化信号和槽
    connectSignalAndSlot();
}

BiliVideoPlayer::~BiliVideoPlayer()
{
    delete ui;
}

void BiliVideoPlayer::initUI()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    QGraphicsDropShadowEffect* dropShadow = new QGraphicsDropShadowEffect(this);
    dropShadow->setColor(Qt::black);
    dropShadow->setBlurRadius(5);
    dropShadow->setOffset(0, 0);
    // this->setGraphicsEffect(dropShadow); // 这样设置会在程序输出窗口出现参数错误
    ui->biliPlayerBg->setGraphicsEffect(dropShadow); // 应该设置到背景上
}

void BiliVideoPlayer::connectSignalAndSlot()
{
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);
}

void BiliVideoPlayer::mousePressEvent(QMouseEvent *event)
{
    // 检查是否在head区域
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)) {
        if(event->button() == Qt::LeftButton) {
            // 记录鼠标按下位置和窗口左上角的相对距离
            dragPos = event->globalPosition().toPoint() - geometry().topLeft();
            return ;
        }
    }
    return QWidget::mousePressEvent(event);
}

void BiliVideoPlayer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)) {
        if(event->buttons() == Qt::LeftButton) {
            move(event->globalPosition().toPoint() - dragPos);
            return ;
        }
    }
    return QWidget::mouseMoveEvent(event);
}
