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
    ui->passwordNum->setEchoMode(QLineEdit::Password);

    // 邮箱编辑框和账号限制
    QRegularExpression regExp(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    // [a-zA-Z0-9._%+-]  --->匹配邮箱用户名部分
    // @    --->必须包含一个@符号
    // [a-zA-Z0-9.-]  --->匹配域名部分
    // \.[a-zA-Z]{2,}   --->匹配顶级域名，必须以点开头，后面至少两个字母
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->accountNum->setValidator(validator);
    ui->emailNum->setValidator(validator);
    // 创建整数验证器，输入为0~999999
    QIntValidator* intValid = new QIntValidator(0, 999999, ui->authcode);
    ui->authcode->setValidator(intValid);
    ui->authcode->setMaxLength(6);

    // 密码登录和短信登录，默认为密码登录
    ui->emailWidget->show();
    ui->passwordWidget->hide();

    connect(ui->min, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quit, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->passwordBtn, &QPushButton::clicked, this, &Login::onPasswordBtnClicked);
    connect(ui->emailBtn, &QPushButton::clicked, this, &Login::onEmailBtnClicked);
    connect(ui->authcodeBtn, &QPushButton::clicked, this, &Login::onAuthcodeBtnClicked);
    auto dataCenter = model::DataCenter::getInstance();
    // 获取邮箱验证码
    connect(dataCenter, &model::DataCenter::getAuthcodeDone, this, &Login::onAuthcodeDone);
    // 注册/登录按钮点击
    connect(ui->loginOrRegister, &QPushButton::clicked, this, &Login::onLoginAndRegisterBtnClicked);


    // 邮箱登录成功
    connect(dataCenter, &model::DataCenter::loginWithEmailDone, this, [=](){
        emit loginSuccess();
        close();
    });
    // 邮箱登录失败
    connect(dataCenter, &model::DataCenter::loginWithEmailFailed, this, [=](const QString& errorMsg){
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

void Login::reset()
{
    ui->accountNum->setText("");
    ui->passwordNum->setText("");
    ui->emailNum->setText("");
    ui->authcode->setText("");
}

void Login::onPasswordBtnClicked()
{
    ui->emailWidget->hide();
    ui->passwordWidget->show();
    ui->passwordBtn->setStyleSheet("QPushButton{"
                                   "color : #EC5D85;"
                                   "font-weight:bold;"
                                   "border : none;"
                                   "border-bottom : 6px solid #EC5D85;}");

    ui->emailBtn->setStyleSheet("QPushButton{"
                                  "color : #222222;"
                                  "border : none;"
                                  "border-bottom: 2px solid #EC5D85;}");


}

void Login::onEmailBtnClicked()
{
    ui->emailWidget->show();
    ui->passwordWidget->hide();
    ui->emailBtn->setStyleSheet("QPushButton{"
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
    const QString& email = ui->emailNum->text();
    if(!email.isEmpty()) {
        if(!ui->emailNum->hasSelectedText()) {
            Toast::showMessage("邮箱格式有误！");
            return ;
        }
        auto dataCener = model::DataCenter::getInstance();
        dataCener->getAuthcodeAsync(email);
    } else {
        Toast::showMessage("邮箱不能为空！");
    }
}

void Login::onAuthcodeDone(const QString &authcodeId)
{
    this->authcodeId = authcodeId;
    Toast::showMessage("验证码已经发送！");
}

void Login::onLoginAndRegisterBtnClicked()
{
    const QString& email = ui->emailNum->text();
    const QString& authcode = ui->authcode->text();
    if(email.isEmpty() || authcode.isEmpty()) {
        Toast::showMessage("邮箱或验证码不能为空！");
        return ;
    }

    if(!ui->emailNum->hasAcceptableInput()) {
        Toast::showMessage("邮箱格式有误！");
        return ;
    }

    if(!ui->authcode->hasAcceptableInput()) {
        Toast::showMessage("验证码格式有误！");
        return ;
    }

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithEmailAsync(email, authcode, authcodeId);
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
    onEmailBtnClicked();  // 切换到注册界面
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
    // 获取账户和密码
    QString accountNum = ui->accountNum->text();
    QString password = ui->passwordNum->text();
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithPasswordAsync(accountNum, password);
}
