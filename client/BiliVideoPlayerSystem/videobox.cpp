#include "videobox.h"
#include "ui_videobox.h"
#include "util.h"
#include "model/datacenter.h"
#include <QDir>
#include <QPixmap>

PlayerPage* VideoBox::playPage = nullptr;

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
    connect(dataCenter, &model::DataCenter::getVideoBarrageDone,
            this, &VideoBox::getVideoBarrageSuccess);

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
            LOG() << "按下播放按钮";
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
    ui->loadupTime->setText(videoInfo.videoUpTime);
    setVideoDuration(videoInfo.videoDuration);
    setVideoImage(videoInfo.photoFileId);
    setUserIcon(videoInfo.userAvatarId);
}

void VideoBox::onPlayBtnClicked()
{
    // 如果已经有一个播放器窗口存在，则先销毁它
    if(playPage)
        delete playPage;

    // 创建一个新的播放器实例
    playPage = new PlayerPage(videoInfo);
    // 当窗口被销毁（例如关闭）时，将静态指针重置为nullptr
    connect(playPage, &PlayerPage::destroyed, this, [=](){
        playPage = nullptr;
    });
    // 更新videoBox播放数
    connect(playPage, &PlayerPage::increasePlayCount, this, [=](const QString& videoId){
        if(videoId == this->videoInfo.videoId) {
            LOG() << "更新videoBox播放数";
            this->videoInfo.playCount++;
            ui->playNum->setText(intToString(this->videoInfo.playCount));
            return;
        }
    });
    playPage->show();

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->downloadVideoAsync(videoInfo.videoFileId);  // 获取视频
    dataCenter->getVideoBarrageAsync(videoInfo.videoId);    // 获取弹幕
    connect(dataCenter, &model::DataCenter::downloadVideoDone, this,
            [=](const QString& videoFilePath, const QString& videoFileId){
        if(videoInfo.videoFileId != videoFileId)
            return;
        playPage->startPlaying();  // 开始播放
    });

    connect(playPage, &PlayerPage::updateLikeNum, this, [=](int64_t likeCount){
        LOG()<<"更新 videoBox 点赞数";
        this->videoInfo.likeCount = likeCount;
        ui->likeNum->setText(intToString(videoInfo.likeCount));
    });
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
    playPage->setUserIcon(userPixmap);  // 设置用户头像
    playPage->show();
    playPage->startPlaying();
}


