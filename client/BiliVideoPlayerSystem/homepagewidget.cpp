#include "homepagewidget.h"
#include "ui_homepagewidget.h"

homePageWidget::homePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::homePageWidget)
{
    ui->setupUi(this);
}

homePageWidget::~homePageWidget()
{
    delete ui;
}
