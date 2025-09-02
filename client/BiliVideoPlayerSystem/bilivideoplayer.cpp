#include "bilivideoplayer.h"
#include "ui_bilivideoplayer.h"

BiliVideoPlayer::BiliVideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BiliVideoPlayer)
{
    ui->setupUi(this);
}

BiliVideoPlayer::~BiliVideoPlayer()
{
    delete ui;
}
