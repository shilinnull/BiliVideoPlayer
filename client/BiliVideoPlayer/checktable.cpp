#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QValidator>

#include "checktable.h"
#include "ui_checktable.h"

#include "checktableitem.h"
#include "model/datacenter.h"
#include "toast.h"

CheckTable::CheckTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CheckTable)
{
    ui->setupUi(this);

    ui->videoStatus->addItem("全部分类");
    ui->videoStatus->addItem("待审核");
    ui->videoStatus->addItem("审核通过");
    ui->videoStatus->addItem("审核驳回");
    ui->videoStatus->addItem("已下架");
    ui->videoStatus->setCurrentIndex(0);

    QRegularExpression regExp("^[0-9a-zA-Z]{20}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    // 创建验证器，验证是否符合
    ui->userIdEdit->setValidator(validator);
    ui->userIdEdit->setMaxLength(20);

    // 创建分页器
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();

    // 重置按钮点击信号槽绑定
    connect(ui->resetBtn, &QPushButton::clicked, this, &CheckTable::onResetBtnClicked);
    // 查询按钮点击信号槽绑定
    connect(ui->queryBtn, &QPushButton::clicked, this, &CheckTable::onQueryBtnClicked);

    // 获取用户视频列表
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getUserVideoListDone, this, &CheckTable::updateCheckTable);

    // 获取状态视频列表
    connect(dataCenter, &model::DataCenter::getStatusVideoListDone, this, [=]{
        updateCheckTable("", "checkPage");
    });

}

CheckTable::~CheckTable()
{
    delete ui;
}

void CheckTable::onResetBtnClicked()
{
    ui->resetBtn->setStyleSheet("background-color:#FF6699;"
                                "border-radius:4px;"
                                "font-family:微软雅黑;"
                                "font-size:14px;"
                                "color:#FFFFFF;");
    ui->queryBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅黑;"
                                "font-size:14px;"
                                "color:#222222;");
    // 清空用户id
    ui->userIdEdit->setText("");
    ui->videoStatus->setCurrentIndex(0);

    // 获取用户视频列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable()) {
        Toast::showMessage("您已经被禁止了，无法进行操作");
    } else {
        getVideoList(1);
    }

}

void CheckTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet("background-color:#FF6699;"
                                "border-radius:4px;"
                                "font-family:微软雅黑;"
                                "font-size:14px;"
                                "color:#FFFFFF;");
    ui->resetBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅黑;"
                                "font-size:14px;"
                                "color:#222222;");

    // 获取用户视频列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable()) {
        Toast::showMessage("您已经被禁止了，无法进行操作");
    } else {
        // 点击查询按钮查询时，获取管理员状态，通过分⻚器按钮查询时则无需获取
        videoStatus = static_cast<model::VideoStatus>(ui->videoStatus->currentIndex());
        getVideoList(1);
    }
}

void CheckTable::getVideoList(int page)
{
    this->page = page;
    auto dataCenter = model::DataCenter::getInstance();
    // 系统页面每次只保存一页视频
    auto videoListPtr = dataCenter->getStatusVideoList();
    videoListPtr->videoInfos.clear();
    // 优先按照用户ID获取视频
    QString userId = ui->userIdEdit->text();
    if(!userId.isEmpty()) {
        // 获取指定用户视频列表
        if(!ui->userIdEdit->hasAcceptableInput()){
            Toast::showMessage("上传视频用户id格式错误！");
            return;
        }
        // 获取指定用户视频
        dataCenter->getUserVideoListAsync(userId, page, model::noStatus, "checkPage");
    } else {
        // 获取状态视频列表
        dataCenter->getStatusVideoListAsync(videoStatus, page);
    }
}

void CheckTable::resetPaginator(int pageCount)
{
    // 当重新获取视频列表后，每次获取的结果页面不同，分页器也需要重新设置
    if(!paginator.isNull()) {
        paginator->deleteLater();
        paginator = nullptr;
    }
    paginator = new Paginator(pageCount, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();
    // 重新获取
    connect(paginator, &Paginator::pageChanged, this, [=](int page){
        getVideoList(page);
    });

}

void CheckTable::updateCheckTable(const QString& userId, const QString& whichPage)
{
    if(whichPage != "checkPage") {
        return;
    }
    // 清空旧内容
    QLayoutItem* item = nullptr;
    while(nullptr != (item = ui->layout->takeAt(0))) {
        delete item->widget();
        delete item;
    }

    // 获取用户视频列表
    auto dataCenter = model::DataCenter::getInstance();
    auto statusVideoList = dataCenter->getStatusVideoList();
    if(nullptr == statusVideoList) {
        return ;
    }

    // 重置分页器
    auto videoList = statusVideoList->videoInfos;
    int videoCountOfPage = model::VideoList::PAGE_COUNT;
    if(1 == page) {
        resetPaginator((statusVideoList->getVideoTotalCount() + videoCountOfPage - 1) / videoCountOfPage);
    }

    // 添加视频
    for(int i = 0; i < videoList.size(); i++) {
        CheckTableItem* item = new CheckTableItem(this, videoList[i]);
        ui->layout->addWidget(item);
    }

}
