#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QValidator>

#include "roletable.h"
#include "ui_roletable.h"

#include "edituserdialog.h"
#include "model/datacenter.h"
#include "roletableitem.h"
#include "toast.h"

RoleTable::RoleTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoleTable)
{
    ui->setupUi(this);

    ui->userStatus->addItem("全部分类");
    ui->userStatus->addItem("启用");
    ui->userStatus->addItem("禁用");
    ui->userStatus->setCurrentIndex(0);

    // 水平布局器中的元素向上对齐，否则如果只有一个元素会垂直居中
    ui->layout->setAlignment(Qt::AlignTop);

    // 创建分页器
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 20);
    paginator->show();

    // 限制编辑框只能输入邮箱
    QRegularExpression regExp(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->email->setValidator(validator);

    connect(ui->resetBtn, &QPushButton::clicked, this, &RoleTable::onResetBtnClicked);
    connect(ui->queryBtn, &QPushButton::clicked, this, &RoleTable::onQueryBtnClicked);
    connect(ui->insertBtn, &QPushButton::clicked, this, &RoleTable::onInsertBtnClicked);

    // 通过邮箱或者管理员的状态信息进行获取管理员的信息
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getAdminByEmailDone, this, &RoleTable::updateAdminListUI);
    connect(dataCenter, &model::DataCenter::getAdminListByStatusDone, this, &RoleTable::updateAdminListUI);
    // 新增管理员
    connect(dataCenter, &model::DataCenter::newAdminDone, this, [=] {
        getAdminList(1);    // 新增管理员成功，重新获取下管理员列表
    });

    // 删除管理员成功
    connect(dataCenter, &model::DataCenter::delAdminDone, this, [=]{
        // 重新获取管理员列表，界面和DataCenter中数据都可以更新
        getAdminList(1);
    });

    initStyleSheet();
}

RoleTable::~RoleTable()
{
    delete ui;
}

void RoleTable::onResetBtnClicked()
{
    // 设置按钮样式
    ui->resetBtn->setStyleSheet(styleSheet["选中"]);
    ui->queryBtn->setStyleSheet(styleSheet["未选中"]);
    ui->email->setText("");
    ui->userStatus->setCurrentIndex(0);

    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable()) {
        Toast::showMessage("您已被禁止，无法进行操作！");
    } else {
        getAdminList(1);
    }
}

void RoleTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet(styleSheet["选中"]);
    ui->resetBtn->setStyleSheet(styleSheet["未选中"]);

    // 获取管理员列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable()){
        Toast::showMessage("您已被禁止，无法进行操作");
    }else{
        getAdminList(1);
    }
}

void RoleTable::updateAdminListUI()
{
    // 清空旧视频内容
    QLayoutItem* item = nullptr;
    while((item = ui->layout->takeAt(0)) != nullptr){
        delete item->widget();
        delete item;
    }

    // 管理员信息显示到界面
    auto dataCenter = model::DataCenter::getInstance();
    auto adminInfoList = dataCenter->getAdminsList();
    if(nullptr == adminInfoList){
        return;
    }

    // 重置分页器
    auto adminList = adminInfoList->adminList;
    int adminCountOfPage = model::AdminList::PAGE_COUNT;
    if(1 == page) {
        resetPaginator((adminInfoList->totalCount + adminCountOfPage - 1) / adminCountOfPage);
    }

    // 管理员信息显示到界面
    int seqNum = (page - 1) * adminCountOfPage + 1;
        int maxCount = adminList.size();
        int remaining = adminInfoList->totalCount - (page - 1) * adminCountOfPage;
        if(remaining < 0) {
            remaining = 0;
        }
        if(remaining < maxCount) {
            maxCount = remaining;
        }
        for (int i = 0; i < maxCount; i++) {
        RoleTableItem* item = new RoleTableItem(this, adminList[i], seqNum++);
        ui->layout->addWidget(item);
    }
}

void RoleTable::initStyleSheet()
{
    styleSheet.insert("选中", "background-color:#FF6699;"
                              "border-radius:4px;"
                              "font-family:微软雅黑;"
                              "font-size:14px;"
                              "color:#FFFFFF;");
    styleSheet.insert("未选中", "background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅黑;"
                                "font-size:14px;"
                                "color:#222222;");
}

void RoleTable::onInsertBtnClicked()
{
    // 显示新增后台用户对话框
    model::AdminInfo adminInfo;
    EditUserDialog* editAdminDlg = new EditUserDialog(nullptr, "新增后台用户", adminInfo);
    editAdminDlg->setEmailEditReadOnly(false);
    editAdminDlg->exec();
    // 如果用户确实点击了确认按钮，向服务器发送新增用户请求
    if(editAdminDlg->getCommitResult()){
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->newAdminAsync(adminInfo);
    }
    delete editAdminDlg;
}

void RoleTable::getAdminList(int page)
{
    this->page = page;
    auto dataCenter = model::DataCenter::getInstance();
    auto adminListPtr = dataCenter->getAdminsList();
    adminListPtr->adminList.clear();        // 先将旧的数据进行清空
    // 优先按邮箱获取
    const QString email = ui->email->text();
    if(!email.isEmpty()) {
        // 通过邮箱获取管理员信息
        if(!ui->email->hasAcceptableInput()) {
            Toast::showMessage("输入邮箱有错，请重新输入！");
            return ;
        }
        dataCenter->getAdminByEmailAsync(email);
    } else {
        // 通过状态获取管理员信息
        model::AdminStatus videoStatus = static_cast<model::AdminStatus>(ui->userStatus->currentIndex());
        dataCenter->getAdminListByStatusAsync(page, videoStatus);
    }
}

void RoleTable::resetPaginator(int pageCount)
{
    if(paginator) {
        delete paginator;
    }
    paginator = new Paginator(pageCount, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();

    connect(paginator, &Paginator::pageChanged, this, [=](int page){
        getAdminList(page);
    });
}
