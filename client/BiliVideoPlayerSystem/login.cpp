#include "login.h"
#include "ui_login.h"

#include <QGraphicsDropShadowEffect>
#include <QRegularExpressionValidator>

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
