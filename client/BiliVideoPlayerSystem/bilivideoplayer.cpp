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
    setWindowIcon(QIcon(":/images/homePage/logo.png"));
}
