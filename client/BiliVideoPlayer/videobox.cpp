#include "videobox.h"
#include "ui_videobox.h"

#include <QAction>
#include <QBrush>
#include <QCursor>
#include <QMenu>
#include <QPalette>

#include "model/datacenter.h"
#include "playerpage.h"
#include "util.h"

QPointer<PlayerPage> VideoBox::playPage = nullptr;

VideoBox::VideoBox(model::VideoInfo videoInfo, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
    , videoInfo(videoInfo)
{
    ui->setupUi(this);
    ui->delVideoBtn->hide();    // 默认是隐藏的

    ui->imageBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);

    // 获取视频封面和用户头像
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::downloadPhotoDone,
            this, &VideoBox::getVideoImageDone);
    connect(dataCenter, &model::DataCenter::downloadPhotoDone,
            this, &VideoBox::getUserImageDone);

    updateVideoInfoUI();		// 设置视频信息到界面
    // ...按钮点击
    connect(ui->delVideoBtn, &QPushButton::clicked, this, &VideoBox::onMoreBtnClicked);

}

VideoBox::~VideoBox()
{
    delete ui;
}

bool VideoBox::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->imageBox == watched || ui->videoTitle == watched) {
        if(QEvent::MouseButtonPress == event->type()) {
            onPlayBtnClicked();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void VideoBox::updateVideoInfoUI()
{
    ui->videoTitle->setText(videoInfo.videoTitle);
    ui->likeNum->setText(intToString(videoInfo.likeCount));
    ui->playNum->setText(intToString(videoInfo.playCount));
    ui->userNikeName->setText(videoInfo.nickName);
    ui->loadupTime->setText(" · " + formatDate(videoInfo.videoUpTime));
    setVideoDuration(videoInfo.videoDuration);
    setVideoImage(videoInfo.photoFileId);
    setUserIcon(videoInfo.userAvatarId);
}

void VideoBox::showMoreBtn(bool isShow)
{
    if(isShow) {
        ui->delVideoBtn->show();
    } else {
        ui->delVideoBtn->hide();
    }
}

void VideoBox::onPlayBtnClicked()
{
    // 获取弹幕
    auto dataCenter = model::DataCenter::getInstance();
    // 断开弹幕获取成功所绑定的所有槽函数
    disconnect(dataCenter, &model::DataCenter::getVideoBarrageDone, nullptr, nullptr);
    // 获取弹幕成功的信号槽绑定
    connect(dataCenter, &model::DataCenter::getVideoBarrageDone, this, &VideoBox::getVideoBarrageSuccess);
    dataCenter->getVideoBarrageAsync(videoInfo.videoId);
}

void VideoBox::setVideoDuration(int64_t duration)
{
    QString time;
    if(duration / 60 / 60) {
        time += QString::asprintf("%02lld:", duration / 60 / 60);
    }
    time += QString::asprintf("%02lld:%02lld", duration / 60, duration % 60);
    ui->videoDuration->setText(time);
}

void VideoBox::setVideoImage(const QString &photoFileId)
{
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->downloadPhotoAsync(photoFileId);
}

void VideoBox::setUserIcon(const QString &userAvatarId)
{
    if(userAvatarId.isEmpty()) {
        ui->userIcon->setStyleSheet("border-image: url(:/images/myself/defaultAvatar.png);");
    } else {
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->downloadPhotoAsync(userAvatarId);
    }
}

void VideoBox::setNicknameOfVideoUser(const QString &nickName)
{
    videoInfo.nickName = nickName;
    ui->userNikeName->setText(nickName);
}

void VideoBox::paintEvent(QPaintEvent *event)
{
    // 防止图片还没下载好就对QPixmap进行绘制
    if(videoCoverImage.isNull()) {
        QWidget::paintEvent(event);
        return;
    }
    // 绘制图片
    ui->imageBox->setAutoFillBackground(true);
    videoCoverImage = videoCoverImage.scaled(ui->imageBox->size(),
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    QPalette palette = ui->imageBox->palette();
    QBrush brush(videoCoverImage);
    palette.setBrush(QPalette::Window, brush);
    ui->imageBox->setPalette(palette);
}

void VideoBox::getVideoImageDone(const QString &imageId, QByteArray imageData)
{
    if(videoInfo.photoFileId != imageId)
        return ;
    videoCoverImage.loadFromData(imageData);
    repaint();
}

void VideoBox::getUserImageDone(const QString &imageId, QByteArray imageData)
{
    if(videoInfo.userAvatarId != imageId)
        return ;
    // 制作圆心头像设置到界面上
    userPixmap = makeCircleIcon(imageData,
                                ui->userIcon->width() / 2).pixmap(ui->userIcon->size());
    ui->userIcon->setPixmap(userPixmap);
}

void VideoBox::getVideoBarrageSuccess(const QString &videoId)
{
    if(videoId != videoInfo.videoId)
        return;

    if(!playPage.isNull()) {
        playPage->close();
    }

    playPage = new PlayerPage(videoInfo);
    playPage->setUserIcon(userPixmap);
    playPage->show();   // 显示播放窗口
    playPage->startPlaying();

    // 更新videoBox播放数
    connect(playPage, &PlayerPage::increasePlayCount, this, [=](const QString& videoId){
        if(videoId == this->videoInfo.videoId) {
            LOG() << "更新videoBox播放数";
            this->videoInfo.playCount++;
            ui->playNum->setText(intToString(this->videoInfo.playCount));
            return;
        }
    });
    connect(playPage, &PlayerPage::updateLikeNum, this, [=](int64_t likeCount){
        LOG()<<"更新 videoBox 点赞数";
        this->videoInfo.likeCount = likeCount;
        ui->likeNum->setText(intToString(videoInfo.likeCount));
    });
}

void VideoBox::onMoreBtnClicked()
{
    // 定义菜单的样式
    QString style = "QMenu {"
                    "background-color:#FFFFFF;"
                    "border:none;"
                    "border-radius: 6px;"
                    "padding: 0; }";

    style += "QMenu::item {"
             "background-color:#FFFFFF;"
             "border: none;"
             "border-radius: 6px;"
             "min-width: 50px;"
             "min-height: 32px;"

             "font-size: 12px;"
             "color: #222222;"
             "padding-left: 24px;}";

    style += "QMenu::item:selected { "
             "background-color: rgb(236, 93, 133); "
             "color: #FFFFFF; }";
    QMenu menu(this);
    menu.setStyleSheet(style);
    // 让圆角生效: 去掉窗口框架和阴影、设置透明度
    menu.setWindowFlags(menu.windowFlags() | Qt::FramelessWindowHint
                        | Qt::NoDropShadowWindowHint);
    menu.setAttribute(Qt::WA_TranslucentBackground, true);

    menu.addAction("删除");

    QPoint point = QCursor::pos();
    QAction* action = menu.exec(point);
    if(nullptr == action) { // 用户取消菜单则返回
        return ;
    }
    if(action->text() == "删除") {
        LOG() << "删除视频: " << videoInfo.videoId;
        emit deleteVideo(videoInfo.videoId);    // 发送信号让MyselfWidget真正删除视频操作
    }
}



