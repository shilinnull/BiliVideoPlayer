#include "bilivideoplayer.h"
#include "ui_bilivideoplayer.h"

BiliVideoPlayer::BiliVideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BiliVideoPlayer)
{
    ui->setupUi(this);

    // 初始化UI
    initUI();
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
