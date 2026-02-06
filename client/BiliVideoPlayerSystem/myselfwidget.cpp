#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "videobox.h"
#include "modifymyselfdialog.h"
#include "bilivideoplayer.h"
#include "model/datacenter.h"
#include "toast.h"
#include "confirmdialog.h"

#include <QFileDialog>
#include <QScrollBar>


AttentionButton::AttentionButton(QWidget *parent)
    : QPushButton(parent)
{
    changeStatus(false);
}

void AttentionButton::changeStatus(bool isAttention)
{
    isAttentionStatus = isAttention;
    if(isAttentionStatus) {
        setText("已关注");
        setStyleSheet("QPushButton{"
                      "background-color: transparent;"
                      "color: #FF6699;"
                      "border-radius: 4px;"
                      "border: 1px solid #FF6699;"
                      "padding-left: 13px;"
                      "padding-right: 13px;}");
        setIconSize(QSize(24, 24));
        setIcon(QIcon(":/images/myself/guanzhu.png"));
    } else {
        setText("关注");
        setStyleSheet("QPushButton{"
                      "background-color: #FF6699;"
                      "color: #FFFFFF;"
                      "border-radius: 4px;"
                      "border: none;}");
        setIcon(QIcon());
    }
}

bool AttentionButton::isAttentioned() const
{
    return isAttentionStatus;
}

//////////////////////////////////////////////////


MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
    , login(new Login())
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

void MyselfWidget::getOtherUserInfo(const QString &userId)
{
    // 从网络中获取，每次加载其他用户是不同的
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getOtherUserInfoAsync(userId);
}

void MyselfWidget::loadMySelf()
{
    // 加载个人信息
    getMyselfInfo();
    // 加载个人视频列表
    userId = "";
    getUserVideoList("", 1);
}

void MyselfWidget::loadOtherUser(const QString &userId)
{
    // 加载用户信息
    getOtherUserInfo(userId);
    this->userId = userId;
    // 加载用户视频列表
    getUserVideoList(userId, 1);

    // 切换到其他用户模式
    ui->avatarBtn->changeMod(false);
    ui->avatarBtn->setEnabled(false);
}

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

#ifndef TEST_UI
    // 往视频显示区域添加VideoBox-测试
    int resourceId = 10000;
    for(int i = 0; i < 20; ++i){
        model::VideoInfo videoInfo;
        resourceId++;
        videoInfo.userAvatarId = "";
        videoInfo.photoFileId = QString::number(resourceId++);
        videoInfo.videoFileId = QString::number(resourceId++);
        videoInfo.nickName = "用户昵称";
        videoInfo.likeCount = 1234;
        videoInfo.playCount = 23456;
        videoInfo.videoTitle = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoInfo.videoDuration = 10;
        videoInfo.videoUpTime = "9-16 12:28:58";
        VideoBox* videoBox =  new VideoBox(videoInfo);
        ui->layout->addWidget(videoBox, i/4, i%4);
    }
#endif
}

void MyselfWidget::connectSignalAndSlots()
{
    connect(ui->avatarBtn, &AvatarButton::clicked, this, &MyselfWidget::onUploadAvatarBtnClicked);
    connect(ui->nicknameBtn, &AvatarButton::clicked, this, &MyselfWidget::onNicknameBtnClicked);
    connect(ui->quitBtn, &AvatarButton::clicked, this, &MyselfWidget::onQuitBtnClicked);
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this, &MyselfWidget::uploadViewBtnClicked);
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getMyselfInfoDone, this, &MyselfWidget::getMyselfInfoDone, Qt::UniqueConnection);
    connect(dataCenter, &model::DataCenter::getOtherUserInfoDone, this, &MyselfWidget::getOtherUserInfoDone, Qt::UniqueConnection);
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, &MyselfWidget::getAvatarDone);
    connect(dataCenter, &model::DataCenter::uploadPhotoDone, this, &MyselfWidget::uploadAvatarDone1);
    connect(dataCenter, &model::DataCenter::setAvatarDone, this, &MyselfWidget::uploadAvatarDone2);
    connect(dataCenter, &model::DataCenter::getUserVideoListDone, this, &MyselfWidget::getUserVideoListDone);
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MyselfWidget::onSCrollAreaValueChanged);
    connect(dataCenter, &model::DataCenter::deleteVideoDone, this, &MyselfWidget::deleteVideoDone);
    connect(ui->attentionBtn, &AttentionButton::clicked, this, &MyselfWidget::onAttentionBtnClicked);
    connect(dataCenter, &model::DataCenter::newAttentionDone, this, &MyselfWidget::newAttentionDone);
    connect(dataCenter, &model::DataCenter::delAttentionDone, this, &MyselfWidget::delAttentionDone);
    connect(login, &Login::loginSuccess, this, [=]{
        dataCenter->clearUserInfo();    // 清除旧信息
        loadMySelf();                   // 从服务器上重新获取信息
    });
}

