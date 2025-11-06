#include "bilivideoplayer.h"
#include "startuppage.h"

#include <QApplication>

#include "test/TestJson.h"
#include "test/TestHttp.h"
#include "netclient/netclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 显示启动页面
    // StartupPage startPage;
    // startPage.exec();
    // QByteArray str = serialize();
    // Deserialize(str);
    netclient::NetClient httpClient;

    TestHelloRequest(httpClient);
    TestPingRequest(httpClient);

    BiliVideoPlayer *player = BiliVideoPlayer::getInstance();
    player->show();

    return a.exec();
}
