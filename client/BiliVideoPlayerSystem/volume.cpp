#include "volume.h"
#include "ui_volume.h"

#include "util.h"

Volume::Volume(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Volume)
    , volumeRatio(45)   // 默认音量
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
            emit setVolume(volumeRatio);        // 音量调节信号
        } else if(event->type() == QEvent::MouseMove) {
            calcVolume();
            emit setVolume(volumeRatio);        // 音量调节信号
        }
        return true;
    }
    return eventFilter(watched, event);
}

void Volume::calcVolume()
{
    // 获取鼠标点击时的 y 坐标, QCursor().pos()鼠标点击时的位置，该位置是基于屏幕的全局坐标
    int y = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();

    // 验证y的正确性，y的范围在[5, 150]之间
    y = y < 5? 5 : y;
    y = y > 150? 150 : y;

    // 更新outLine的高度
    ui->outLine->setGeometry(ui->outLine->x(), y, ui->outLine->width(), 150 - y);

    // 更新volumeBox的位置
    int volumeBtnY = ui->outLine->y() - ui->volumeBtn->height()/2;
    // 为了防止当outLine的y坐标为150时，按钮的一半显示不出来的问题
    if(volumeBtnY > 140){
        volumeBtnY = 140;
    }
    ui->volumeBtn->move(ui->volumeBtn->x(), volumeBtnY);

    // 计算音量大小
    volumeRatio = ui->outLine->height() / (double)145 * 100;
    LOG()<<"音量大小"<<volumeRatio;
}