void MyselfWidget::onUploadAvatarBtnClicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    const auto* myself = dataCenter->getMyselfInfo();
    if(myself != nullptr && myself->isTempUser()) {
        Toast::showMessage("请先登录，再修改头像！");
        return;
    }
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
    dataCenter->uploadPhotoAsync(fileDate); // 上传图片到服务器
}

void MyselfWidget::onNicknameBtnClicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    auto mySelf = dataCenter->getMyselfInfo();
    if(mySelf->isTempUser()) {
        login->show();
    }
}

void MyselfWidget::onQuitBtnClicked()
{
    ConfirmDialog confirmDlg;
    confirmDlg.setOperatorText("确定退出登录吗？");
    confirmDlg.exec();
    if(confirmDlg.isConfirmPass()) {
        LOG() << "用户选择退出登录";
    }
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
        ui->avatarBtn->setEnabled(true);
        ui->avatarBtn->changeMod(true);
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
    QRect rect = ui->nicknameBtn->geometry();
    ui->settingBtn->move(rect.x() + rect.width() + 8, ui->settingBtn->geometry().y());

    // 设置关注数、粉丝数、点赞数、播放数
    ui->attentionCountLabel->setText(intToString2(myself->followedCount));
    ui->fansCountLabel->setText(intToString2(myself->followerCount));
    ui->likeCountLabel->setText(intToString2(myself->likeCount));
    ui->playCountLabel->setText(intToString2(myself->playCount));

    // 3. 设置头像
    if(myself->avatarFileId.isEmpty()) {
        ui->avatarBtn->setIcon(QIcon(":/images/myself/defaultAvatar.png"));
    } else {
        dataCenter->downloadPhotoAsync(myself->avatarFileId);
    }

    // 4. 其他：隐藏关注按钮、不能点击登录、允许修改头像
    ui->attentionBtn->hide();
    ui->avatarBtn->setEnabled(true);
    ui->avatarBtn->changeMod(true);
    ui->myVideoLabel->setText("我的视频");
}

void MyselfWidget::getOtherUserInfoDone()
{
    // 1. 界面控件显示隐藏处理
    hideWidget(false);
    // 隐藏 设置用户信息按钮、退出登录按钮、上传视频按钮
    ui->settingBtn->hide();
    ui->quitBtn->hide();
    ui->uploadVideoBtn->hide();
    ui->avatarBtn->setEnabled(false);
    ui->nicknameBtn->setEnabled(false);

    // 2. 获取其他用户的个人信息
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();

    // 3. 更新界面数据
    // 设置昵称、关注数、粉丝数、点赞数、播放数
    ui->nicknameBtn->setText(otherUserInfo->nickname);
    ui->attentionCountLabel->setText(intToString2(otherUserInfo->followedCount));
    ui->fansCountLabel->setText(intToString2(otherUserInfo->followerCount));
    ui->likeCountLabel->setText(intToString2(otherUserInfo->likeCount));
    ui->playCountLabel->setText(intToString2(otherUserInfo->playCount));
    ui->attentionBtn->changeStatus(otherUserInfo->isFollowing == 1);
    ui->myVideoLabel->setText("TA的视频");

    // 4. 更新用户头像
    if(otherUserInfo->avatarFileId.isEmpty()){
        ui->avatarBtn->setIcon(QIcon(":/images/myself/defaultAvatar.png"));
    }else{
        dataCenter->downloadPhotoAsync(otherUserInfo->avatarFileId);
    }
}

void MyselfWidget::getAvatarDone(const QString &fileId, const QByteArray &data)
{
    // 下面两个只会走一个

    // 获取自己的头像
    auto* myself = model::DataCenter::getInstance()->getMyselfInfo();
    if(myself != nullptr && myself->avatarFileId == fileId) {
        ui->avatarBtn->setIcon(QIcon(makeCircleIcon(data, ui->avatarBtn->width() / 2)));
    }
    // 获取其他用户的头像
    auto* otherUser = model::DataCenter::getInstance()->getOtherUserInfo();
    if(otherUser != nullptr && otherUser->avatarFileId == fileId) {
        ui->avatarBtn->setIcon(QIcon(makeCircleIcon(data, ui->avatarBtn->width() / 2)));
    }
}

