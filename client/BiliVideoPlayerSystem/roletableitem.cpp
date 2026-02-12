#include "roletableitem.h"
#include "ui_roletableitem.h"

#include "edituserdialog.h"
#include "toast.h"
#include "model/datacenter.h"

RoleTableItem::RoleTableItem(QWidget *parent, model::AdminInfo& adminInfo, int seqNumber)
    : QWidget(parent)
    , ui(new Ui::RoleTableItem)
    , adminInfo(adminInfo)
{
    ui->setupUi(this);

    initStyleSheet();       // 初始化样式表
    updateUI(seqNumber);    // 更新界面UI
    connect(ui->editBtn, &QPushButton::clicked, this, &RoleTableItem::onEditBtnClicked);
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::editAdminDone, this, [=](const QString& adminId){
        // 修改当前项的管理员信息时再更新到界面
        if(adminId != adminInfo.userId) {
            return ;
        }
        ui->emailLabel->setText(this->adminInfo.email);
        ui->nameLabel->setText(this->adminInfo.nickName);
        // 设置启用和禁用状态
        if(model::Enable == this->adminInfo.userStatu) {
            ui->statusButton->setText("启用");
        } else if(model::Disable == this->adminInfo.userStatu){
            ui->statusButton->setText("禁用");
        }
        // 修改按钮样式
        ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);
        // 设置备注
        ui->commentLabel->setText(this->adminInfo.remark);

        // 更新dataCenter
        auto& adminLists = dataCenter->getAdminsList()->adminList;
        for(auto& admin : adminLists){
            if(admin.userId == adminId){
                admin.nickName = this->adminInfo.nickName;
                admin.userStatu = this->adminInfo.userStatu;
                admin.remark = this->adminInfo.remark;
                break;
            }
        }
    });
    // 状态按钮点击
    // connect(ui->statusButton, &QPushButton::clicked, this, &RoleTableItem::onStatusBtnClicked);
    connect(ui->delBtn, &QPushButton::clicked, this, &RoleTableItem::onDelBtnClicked);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

void RoleTableItem::initStyleSheet()
{
    styleSheet.insert("启用", "border: none; "
                              "background-color: #EBF3FF; "
                              "border-radius: 10px; "
                              "border: 1px solid #EBEDF0; "
                              "font-size: 12px; "
                              "color: #3686FF;");
    styleSheet.insert("禁用", "border: none; "
                              "background-color: #FFF0F0; "
                              "border-radius: 10px; "
                              "border: 1px solid #EBEDF0; "
                              "font-size: 12px; "
                              "color: #FD6A6A;");
    styleSheet.insert("--", "border: none; "
                            "background-color: #FFFFFF; "
                            "border-radius: 10px; "
                            "font-size: 14px; "
                            "color: #222222;");
}

void RoleTableItem::updateUI(int seqNumber)
{
    if(0 != seqNumber){
        ui->idLabel->setText(QString::number(seqNumber)); // 序号
    }
    // 用户角色
    if(model::Admin == adminInfo.roleType){
        ui->roleLabel->setText("平台管理员");
    }else if(model::SuperAdmin == adminInfo.roleType){
        ui->roleLabel->setText("超级管理员");
        // 超级管理员身份，没有管理编辑和删除操作，只显示一个按钮，点击操作禁用
        ui->delBtn->hide();
        ui->editBtn->move(1155, 25);
        ui->editBtn->setText("--");
        ui->editBtn->setStyleSheet(styleSheet["--"]);
        ui->editBtn->setEnabled(false);
    }
    ui->emailLabel->setText(adminInfo.email);   // 邮箱
    ui->nameLabel->setText(adminInfo.nickName); // 用户昵称

    // 状态
    if(model::Enable == adminInfo.userStatu){
        ui->statusButton->setText("启用");
    }else if(model::Disable == adminInfo.userStatu){
        ui->statusButton->setText("禁用");
    }
    ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);
    ui->statusButton->setEnabled(false);

    // 超级管理员不允许启用或禁用，但保留按钮可点以弹出提示
    ui->commentLabel->setText(adminInfo.remark);
}

bool RoleTableItem::logUserIsAdminMyself(const QString &text)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->userId == adminInfo.userId){
        Toast::showMessage(text);
        return true;
    }

    return false;
}

void RoleTableItem::onEditBtnClicked()
{
    // 提取当前管理员信息
    QString userRole = ui->roleLabel->text();
    if("超级管理员" == userRole){
        adminInfo.roleType = model::SuperAdmin;
    }else if("平台管理员" == userRole){
        adminInfo.roleType = model::Admin;
    }
    adminInfo.email = ui->emailLabel->text();
    adminInfo.nickName = ui->nameLabel->text();
    if(ui->statusButton->text() == "启用"){
        adminInfo.userStatu = model::Enable;
    }else{
        adminInfo.userStatu = model::Disable;
    }
    adminInfo.remark = ui->commentLabel->text();
    // 在编辑页面修改管理员信息
    EditUserDialog* editAdminDlg = new EditUserDialog(nullptr, "编辑后台用户", adminInfo);
    editAdminDlg->setEmailEditReadOnly(true);
    editAdminDlg->exec();
    // 如果用户点击了确认，向服务器发送编辑用户请求
    if(editAdminDlg->getCommitResult()){
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->editAdminAsync(adminInfo);
    }
    delete editAdminDlg;
}

void RoleTableItem::onStatusBtnClicked()
{
    if(logUserIsAdminMyself("无法启用或者禁用自己")) {
        return ;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin){
        Toast::showMessage("不能启用或禁用超级管理员！");
        return;
    }
    QString btnText = ui->statusButton->text();
    if("启用" == btnText){
        btnText = "禁用";
        adminInfo.userStatu = model::Enable;
    }else{
        btnText = "启用";
        adminInfo.userStatu = model::Disable;
    }
    ui->statusButton->setText(btnText);
    ui->statusButton->setStyleSheet(styleSheet[btnText]);

    // 设置状态
    auto dataCenter = model::DataCenter::getInstance();
    auto adminListPtr = dataCenter->getAdminsList();
    adminListPtr->setAdminStatus(adminInfo.userId, adminInfo.userStatu);
    dataCenter->setAdminStatusAsync(adminInfo);
}

void RoleTableItem::onDelBtnClicked()
{
    if(logUserIsAdminMyself("禁止删除自己")){
        return;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin){
        Toast::showMessage("不能删除超级管理员！");
        return;
    }
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->delAdminAsync(adminInfo.userId);
}
