#include "videobox.h"
#include "ui_videobox.h"

VideoBox::VideoBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
{
    ui->setupUi(this);
}

VideoBox::~VideoBox()
{
    delete ui;
}
