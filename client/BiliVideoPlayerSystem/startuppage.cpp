#include "startuppage.h"
#include "./model/datacenter.h"
#include "util.h"

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

    // 绑定信号和槽临时用户登录成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::loginTempUserDone, this, [=]{
        loginSuccess = true;
        // 临时用户在服务器上没有信息，自己本地构建一个
        dataCenter->buildTempUserInfo();
    });
}

void StartupPage::startTimer()
{
    QTimer* timer = new QTimer(this);
	timer->setSingleShot(true);		// 设置为周期定时
    connect(timer, &QTimer::timeout, this, [this]{
        if(!loginSuccess) {
            LOG() << "临时登录未完成，继续启动客户端";
        }
        close();
    });
    timer->start(2000); // 两秒钟后触发

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginTempUserAsync();
}
