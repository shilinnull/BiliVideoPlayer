#include "playerpage.h"
#include "ui_playerpage.h"

#include "util.h"
#include "login.h"
#include "toast.h"
#include "volume.h"
#include "bulletscreenitem.h"
#include <QShortcut>
#include <QKeySequence>
#include <QTimer>
#include "bilivideoplayer.h"
#include <QJsonArray>

PlayerPage::PlayerPage(const model::VideoInfo& videoInfo, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
    , videoInfo(videoInfo)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Bili视频播放器");
    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    volume = new Volume(this);						// 音量
    playSpeed = new PlaySpeed(this);				// 倍速
    mpvPlayer = new MpvPlayer(ui->screen, this);	// 视频
    login = new Login();                            // 登录窗口

    initBarrageArea();								// 初始化弹幕布局
    updateVideoInfoUI();

    // 设置快捷键空格
    QShortcut* shortCut = new QShortcut(ui->playBtn);
    QKeySequence keySqeuence(" ");
    shortCut->setKey(keySqeuence);
    connect(shortCut, &QShortcut::activated, this, [=]{
        ui->playBtn->animateClick();
    });

    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &PlayerPage::onQuitBtnClicked);
    connect(ui->volumeBtn, &QPushButton::clicked, this, &PlayerPage::onVolumeBtnClicked);
    connect(ui->speedBtn, &QPushButton::clicked, this, &PlayerPage::onSpeedBtnClicked);
    connect(ui->likeImageBtn, &QPushButton::clicked, this, &PlayerPage::onLikeImageBtnClicked);
    connect(ui->playBtn, &QPushButton::clicked, this, &PlayerPage::onplayBtnClicked);
    connect(playSpeed, &PlaySpeed::setPlaySpeed, this, &PlayerPage::onPlaySpeedChanged);
    connect(volume, &Volume::setVolume, this, &PlayerPage::setVolume);
    connect(ui->videoSlider, &PlaySlider::setPlayProgress, this, &PlayerPage::onSetPlayProgress);
    connect(mpvPlayer, &MpvPlayer::playPositionChanged, this, &PlayerPage::onPlayPositionChanged);
    connect(mpvPlayer, &MpvPlayer::endOfPlaylist, this, &PlayerPage::onEndOfPlayList);
    connect(ui->bulletScreenBtn, &QPushButton::clicked, this, &PlayerPage::onBulletScreenClicked);
    connect(ui->bulletScreenText, &BarrageEdit::onSendScreenBtn, this, &PlayerPage::onSendBulletScreenBtnClicked);
    connect(ui->userAvatar, &QPushButton::clicked, this, &PlayerPage::onUserAvatarClicked);

    likeCount = videoInfo.likeCount;        // 更新播放数
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getIsLikeVideoDone, this,
            [=](const QString& videoId, bool isLike){
        if(videoId != videoInfo.videoId) {
            return ;
        }
        if(isLike) {
            ui->likeImageBtn->setStyleSheet("#likeImageBtn{border-image: url(:/images/PlayPage/dianzan.png)}");
        } else {
            ui->likeImageBtn->setStyleSheet("#likeImageBtn{border-image: url(:/images/PlayPage/quxiaodianzan.png)}");
        }
        this->isLike = isLike;
    });
    dataCenter->getIsLikeVideoAsync(videoInfo.videoId);

}

PlayerPage::~PlayerPage()
{
    delete ui;
    delete mpvPlayer;
    delete login;
}

