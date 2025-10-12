#include "bilivideoplayer.h"
#include "startuppage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 显示启动页面
    // StartupPage startPage;
    // startPage.exec();

    BiliVideoPlayer *player = BiliVideoPlayer::getInstance();
    player->show();

    return a.exec();
}
