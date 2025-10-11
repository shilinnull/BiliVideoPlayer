#include "roletableitem.h"
#include "ui_roletableitem.h"

RoleTableItem::RoleTableItem(QWidget *parent, int seqNumber)
    : QWidget(parent)
    , ui(new Ui::RoleTableItem)
{
    ui->setupUi(this);
    updateUI(seqNumber);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

void RoleTableItem::updateUI(int seqNumber)
{
    ui->idLabel->setText(QString::number(seqNumber));
}
