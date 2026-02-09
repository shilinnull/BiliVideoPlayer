#include "roletableitem.h"
#include "ui_roletableitem.h"

#include "edituserdialog.h"
#include "toast.h"

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
    connect(dataCenter, &model::DataCenter::newAdminDone, this, [=]{
        ui->phoneLabel->setText(this->adminInfo.phone);
        ui->nameLabel->setText(this->adminInfo.nickName);
        // 设置启用和禁用状态
        if(model::enable == this->adminInfo.userStatu) {
            ui->statusButton->setText("禁用");
        } else if(model::disable == this->adminInfo.userStatu){
            ui->statusButton->setText("启用");
        }
        // 修改按钮样式
        ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);
        // 设置备注
        ui->commentLabel->setText(this->adminInfo.remark);
    });
    // 状态按钮点击
    connect(ui->statusButton, &QPushButton::clicked, this, &RoleTableItem::onStatusBtnClicked);
    connect(ui->delBtn, &QPushButton::clicked, this, &RoleTableItem::onDelBtnClicked);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

void RoleTableItem::initStyleSheet()
{
    styleSheet.insert("启用", "border:none;"
                              "background-color:#EBF3FF;"
                              "border-radius:10px;"
                              "border:1px solid #EBEDF0;"
                              "font-size:12px;"
                              "color:#3686FF;");
    styleSheet.insert("禁止", "border:none;"
                              "background-color:#FFF0F0;"
                              "border-radius:10px;"
                              "border:1px solid #EBEDF0;"
                              "font-size:12px;"
                              "color:#FD6A6A;");
    styleSheet.insert("--", "border:none;"
                            "background-color:#FFFFFF;"
                            "border-radius:10px;"
                            "font-size:14px;"
                            "color:#222222;");
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
    ui->phoneLabel->setText(adminInfo.phone);   // 手机号
    ui->nameLabel->setText(adminInfo.nickName); // 用户昵称

    // 状态
    if(model::enable == adminInfo.userStatu){
        ui->statusButton->setText("禁止");
    }else if(model::disable == adminInfo.userStatu){
        ui->statusButton->setText("启用");
    }
    ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);

    // 超级管理员不允许启用或禁用，但保留按钮可点以弹出提示
    ui->commentLabel->setText(adminInfo.remark);
}

bool RoleTableItem::logUserisAdminMyself(const QString &text)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(myself == nullptr) {
        return false;
    }
    if(!adminInfo.userId.isEmpty() && adminInfo.userId == myself->userId) {
        Toast::showMessage(text);
        return true;
    }
    if(!adminInfo.phone.isEmpty() && !myself->phoneNum.isEmpty() && adminInfo.phone == myself->phoneNum) {
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
    adminInfo.phone = ui->phoneLabel->text();
    adminInfo.nickName = ui->nameLabel->text();
    if(ui->statusButton->text() == "启用"){
        adminInfo.userStatu = model::disable;
    }else{
        adminInfo.userStatu = model::enable;
    }
    adminInfo.remark = ui->commentLabel->text();
    // 在编辑页面修改管理员信息
    EditUserDialog* editUserDlg = new EditUserDialog(nullptr, "编辑后台用户", adminInfo);
    editUserDlg->exec();
    // 如果用户点击了确认，向服务器发送编辑用户请求
    if(editUserDlg->getCommitResult()){
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->newAdminAsync(adminInfo);
    }
    delete editUserDlg;
}

void RoleTableItem::onStatusBtnClicked()
{
    if(logUserisAdminMyself("无法启用或者禁用自己")) {
        return ;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin){
        Toast::showMessage("不能启用或禁用超级管理员!!!");
        return;
    }
    QString btnText = ui->statusButton->text();
    if("启用" == btnText){
        btnText = "禁止";
        adminInfo.userStatu = model::enable;
    }else{
        btnText = "启用";
        adminInfo.userStatu = model::disable;
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
    if(logUserisAdminMyself("禁止删除自己")){
        return;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin){
        Toast::showMessage("不能删除超级管理员!!!");
        return;
    }
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->delAdminAsync(adminInfo.userId);
}
