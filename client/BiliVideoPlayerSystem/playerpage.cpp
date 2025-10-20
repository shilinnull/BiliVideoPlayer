#include "playerpage.h"
#include "ui_playerpage.h"

#include "login.h"
#include "toast.h"

PlayerPage::PlayerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_ShowModal, true);       // 设置窗口为模态

    volume = new Volume(this);
    playSpeed = new PlaySpeed(this);

    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->volumeBtn, &QPushButton::clicked, this, &PlayerPage::onVolumeBtnClicked);
    connect(ui->speedBtn, &QPushButton::clicked, this, &PlayerPage::onSpeedBtnClicked);
    connect(ui->likeImageBtn, &QPushButton::clicked, this, &PlayerPage::onLikeImageBtnClicked);
    connect(ui->playBtn, &QPushButton::clicked, this, &PlayerPage::onplayBtnClicked);
}

PlayerPage::~PlayerPage()
{
    delete ui;
}

void PlayerPage::moveWindows(const QPoint &point)
{
    // 13 是音量调节窗口右边界和PlayerPage右边界的距离
    QPoint newPoint = point + QPoint(this->width() - volume->width() - 13, 533);
    volume->move(newPoint);

    newPoint = point + QPoint(this->width() - playSpeed->width() - 72, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::startPlaying(const QString &videoFilePath)
{
    mpvPlayer = new MpvPlayer(ui->screen, this/*, ui->screen*/);
    mpvPlayer->startPlay(videoFilePath);

    // 视频加载后会立即播放，默认需要先暂停，让用户点击播放按钮进行播放
    mpvPlayer->pause();
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

void PlayerPage::onVolumeBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0, 0)));
    volume->show();
}

void PlayerPage::onSpeedBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0, 0)));
    playSpeed->show();
}

void PlayerPage::onLikeImageBtnClicked()
{
    // 检测用户是否登录，登录才能点赞
    Login* login = new Login();
    // login->show();
    Toast::showMessage("先登录，登录后才能点赞", login);
}

void PlayerPage::onplayBtnClicked()
{
    isPlay = !isPlay;
    if(isPlay) {
        // 播放
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/bofang.png)");
        mpvPlayer->play();
    } else {
        // 暂停
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/zanting.png)");
        mpvPlayer->pause();
    }
}