void MyselfWidget::uploadAvatarDone1(const QString &fileId)
{
    // 图片上传成功之后，通过fileId修改服务器上用户头像id
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->setAvatarAsync(fileId);
}

void MyselfWidget::uploadAvatarDone2()
{
    // 重新通过用户头像 fildId 获取头像，头像获取成功会自动设置到界面
    auto dataCenter = model::DataCenter::getInstance();
    const auto* myself = dataCenter->getMyselfInfo();
    dataCenter->downloadPhotoAsync(myself->avatarFileId);
}

void MyselfWidget::getUserVideoListDone(const QString &userId)
{
    auto* dataCenter = model::DataCenter::getInstance();
    auto* userVideoList = dataCenter->getUserVideoList();

    const int rowCount = 4;     // 一行多少个视频
    for(int i = ui->layout->count(); i < userVideoList->getVideoCount(); ++i) {
        int row = i / rowCount;
        int col = i % rowCount;
        VideoBox* videoBox = new VideoBox(userVideoList->videoInfos[i]);
        ui->layout->addWidget(videoBox, row, col);
        if("" == userId){   // 如果是当前用户的获取的视频列表，每个VideoBox都支持删除
            videoBox->showMoreBtn(true);
            connect(videoBox, &VideoBox::deleteVideo, this, &MyselfWidget::deleteVideo);
        }
    }
}

void MyselfWidget::onSCrollAreaValueChanged(int value)
{
    if(0 == value) {
        return;
    }
    if(value == ui->scrollArea->verticalScrollBar()->maximum()) {
        auto dataCenter = model::DataCenter::getInstance();
        auto userVideoListPtr = dataCenter->getUserVideoList();
        dataCenter->getUserVideoListAsync(userId, userVideoListPtr->getPageIndex());
        userVideoListPtr->setPageIndex(userVideoListPtr->getPageIndex() + 1);
    }
}

void MyselfWidget::deleteVideo(const QString &videoId)
{
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->deleteVideoAsync(videoId);
}

void MyselfWidget::deleteVideoDone(const QString &videoId)
{
    LOG() << "删除视频";
    getUserVideoList("", 1);    // 刷新视频列表
}

void MyselfWidget::onAttentionBtnClicked()
{
    // 临时用户不能关注
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isTempUser()) {
        Toast::showMessage("请先登录或注册再进行关注！", this);
        return ;
    }
    // 修改界面
    bool isAttentioned =  ui->attentionBtn->isAttentioned();
    isAttentioned = !isAttentioned;
    ui->attentionBtn->changeStatus(isAttentioned);

    // 关注页面有效的时候才能关注
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    if(nullptr ==  otherUserInfo) {
        Toast::showMessage("指定用户不存在！");
        return ;
    }
    if(isAttentioned) {
        dataCenter->newAttentionAsync(userId);
    } else {
        dataCenter->delAttentionAsync(userId);
    }
}

void MyselfWidget::newAttentionDone(const QString &userId)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    int64_t followCount = otherUserInfo->followerCount + 1;
    ui->fansCountLabel->setText(intToString2(followCount));

    otherUserInfo->followerCount = followCount;
}

void MyselfWidget::delAttentionDone(const QString &userId)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    int64_t followCount = otherUserInfo->followerCount - 1;
    ui->fansCountLabel->setText(intToString2(followCount));

    otherUserInfo->followerCount = followCount;
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

void MyselfWidget::getUserVideoList(const QString &userId, int pageIndex)
{
    //如果获取的是第一页的视频时，需要将之前界面上的视频元素清空
    auto* dataCenter = model::DataCenter::getInstance();
    auto userVideoList = dataCenter->getUserVideoList();
    if(pageIndex == 1) {
        userVideoList->clearVideoList();    // 清空视频列表
        clearVideoList();                   // 删除界面元素
    }
    dataCenter->getUserVideoListAsync(userId, pageIndex);
    // page+1，滚动条向下动时就可以获取下一页视频
    userVideoList->setPageIndex(pageIndex + 1);
}

void MyselfWidget::clearVideoList()
{
    QLayoutItem* videoBoxWidget = nullptr;
    while(nullptr != (videoBoxWidget = ui->layout->takeAt(0))) {
        if (videoBoxWidget->widget() != nullptr) {
            delete videoBoxWidget->widget();
        }
        delete videoBoxWidget;
    }
}

