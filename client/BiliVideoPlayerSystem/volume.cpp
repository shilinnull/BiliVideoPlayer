#include "volume.h"
#include "ui_volume.h"

Volume::Volume(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Volume)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Popup); // 设置为弹出窗口

    ui->volumeBox->installEventFilter(this);
}

Volume::~Volume()
{
    delete ui;
}

bool Volume::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->volumeBox == watched) {
        if(event->type() == QEvent::MouseButtonPress) {
            calcVolume();
        } else if(event->type() == QEvent::MouseButtonRelease) {
            // 鼠标释放 设置音量
        } else if(event->type() == QEvent::MouseMove) {
            calcVolume();
        }
        return true;
    }
    return eventFilter(watched, event);
}

void Volume::calcVolume()
{
    // 处理音量进度条
    int y = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();
    y = y < 5 ? 5 : y;
    y = y > 150 ? 150 : y;
    ui->outLine->setGeometry(ui->outLine->x(), y, ui->outLine->width(), 150 - y);

    // 处理音量按钮
    int volumeBtnY = ui->outLine->y() - ui->volumeBtn->height() / 2;
    volumeBtnY = volumeBtnY > 140 ? 140 : volumeBtnY;           // 处理当按钮拖动到最下面的时候有一半的按钮不显示
    ui->volumeBtn->move(ui->volumeBtn->x(), volumeBtnY);
}
