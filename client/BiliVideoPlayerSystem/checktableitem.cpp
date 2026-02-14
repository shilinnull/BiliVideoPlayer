#include "checktableitem.h"
#include "ui_checktableitem.h"
#include "util.h"
#include "confirmdialog.h"

CheckTableItem::CheckTableItem(QWidget *parent, const model::VideoInfo& videoInfo)
    : QWidget(parent)
    , ui(new Ui::CheckTableItem)
    , videoInfo(videoInfo)
{
    ui->setupUi(this);
    // 设置视频用户id、视频用户昵称、视频标题、审核者名称
    ui->videoUserId->setText(videoInfo.userId);
    ui->nickNameLabel->setText(videoInfo.nickName);
    ui->videoTitleLabel->setText(videoInfo.videoTitle);
    // 根据视频状态设置审核者，未审核时审核者为--
    if(model::waitForChecking == videoInfo.videoStatus){
        ui->checkerLabel->setText("--");
    }else{
        ui->checkerLabel->setText(videoInfo.checkerName);
    }

    // 状态和操作按钮
    ui->operationBtn->move(1186, 51);
    ui->operationBtn2->hide();
    // 根据视频状态：设置状态按钮 以及 操作按钮
    switch(videoInfo.videoStatus){
    case model::waitForChecking:{
        ui->statusBtn->setText("待审核");
        ui->operationBtn->setText("通过");
        ui->operationBtn->move(1156, 51);
        ui->operationBtn2->setText("驳回");
        ui->operationBtn2->show();
        break;
    }
    case model::putaway:
    {
        ui->statusBtn->setText("已审核");
        ui->operationBtn->setText("下架");
        break;
    }
    case model::discard:
    {
        ui->statusBtn->setText("已下架");
        ui->operationBtn->setText("上架");
        break;
    }
    case model::reject:
    {
        ui->statusBtn->setText("已驳回");
        ui->operationBtn->setText("--");
        break;
    }
    default:
    {
        LOG()<<"视频状态未知";
    }
    }

    initStyleSheet();
    ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
    ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
    ui->operationBtn2->setStyleSheet(styleSheet[ui->operationBtn2->text()]);
    if("--" == ui->operationBtn->text()) {
        ui->operationBtn->setEnabled(false);
    }

    // 播放视频按钮点击，即用户在视频封面图上点击，封面图是在按钮上
    connect(ui->videoBtn, &QPushButton::clicked, this, &CheckTableItem::onVideoBtnClicked);
    // 操作按钮点击
    connect(ui->operationBtn, &QPushButton::clicked, this, &CheckTableItem::onOperatorBtnClicked);
    // 驳回按钮点击
    connect(ui->operationBtn2, &QPushButton::clicked, this, &CheckTableItem::onOperatorBtn2Clicked);

    // 视频封面图
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, [=](const QString& photoFileId, QByteArray imageData){
        if(photoFileId != videoInfo.photoFileId){
            return;
        }
        // 将视频封面图更新到界面
        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        ui->videoBtn->setIcon(QIcon(pixmap));
    });
    dataCenter->downloadPhotoAsync(videoInfo.photoFileId);

    // 视频封面按钮点击
    connect(ui->videoBtn, &QPushButton::clicked, this, &CheckTableItem::onVideoBtnClicked);
}

CheckTableItem::~CheckTableItem()
{
    delete ui;
}

void CheckTableItem::initStyleSheet()
{
    styleSheet.insert("待审核", "#statusBtn{"
                                "border : none;"
                                "background-color : #FFF0E6;"
                                "font-size : 12px;"
                                "color : #FE964A;"
                                "border-radius : 10px;}");
    styleSheet.insert("通过", "#operationBtn{"
                              "border : none;"
                              "font-size : 14px;"
                              "color : #3686FF}");
    styleSheet.insert("驳回", "#operationBtn2{"
                              "border : none;"
                              "font-size : 14px;"
                              "color : #FD6A6A;}");

    styleSheet.insert("已审核", "#statusBtn{"
                                "border : none;"
                                "background-color : #EBF3FF;"
                                "font-size : 12px;"
                                "color : #3686FF;"
                                "border-radius : 10px;}");
    styleSheet.insert("下架", "#operationBtn{"
                              "border : none;"
                              "font-size : 14px;"
                              "color : #FD6A6A}");

    styleSheet.insert("已下架", "#statusBtn{"
                                "border : none;"
                                "background-color : #FFF0F0;"
                                "font-size : 12px;"
                                "color : #FD6A6A;"
                                "border-radius : 10px;}");
    styleSheet.insert("上架", "#operationBtn{"
                              "border : none;"
                              "font-size : 14px;"
                              "color : #3ECEFF}");

    styleSheet.insert("已驳回", "#statusBtn{"
                                "border : none;"
                                "background-color : #FFF0E6;"
                                "font-size : 12px;"
                                "color : #EF964A;"
                                "border-radius : 10px;}");
    styleSheet.insert("--", "#operationBtn{"
                            "border : none;"
                            "font-size : 14px;"
                            "color : #222222}");
}

