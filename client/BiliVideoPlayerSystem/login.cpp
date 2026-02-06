#include "login.h"
#include "ui_login.h"

#include <QGraphicsDropShadowEffect>
#include <QRegularExpressionValidator>
#include "model/datacenter.h"
#include "toast.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    // 去除窗口边框，设置模态
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_ShowModal);

    // 窗口添加阴影效果
    setAttribute(Qt::WA_TranslucentBackground);     // 阴影效果需要设置窗口透明
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setOffset(0);
    ui->loginBg->setGraphicsEffect(shadowEffect);

    // lineEdit限制
    ui->passwordNum->setEchoMode(QLineEdit::Password);
    // 手机编辑框和账号限制
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->accountNum->setValidator(validator);
    ui->phoneNum->setValidator(validator);
    // 创建整数验证器，输入为0~999999
    QIntValidator* intValid = new QIntValidator(0, 999999, ui->authcode);
    ui->authcode->setValidator(intValid);
    ui->authcode->setMaxLength(6);

    // 密码登录和短信登录，默认为密码登录
    ui->messageWidget->show();
    ui->passwordWidget->hide();

    connect(ui->min, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quit, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->passwordBtn, &QPushButton::clicked, this, &Login::onPasswordBtnClicked);
    connect(ui->messageBtn, &QPushButton::clicked, this, &Login::onMessageBtnClicked);
    connect(ui->authcodeBtn, &QPushButton::clicked, this, &Login::onAuthcodeBtnClicked);
    auto dataCenter = model::DataCenter::getInstance();
    // 获取短信验证码
    connect(dataCenter, &model::DataCenter::getAuthcodeDone, [=](const QString& authcodeId){
        this->authcodeId = authcodeId;
    });
    // 注册/登录按钮点击
    connect(ui->loginOrRegister, &QPushButton::clicked, this, &Login::onLoginAndRegisterBtnClicked);


    // 短信登录成功
    connect(dataCenter, &model::DataCenter::loginWithMessageDone, this, [=](){
        emit loginSuccess();
        close();
    });
    // 短信登录失败
    connect(dataCenter, &model::DataCenter::loginWithMessageFailed, this, [=](const QString& errorMsg){
        Toast::showMessage(errorMsg);
    });
    // 短信登录
    connect(ui->registerNow, &QPushButton::clicked, this, &Login::onRegisterNowBtnClicked);


    // 密码登录成功
    connect(dataCenter, &model::DataCenter::loginWithPasswordDone, this, [=](){
        emit loginSuccess();
        close();
    });
    // 密码登录失败
    connect(dataCenter, &model::DataCenter::loginWithPasswordFailed, this, [=](const QString& errorMsg){
        Toast::showMessage(errorMsg);
    });
    // 密码登录
    connect(ui->loginNow, &QPushButton::clicked, this, &Login::onLoginNowBtnClicked);
}

Login::~Login()
{
    delete ui;
}

void Login::onPasswordBtnClicked()
{
    ui->messageWidget->hide();
    ui->passwordWidget->show();
    ui->passwordBtn->setStyleSheet("QPushButton{"
                                   "color : #EC5D85;"
                                   "font-weight:bold;"
                                   "border : none;"
                                   "border-bottom : 6px solid #EC5D85;}");

    ui->messageBtn->setStyleSheet("QPushButton{"
                                  "color : #222222;"
                                  "border : none;"
                                  "border-bottom: 2px solid #EC5D85;}");


}

void Login::onMessageBtnClicked()
{
    ui->messageWidget->show();
    ui->passwordWidget->hide();
    ui->messageBtn->setStyleSheet("QPushButton{"
                                  "color : #EC5D85;"
                                  "font-weight:bold;"
                                  "border : none;"
                                  "border-bottom : 6px solid #EC5D85;}");
    ui->passwordBtn->setStyleSheet("QPushButton{"
                                   "color : #222222;"
                                   "border : none;"
                                   "border-bottom: 2px solid #EC5D85;}");

}

void Login::onAuthcodeBtnClicked()
{
    const QString& phoneNum = ui->phoneNum->text();
    if(!phoneNum.isEmpty()) {
        auto dataCener = model::DataCenter::getInstance();
        dataCener->getAuthcodeAsync(phoneNum);
    } else {
        Toast::showMessage("手机号不能为空！");
    }
}

void Login::onLoginAndRegisterBtnClicked()
{
    const QString& phoneNum = ui->phoneNum->text();
    const QString& authcode = ui->authcode->text();
    if(phoneNum.isEmpty() || authcode.isEmpty()) {
        Toast::showMessage("手机号或验证码不能为空！");
        return ;
    }
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithMessageAsync(phoneNum, authcode, authcodeId);
}

void Login::onRegisterNowBtnClicked()
{
    ui->registerNow->setStyleSheet("#registerNow{"
                                   "color: #FFFFFF;"
                                   "background-color: #FF6699;"
                                   "border-radius: 36px;}");
    ui->loginNow->setStyleSheet("#loginNow{"
                                "color: #222222;"
                                "background-color: #FFFFFF;"
                                "border-radius: 36px;"
                                "border:1px solid #FF6699;}");
    onMessageBtnClicked();  // 切换到注册界面
}

void Login::onLoginNowBtnClicked()
{
    ui->registerNow->setStyleSheet("#registerNow{"
                                   "color: #222222;"
                                   "background-color: #FFFFFF;"
                                   "border-radius: 36px;"
                                   "border:1px solid #FF6699;}");
    ui->loginNow->setStyleSheet("#loginNow{"
                                "color: #FFFFFF;"
                                "background-color: #FF6699;"
                                "border-radius: 36px;}");
    QString accountNum = ui->accountNum->text();
    QString password = ui->passwordNum->text();
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithPasswordAsync(accountNum, password);
}
