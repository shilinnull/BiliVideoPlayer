#include "myselfwidget.h"
#include "ui_myselfwidget.h"

MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);
}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}
