#include "startuppage.h"

#include <QLabel>
#include <QPixmap>
#include <QTimer>

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

    // 临时用户登录成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::loginTempUserDone, this, [=]{
        loginSuccess = true;
        // 临时用户在服务器上没有信息，自己本地构建一个
        dataCenter->buildTempUserInfo();
        mySelfInfoSuccess = true;       // 构建临时用户成功
    });
    // session登录成功
    connect(dataCenter, &model::DataCenter::loginSessionDone, this, [=](bool isTemper){
        loginSuccess = true;
        // 如果是临时用户身份就构造临时用户身份
        if(isTemper) {
            dataCenter->buildTempUserInfo();
            mySelfInfoSuccess = true;
        } else {
            dataCenter->getMyselfInfoAsync();
        }
    });
    // session登录失败
    connect(dataCenter, &model::DataCenter::loginSessionFailed, this, [=](const QString& errorString){
        LOG() << "session登录失败，启动临时用户身份进行登录，失败原因: " << errorString;
        dataCenter->loginTempUserAsync();
    });
    // 获取个人信息完成
    connect(dataCenter, &model::DataCenter::getMyselfInfoDone, this, [=](){
        mySelfInfoSuccess = true;
    });
}

void StartupPage::startTimer()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]{
        if(loginSuccess && mySelfInfoSuccess) {
            timer->stop();
            delete timer;
            close();
        }
    });
    timer->start(2000); // 两秒钟后触发

    // 如果sessionId为空，说明没有登录过
    // 如果sessionId为空，但用户角色为临时用户，仍然使用临时身份登录
    auto dataCenter = model::DataCenter::getInstance();
    QString sessionId = dataCenter->getLoginSessionId();
    if(sessionId.isEmpty() || dataCenter->getMyselfInfo()->isTempUser()) {
        dataCenter->loginTempUserAsync();       // 临时登录，从服务器获取sessionId
        LOG() << "临时用户登录";
    } else { // 有session时走会话登录
        dataCenter->setSessionId(sessionId);    // 确认使用本地sessionId
        dataCenter->loginSessionAsync();
        LOG() << "会话用户登录";
    }
}
