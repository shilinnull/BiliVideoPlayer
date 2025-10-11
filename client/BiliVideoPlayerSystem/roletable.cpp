#include "roletable.h"
#include "ui_roletable.h"

#include "util.h"
#include "roletableitem.h"

RoleTable::RoleTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoleTable)
{
    ui->setupUi(this);

    ui->userStatus->addItem("全部分类");
    ui->userStatus->addItem("启用");
    ui->userStatus->addItem("停用");
    ui->userStatus->setCurrentIndex(0);

    // 限制编辑框只能输入手机号
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phone->setValidator(validator);

    connect(ui->resetBtn, &QPushButton::clicked, this, &RoleTable::onResetBtnClicked);
    connect(ui->queryBtn, &QPushButton::clicked, this, &RoleTable::onQueryBtnClicked);

    initStyleSheet();
    updateRoleTable();  // 更新表格元素
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
    ui->phone->setText("");
    ui->userStatus->setCurrentIndex(0);
    LOG() << "重置按钮点击...";
}

void RoleTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet(styleSheet["选中"]);
    ui->resetBtn->setStyleSheet(styleSheet["未选中"]);
    LOG() << "查询按钮点击...";
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

void RoleTable::updateRoleTable()
{
    for(int i = 0; i < 11; i++) {
        RoleTableItem* item = new RoleTableItem(this, i + 1);
        ui->layout->addWidget(item);
    }
}
