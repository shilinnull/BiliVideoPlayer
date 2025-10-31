#include "videobox.h"
#include "ui_videobox.h"
#include "util.h"
#include <QDir>

PlayerPage* VideoBox::playPage = nullptr;

VideoBox::VideoBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
{
    ui->setupUi(this);
    ui->delVideoBtn->hide();    // 默认是隐藏的

    ui->imageBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);
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
    // 如果已经有一个播放器窗口存在，则先销毁它
    if(playPage)
    {
        // The connected lambda will set playPage to nullptr upon destruction.
        delete playPage;
    }

    // 创建一个新的播放器实例
    playPage = new PlayerPage();
    // 当窗口被销毁（例如用户关闭）时，将静态指针重置为nullptr
    connect(playPage, &PlayerPage::destroyed, this, [=](){
        playPage = nullptr;
    });
    
    playPage->show();

    // mpv测试
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    QString videoPath = dir.absolutePath();
    videoPath += "/videos/111.mp4";
    playPage->startPlaying(videoPath);
}