void PlayerPage::moveWindows(const QPoint &point)
{
    // 13 是音量调节窗口右边界和PlayerPage右边界的距离
    QPoint newPoint = point + QPoint(this->width() - volume->width() - 13, 533);
    volume->move(newPoint);

    newPoint = point + QPoint(this->width() - playSpeed->width() - 72, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::startPlaying()
{
    buildBulletScreenData();
    auto dataCenter = model::DataCenter::getInstance();
    QString m3u8FileUrl = dataCenter->getServerUrl();
    m3u8FileUrl += "/HttpService/downloadVideo?fileId=";
    m3u8FileUrl += videoInfo.videoFileId;
    mpvPlayer->startPlay(m3u8FileUrl);

    isUpdatePlayNum = false;
    ui->videoSlider->setPlayStep(0);
    // 视频加载成功之后会立马播放，初始时先将其设置为暂停状态，当用户点击播放按钮之后再让视频播放起来
    mpvPlayer->pause();
}

void PlayerPage::buildBulletScreenData()
{
    auto dataCenter = model::DataCenter::getInstance();
    bulletScreens = dataCenter->getBarragesData();
}

void PlayerPage::showBulletScreen()
{
    // 弹幕关闭
    if(!isStartBS) return ;

    // 通过时间获取弹幕数据
    QList<model::BarrageInfo> bulletScreenList = bulletScreens.value(mpvPlayer->getPlayTime());
    // 显示弹幕
    int xTop, xMid, xBottom;
    xTop = xMid = xBottom = top->width();
    BulletScreenItem* bs = nullptr;
    for(int i = 0; i < bulletScreenList.size(); i++) {
        model::BarrageInfo& bsInfo = bulletScreenList[i];
        if(0 == i % 3) {
            // 显示第一行
            bs = new BulletScreenItem(top);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xTop / (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xTop, duration);
            xTop += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        } else if(1 == i % 3) {
            // 显示第二行
            bs = new BulletScreenItem(middle);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xMid/ (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xMid, duration);
            xMid += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        } else {
            // 显示第三行
            bs = new BulletScreenItem(bottom);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xBottom / (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xBottom + 2 * 18, duration);
            xBottom += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        }
        bs->startAnimal();
    }
}

void PlayerPage::setUserIcon(const QPixmap& userPixmap)
{
    ui->userAvatar->setIcon(QIcon(userPixmap));
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
            QPoint point = geometry().topLeft();
            point.setY(point.ry() + 60);
            barrageArea->move(point);
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
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(myself->isTempUser()) {
        Toast::showMessage("先登录/注册后才能点赞！", login);
        return;
    }

    // 如果是视频审核状态
    if(videoInfo.videoStatus == model::VideoStatus::waitForChecking) {
        Toast::showMessage("视频审核中禁止点赞！");
        return ;
    }

    // 更新点赞
    isLike = !isLike;
    if(isLike) {
        likeCount++;
        ui->likeImageBtn->setStyleSheet("#likeImageBtn{border-image: url(:/images/PlayPage/dianzan.png)}");
    } else {
        likeCount--;
        ui->likeImageBtn->setStyleSheet("#likeImageBtn{border-image: url(:/images/PlayPage/quxiaodianzan.png)}");
    }
    ui->likeNum->setText(intToString(likeCount));
}

void PlayerPage::onplayBtnClicked()
{
    isPlay = !isPlay;
    if(isPlay) {
        // 播放
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/bofang.png)");
        if(mpvPlayer) mpvPlayer->play();
        barrageArea->show();	// 弹幕显示
    } else {
        // 暂停
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/zanting.png)");
        if(mpvPlayer) mpvPlayer->pause();
        barrageArea->hide();	// 弹幕隐藏
    }
    // 在本次播放中，视频的播放数未更新时再更新播放数
    if(!isUpdatePlayNum)
        updataPlayCount();
}

void PlayerPage::onPlaySpeedChanged(double speed)
{
    if(mpvPlayer) mpvPlayer->setPlaySpeed(speed);
}

void PlayerPage::setVolume(int volumeRatio)
{
    if(mpvPlayer) mpvPlayer->setVolume(volumeRatio);
}

void PlayerPage::onPlayPositionChanged(int64_t playTime)
{
    QString curPlayTime = secondToTime(playTime);
    QString totalTime = secondToTime(videoInfo.videoDuration);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);

    // 修改进度条
    ui->videoSlider->setPlayStep((double)playTime / videoInfo.videoDuration);

    // 更新弹幕数据
    showBulletScreen();
}

void PlayerPage::onEndOfPlayList()
{
    isPlay = false;
    ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/zanting.png);");
    // 重新点击播放按钮进行播放视频
    startPlaying();
}

void PlayerPage::onSetPlayProgress(double playRatio)
{
    // 更新播放时间
    int playTime = playRatio * videoInfo.videoDuration;
    mpvPlayer->setCurrentPlayPositon(playTime);
}

void PlayerPage::onBulletScreenClicked()
{
    // 是否显示弹幕
    isStartBS = !isStartBS;
    if(isStartBS) {
        ui->bulletScreenBtn->setStyleSheet("border-image: url(:/images/PlayPage/danmu.png);");
        barrageArea->show();
    } else {
        ui->bulletScreenBtn->setStyleSheet("border-image: url(:/images/PlayPage/danmuguan.png);");
        barrageArea->hide();
    }
}

void PlayerPage::onSendBulletScreenBtnClicked(const QString &text)
{
    // 视频在审核就不能发送弹幕
    if(videoInfo.videoStatus == model::VideoStatus::waitForChecking) {
        Toast::showMessage("视频在审核时不能发送弹幕");
        return ;
    }
    // 弹幕关闭就不继续执行
    if(!isStartBS) {
        Toast::showMessage("请打开弹幕开关...");
        return ;
    }

    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(myself == nullptr || myself->isTempUser()) {
        Toast::showMessage("请先登录/注册才能发评论！", login);
        return;
    }

    if(text.isEmpty()) {
        Toast::showMessage("弹幕信息为空，请先输入弹幕再发送信息！");
        return;
    }

    BulletScreenItem* bs = new BulletScreenItem(top);	// 显示到top栏上
    QPixmap pixmap(":/images/homePage/touxiang.png");
    bs->setBulletScreenIcon(pixmap);
    bs->setBulletScreenText(text);
    int duration = 10000 * width() / (double)(30 * 18 + 1450);
    bs->setBulletScreenAnimal(top->width(), duration);
    bs->startAnimal();

    model::BarrageInfo barrageInfo;
    barrageInfo.playTime = mpvPlayer->getPlayTime();
    barrageInfo.text = text;
    barrageInfo.userId = myself->userId;
    dataCenter->loadupBarragesAsync(videoInfo.videoId, barrageInfo);
}

