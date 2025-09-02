#include "bilivideoplayer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BiliVideoPlayer w;
    w.show();
    return a.exec();
}
