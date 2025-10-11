#include "checktableitem.h"
#include "ui_checktableitem.h"

CheckTableItem::CheckTableItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CheckTableItem)
{
    ui->setupUi(this);
}

CheckTableItem::~CheckTableItem()
{
    delete ui;
}
