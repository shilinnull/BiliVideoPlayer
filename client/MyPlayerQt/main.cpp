#include "myplayerqt.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyPlayerQt w;
    w.show();
    return a.exec();
}
