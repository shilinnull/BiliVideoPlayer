#include "startuppage.h"

StartupPage::StartupPage(QDialog *parent)
    : QDialog{parent}
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    // 窗口大小和图片的大小
    int windowWidth = 1450;
    int windowHeight = 860;
    int imageWidth = 280;
    int imageHeight = 128;

    setFixedSize(windowWidth, windowHeight);
    setStyleSheet("background-color: #FFFFFF");

    // 在窗口显示图片
    QLabel* imageLabel = new QLabel(this);
    imageLabel->setPixmap(QPixmap(":/images/startupPage/bilishipin.png"));

    // 显示中间的图片
    imageLabel->move((windowWidth - imageWidth) / 2, (windowHeight - imageHeight) / 2);

    // 设置定时销毁
    startTimer();
}

void StartupPage::startTimer()
{
    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [this, timer]{
        timer->stop();
        delete timer;
        close();
    });
    timer->start(2000); // 两秒钟后触发

    // 获取信息 TODO
}
