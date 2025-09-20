#include "volume.h"
#include "ui_volume.h"

Volume::Volume(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Volume)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Popup); // 设置为弹出窗口
}

Volume::~Volume()
{
    delete ui;
}
