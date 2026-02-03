#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "videobox.h"
#include "modifymyselfdialog.h"
#include "bilivideoplayer.h"
#include "model/datacenter.h"

#include <QFileDialog>

MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);
    initUI();
    connectSignalAndSlots();
}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}

void MyselfWidget::uploadViewBtnClicked()
{
    // 1. 打开文件对话框，让用户选择要上传的文件
    QString videoFilePath = QFileDialog::getOpenFileName(nullptr, "上传视频", "", "Videos (*.mp4 *.rmvb *.avi *.mov)");
    if(!videoFilePath.isEmpty()) {
        // 视频限制大小 4G
        QFileInfo fileInfo(videoFilePath);
        int64_t fileSize = fileInfo.size();
        if(fileSize > 4 * 1024 * 1024) {
            LOG() << "视频文件必须小于4G";
            return ;
        }
        emit switchUploadVideoPage(UploadPage);
    }
}

void MyselfWidget::getMyselfInfo()
{
    auto dataCenter = model::DataCenter::getInstance();
    if(dataCenter->getMyselfInfo() == nullptr) {
        dataCenter->getMyselfInfoAsync();       // 获取个人信息
    } else {
        getMyselfInfoDone();                    // 直接获取
    }
}

void MyselfWidget::loadMySelf()
{
    // 加载个人信息
    getMyselfInfo();
    // 加载个人视频列表

    // 切换到个⼈模式，即允许点击用户头像按钮修改个⼈头像
    ui->avatarBtn->changeMod(false);
    ui->avatarBtn->setEnabled(true);
}

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();


#ifdef TEST_UI
    int resourceId = 10000;
    for(int i = 0; i < 16; i++) {
        model::VideoInfo videoInfo;
        videoInfo.userAvatarId = "";
        videoInfo.photoFileId = QString::number(resourceId++);
        videoInfo.videoFileId = QString::number(resourceId++);
        videoInfo.nickName="用户昵称";
        videoInfo.likeCount = 1234;
        videoInfo.playCount = 23456;
        videoInfo.videoTitle="【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoInfo.videoDuration = 10;
        videoInfo.videoUpTime = "9-16 12:28:58";
        VideoBox* videoBox = new VideoBox(videoInfo);
        ui->layout->addWidget(videoBox, i / 4, i % 4);
    }
#endif
}

void MyselfWidget::connectSignalAndSlots()
{
    connect(ui->avatarBtn, &AvatarButton::clicked, this, &MyselfWidget::uploadAvatarBtnClicked);
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this, &MyselfWidget::uploadViewBtnClicked);
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getMyselfInfoDone, this, &MyselfWidget::getMyselfInfoDone);
}

void MyselfWidget::uploadAvatarBtnClicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, "选择头像", "", "Image File(*jpg *.png)");
    if(filename.isNull()) {
        LOG() << "取消选择头像";
        return ;
    }
    QByteArray fileDate = std::move(loadFileToByteArray(filename));
    if(fileDate.isEmpty()) {
        LOG() << "头像文件读取失败";
        return ;
    }
    ui->avatarBtn->setIcon(std::move(makeCircleIcon(fileDate, ui->avatarBtn->width() / 2)));
}

void MyselfWidget::settingBtnClicked()
{
    ModifyMyselfDialog* dialog = new ModifyMyselfDialog();
    dialog->exec();
    delete dialog;
}

void MyselfWidget::getMyselfInfoDone()
{
    // 1. 获取用户数据
    auto dataCenter = model::DataCenter::getInstance();
    const auto* myself = dataCenter->getMyselfInfo();
    // 当前用户可能是普通用户、管理员、临时用户
    if(myself->isTempUser()){
        // 如果是临时用户
        hideWidget(true);
        BiliVideoPlayer* biliPlayer = BiliVideoPlayer::getInstance();
        biliPlayer->showSystemPageBtn(false);
        ui->avatarBtn->setIcon(QIcon(":/image/myself/defaultAvatar.png"));
        ui->avatarBtn->setEnabled(false); // 临时用户不允许修改头像
        ui->nicknameBtn->setText("点击登录");

        ui->nicknameBtn->adjustSize();
        ui->nicknameBtn->setEnabled(true); // 允许点击昵称
        return;
    }else if(myself->isBUser()){
        // 如果是管理员，显示系统按钮，管理员可以后台操作
        BiliVideoPlayer* biliPlayer = BiliVideoPlayer::getInstance();
        biliPlayer->showSystemPageBtn(true);
    }

    // 普通用户和管理员都要恢复界面控件
    hideWidget(false);

    // 2. 界面更新
    // 设置昵称，根据昵称宽度调整设置按钮距离
    ui->nicknameBtn->setText(myself->nickname);
    ui->nicknameBtn->adjustSize();
    ui->nicknameBtn->setEnabled(false);
    // 根据昵称按钮文本长度移动设置按钮，即让设置按钮紧跟在昵称按钮之后
    // 7是：nicknameBtn和settingBtn间的间隔
    QRect rect = ui->nicknameBtn->geometry();
    ui->settingBtn->move(rect.x() + rect.width() + 8, ui->settingBtn->geometry().y());

    // 设置关注数、粉丝数、点赞数、播放数
    ui->attentionCountLabel->setText(intToString2(myself->followedCount));
    ui->fansCountLabel->setText(intToString2(myself->followerCount));
    ui->likeCountLabel->setText(intToString2(myself->likeCount));
    ui->playCountLabel->setText(intToString2(myself->playCount));

    // 3. 设置头像
    // todo

    // 4. 其他：隐藏关注按钮、不能点击登录、允许修改头像
    ui->attentionBtn->hide();
    ui->avatarBtn->setEnabled(true);
    ui->myVideoLabel->setText("我的视频");
}

void MyselfWidget::hideWidget(bool isHide)
{
    // 临时用户需要隐藏界面上控件，非临时用户显示控件
    if(isHide){
        ui->attentionBtn->hide();
        ui->attentionCountLabel->hide();
        ui->attentionLabel->hide();
        ui->fansLabel->hide();
        ui->fansCountLabel->hide();
        ui->likeLabel->hide();
        ui->likeCountLabel->hide();
        ui->playLabel->hide();
        ui->playCountLabel->hide();
        ui->settingBtn->hide();
        ui->quitBtn->hide();
        ui->uploadVideoBtn->hide();

        // scrollArea隐藏后，控件的值仍旧保留
        QSizePolicy sizePolicy = ui->scrollArea->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden(true);
        ui->scrollArea->setSizePolicy(sizePolicy);
        ui->scrollArea->hide();

        sizePolicy = ui->titleBar->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden(true);
        ui->titleBar->setSizePolicy(sizePolicy);
        ui->titleBar->hide();
    }
    else{
        ui->attentionBtn->show();
        ui->attentionCountLabel->show();
        ui->attentionLabel->show();
        ui->fansLabel->show();
        ui->fansCountLabel->show();
        ui->likeLabel->show();
        ui->likeCountLabel->show();
        ui->playLabel->show();
        ui->playCountLabel->show();
        ui->settingBtn->show();
        ui->quitBtn->show();
        ui->uploadVideoBtn->show();
        ui->scrollArea->show();
        ui->titleBar->show();
    }
}