void CheckTableItem::onVideoBtnClicked()
{
    playerPage = new PlayerPage(videoInfo);
    if(videoInfo.userAvatarId.isEmpty()){
        // 加载默认头像
        userAvatar.loadFromData(loadFileToByteArray(":/images/myself/defaultAvatar.png"));
        playerPage->setUserIcon(userAvatar);
    } else {
        // 获取用户头像
        auto dataCenter = model::DataCenter::getInstance();
        connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, [=](const QString& imageId, QByteArray imageData){
            if(videoInfo.userAvatarId != imageId) {
                return ;
            }
            userAvatar.loadFromData(imageData);
            playerPage->setUserIcon(userAvatar);
        });
        dataCenter->downloadPhotoAsync(videoInfo.userAvatarId);
    }
    playerPage->show();
    playerPage->startPlaying();
}

void CheckTableItem::onOperatorBtnClicked()
{
    QString btnText = ui->operationBtn->text();
    auto dataCenter = model::DataCenter::getInstance();
    ConfirmDialog* confirmDlg = new ConfirmDialog();
    model::VideoStatus videoStatus = static_cast<model::VideoStatus>(videoInfo.videoStatus);
    if("通过" == btnText){
        confirmDlg->setOperatorText("确定审核通过吗?");
        confirmDlg->exec();
        if(confirmDlg->isConfirmPress()){
            // 视频审核成功，更新界面UI 和 发送视频审核请求
            ui->statusBtn->setText("已审核");
            ui->operationBtn->setText("下架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::putaway;
            dataCenter->checkVideoAsync(videoInfo.videoId, true);
        }
    } else if("上架" == btnText){
        confirmDlg->setOperatorText("确定上架视频吗?");
        confirmDlg->exec();
        if(confirmDlg->isConfirmPress()){
            ui->statusBtn->setText("已审核");
            ui->operationBtn->setText("下架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::putaway;
            dataCenter->putawayVideoAsync(videoInfo.videoId);
        }
    } else if("下架" == btnText){
        confirmDlg->setOperatorText("确定下架视频吗?");
        confirmDlg->exec();
        if(confirmDlg->isConfirmPress()){
            ui->statusBtn->setText("已下架");
            ui->operationBtn->setText("上架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::discard;
            dataCenter->discardVideoAsync(videoInfo.videoId);
        }
    } else if("--" == btnText){
        LOG()<<"视频已经下架，无需进行任何操作";
    } else {
        LOG()<<"未知操作";
    }

    // 修改按钮样式
    ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
    ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
    ui->operationBtn2->setStyleSheet(styleSheet[ui->operationBtn2->text()]);

    // 修改改CheckTableItem对应的视频信息
    auto myselfInfo = dataCenter->getMyselfInfo();
    ui->checkerLabel->setText(myselfInfo->nickname);
    videoInfo.videoStatus = videoStatus;
    videoInfo.checkerId = myselfInfo->userId;
    videoInfo.checkerName = myselfInfo->nickname;
    videoInfo.checkerAvatar = myselfInfo->avatarFileId;
    // 修改DataCenter中的视频审核信息
    auto statusVideoList = dataCenter->getStatusVideoList();
    statusVideoList->updateVideoCheckInfo(videoInfo.videoId, videoStatus, myselfInfo->nickname,
                                          myselfInfo->userId, myselfInfo->avatarFileId);
    delete confirmDlg;
}

void CheckTableItem::onOperatorBtn2Clicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    ConfirmDialog* confirmDlg = new ConfirmDialog();
    confirmDlg->setOperatorText("确认驳回视频吗?");
    confirmDlg->exec();
    if(confirmDlg->isConfirmPress()){
        ui->statusBtn->setText("已驳回");
        ui->operationBtn->setText("--");
        ui->operationBtn->move(1186, 51);
        ui->operationBtn2->hide();
        ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
        ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
        ui->operationBtn2->setStyleSheet(styleSheet[ui->operationBtn2->text()]);
        auto myselfInfo = dataCenter->getMyselfInfo();
        ui->checkerLabel->setText(myselfInfo->nickname);
        // 修改改CheckTableItem对应的视频信息
        ui->checkerLabel->setText(myselfInfo->nickname);
        videoInfo.videoStatus = model::VideoStatus::reject;
        videoInfo.checkerId = myselfInfo->userId;
        videoInfo.checkerName = myselfInfo->nickname;
        videoInfo.checkerAvatar = myselfInfo->avatarFileId;
        // 修改DataCenter中的视频审核信息
        auto statusVideoList = dataCenter->getStatusVideoList();
        statusVideoList->updateVideoCheckInfo(videoInfo.videoId, model::VideoStatus::reject,
                                              myselfInfo->nickname, myselfInfo->userId,
                                              myselfInfo->avatarFileId);
        dataCenter->checkVideoAsync(videoInfo.videoId, false);
    }
    delete confirmDlg;
}
