#include "newpassworddialog.h"
#include "ui_newpassworddialog.h"

#include <QLineEdit>
#include <QMap>

NewPasswordDialog::NewPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewPasswordDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);

    connect(ui->cancelBtn, &QPushButton::clicked, this, &NewPasswordDialog::close);
    connect(ui->submitBtn, &QPushButton::clicked, this, &NewPasswordDialog::onSubmitBtnClicked);
    connect(ui->passwordEdit1, &QLineEdit::editingFinished, this, &NewPasswordDialog::onEdittingFinished);
    connect(ui->passwordEdit2, &QLineEdit::editingFinished, this, &NewPasswordDialog::onEdittingFinished);
}

NewPasswordDialog::~NewPasswordDialog()
{
    delete ui;
}

QString NewPasswordDialog::getNewPassword() const
{
    return this->newPassword;
}

void NewPasswordDialog::onSubmitBtnClicked()
{
    if(!checkPasswordEdit()) {
        return ;
    }
    this->newPassword = ui->passwordEdit1->text();
    this->close();
}

void NewPasswordDialog::onEdittingFinished()
{
    checkPasswordEdit();
}

bool NewPasswordDialog::checkPasswordEdit()
{
    // 判断是否为空
    if(ui->passwordEdit1->text().isEmpty() || ui->passwordEdit2->text().isEmpty()) {
        ui->messageLabel->setText("密码不能为空");
        return false;
    }
    // 判断第一个密码框
    QString message = passwordValid(ui->passwordEdit1->text());
    if(!message.isEmpty()) {
        ui->messageLabel->setText(message);
        return false;
    }
    message = passwordValid(ui->passwordEdit2->text());
    if(!message.isEmpty()) {
        ui->messageLabel->setText(message);
        return false;
    }
    if(ui->passwordEdit1->text() != ui->passwordEdit2->text()) {
        ui->messageLabel->setText("两次密码输入必须一致");
        return false;
    }
    ui->messageLabel->setText("");
    return true;
}

QString NewPasswordDialog::passwordValid(const QString &password)
{
    if(password.length() < 8) {
        return QString("密码长度不能小于 8 位!");
    }
    if(password.length() > 16) {
        return QString("密码长度不能大于 16 位!");
    }

    enum CharType {
      Upper, Lower, Punct, Digit
    };
    QMap<CharType, int> charTypeMap;
    for(int i = 0; i < password.length(); i++) {
        QChar c = password.at(i);
        if(c.isUpper()) {
            charTypeMap[Upper] = 1;
        } else if(c.isLower()) {
            charTypeMap[Lower] = 1;
        } else if(c.isPunct()) {
            charTypeMap[Punct] = 1;
        } else if(c.isDigit()) {
            charTypeMap[Digit] = 1;
        } else {
            return QString("密码中只能包含数字字母和标点符号!");
        }
    }
    int totalCount = 0;
    for(auto it = charTypeMap.begin(); it != charTypeMap.end(); ++it) {
        totalCount += it.value();
    }
    if(totalCount < 2) {
        return QString("密码中必须包含至少两类字符!");
    }
    return QString();
}
