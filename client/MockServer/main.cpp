#include "server.h"

#include <QApplication>
#include <QWidget>

#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 启动http服务器
    MockServer* httpServer = MockServer::getInstance();
    if(!httpServer->init()) {
        LOG() << "HTTP服务启动失败";
        return -1;
    }
    LOG() << "HTTP服务启动成功";

    QWidget w;
    w.show();

    return a.exec();
}
