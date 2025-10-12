#include "roletableitem.h"
#include "ui_roletableitem.h"

#include "edituserdialog.h"

RoleTableItem::RoleTableItem(QWidget *parent, int seqNumber)
    : QWidget(parent)
    , ui(new Ui::RoleTableItem)
{
    ui->setupUi(this);
    updateUI(seqNumber);

    connect(ui->editBtn, &QPushButton::clicked, this, &RoleTableItem::onEditBtnClicked);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

void RoleTableItem::updateUI(int seqNumber)
{
    ui->idLabel->setText(QString::number(seqNumber));
}

void RoleTableItem::onEditBtnClicked()
{
    EditUserDialog* editUserDlg = new EditUserDialog(nullptr, "编辑后台用户");
    editUserDlg->exec();
    delete editUserDlg;
}
