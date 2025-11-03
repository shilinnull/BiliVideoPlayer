#include "bilivideoplayer.h"
#include "startuppage.h"

#include <QApplication>

#include "test/testJson.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 显示启动页面
    // StartupPage startPage;
    // startPage.exec();
    QByteArray str = serialize();
    Deserialize(str);

    BiliVideoPlayer *player = BiliVideoPlayer::getInstance();
    player->show();

    return a.exec();
}