QString PlayerPage::secondToTime(int64_t second) const
{
    QString time;
    time.reserve(20);
    // 小时
    if(second / 60 / 60) {
        time += QString::asprintf("%02lld:", second / 60 / 60);
    }
    // 分钟
    time += QString::asprintf("%02lld:%02lld", second / 60, second % 60);
    return time;
}

void PlayerPage::initBarrageArea()
{
    // 创建弹幕显示区域的对话框，没有边框，背景透明
    barrageArea = new QDialog(this);
    barrageArea->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    barrageArea->setAttribute(Qt::WA_TranslucentBackground);
    barrageArea->setMinimumSize(this->width(), 38 * 3);

    // 垂直布局
    QVBoxLayout* layout = new QVBoxLayout(barrageArea);
    barrageArea->setLayout(layout);

    // 在弹幕区域添加用来显示三行弹幕的控件
    top = new QFrame(this);
    top->setFixedSize(this->width(), 38);

    middle = new QFrame(this);
    middle->setFixedSize(this->width(), 38);

    bottom = new QFrame(this);
    bottom->setFixedSize(this->width(), 38);

    layout->addWidget(top);
    layout->addWidget(middle);
    layout->addWidget(bottom);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 移动弹幕窗口到head下
    QPoint point = mapToGlobal(QPoint(0, 0));
    point.setY(point.y() + ui->playHead->height());
    barrageArea->move(point);
    barrageArea->show();
}

void PlayerPage::updateVideoInfoUI()
{
    ui->videoTittle->setText(videoInfo.videoTitle);
    ui->userNikeName->setText(videoInfo.nickName);
    ui->loadupTime->setText(videoInfo.videoUpTime);
    ui->likeNum->setText(intToString(videoInfo.likeCount));
    ui->playNum->setText(intToString(videoInfo.playCount));
    QString curPlayTime = secondToTime(0);
    QString totalTime = secondToTime(videoInfo.videoDuration);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);
    ui->videoDesc->setText(videoInfo.videoDesc);
}

void PlayerPage::updataPlayCount()
{
    // 如果视频在审核，就不更新播放数
    if(videoInfo.videoStatus == model::VideoStatus::waitForChecking) {
        return ;
    }
    // 判断是否有效点击
    if(isUpdatePlayNum)
        return;
    videoInfo.playCount++;
    ui->playNum->setText(intToString(videoInfo.playCount));
    auto dataCenter = model::DataCenter::getInstance();
    auto videoList = dataCenter->getVideoListPtr();
    videoList->incrementPlayNum(videoInfo.videoId);

    // 我的页面视频列表
    auto myVideoList = dataCenter->getUserVideoList();
    if(myVideoList->videoInfos.isEmpty()){
        return;
    }else{
        myVideoList->incrementPlayNum(videoInfo.videoId);
    }

    // 管理员视频列表
    auto statusVideoList = dataCenter->getStatusVideoList();
    if(statusVideoList->videoInfos.isEmpty()){
        return;
    }else{
        statusVideoList->incrementPlayNum(videoInfo.videoId);
    }

    // 更新服务器上的该视频播放数
    dataCenter->setPlayNumberAsync(videoInfo.videoId);

    // 通知videobox更新界面上的播放数
    emit increasePlayCount(videoInfo.videoId);
    isUpdatePlayNum = true;
}

void PlayerPage::onQuitBtnClicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getVideoListPtr();
    if(likeCount != videoInfo.likeCount){
        // 视频的点赞信息发生改变，给服务器同步
        dataCenter->setLikeNumberAsync(videoInfo.videoId);

        // 更新视频列表中的videoId视频的点赞信息：首页 和 我的页面
        videoListPtr->updateLikeNum(videoInfo.videoId, likeCount);
        videoInfo.likeCount = likeCount;

        // 通知videoBox更新点赞数据
        emit updateLikeNum(likeCount);
    }
    dataCenter->setBarragesData(QJsonArray());

    // 更新首页视频列表 和 我的视频列表
    videoListPtr->updateLikeNum(videoInfo.videoId, likeCount);

    // 用户视频列表
    auto myVideoList = dataCenter->getUserVideoList();
    if(myVideoList->videoInfos.isEmpty()){
        return;
    }else{
        myVideoList->updateLikeNum(videoInfo.videoId, likeCount);
    }

    // 管理员视频列表
    auto statusVideoList = dataCenter->getStatusVideoList();
    if(statusVideoList->videoInfos.isEmpty()){
        return;
    }else{
        statusVideoList->updateLikeNum(videoInfo.videoId, likeCount);
    }

    // 释放掉，防止内存泄漏
    this->close();
    this->deleteLater();
}

void PlayerPage::onUserAvatarClicked()
{
    // 先关闭当前界面
    onQuitBtnClicked();

    BiliVideoPlayer::getInstance()->switchToUserInfoPage(this->videoInfo.userId);
}
