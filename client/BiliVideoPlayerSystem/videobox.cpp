#include "videobox.h"
#include "ui_videobox.h"
#include "util.h"
#include <QDir>

VideoBox::VideoBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
{
    ui->setupUi(this);
    ui->delVideoBtn->hide();    // 默认是隐藏的

    ui->imageBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);

    playPage = new PlayerPage();
}

VideoBox::~VideoBox()
{
    delete ui;
}

bool VideoBox::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->imageBox == watched || ui->videoTitle == watched) {
        if(QEvent::MouseButtonPress == event->type()) {
            onPlayBtnClicked();
            LOG() << "按下播放按钮";
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void VideoBox::onPlayBtnClicked()
{
    playPage->show();

    // mpv测试
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    QString videoPath = dir.absolutePath();
    videoPath += "/videos/111.mp4";
    playPage->startPlaying(videoPath);
}
