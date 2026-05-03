#include "bilivideoplayer.h"
#include "startuppage.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));    // 不使用系统的默认样式，使用 Fusion 样式来创建元素

    // 防止程序启动多个客户端
    QSharedMemory sharedMemory("BiliPlayer");
    if(sharedMemory.attach()) {
        QMessageBox::information(nullptr, "BiliPlayer提示", "BiliVideoPlayer已经在运行了...");
        return 0;
    }
    sharedMemory.create(1);

    // 显示启动页面
    StartupPage startPage;
    startPage.exec();

    // 显示主界面
    BiliVideoPlayer *player = BiliVideoPlayer::getInstance();
    player->show();

    return a.exec();
}
