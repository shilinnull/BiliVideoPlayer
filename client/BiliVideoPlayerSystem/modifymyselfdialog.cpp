#include "modifymyselfdialog.h"
#include "ui_modifymyselfdialog.h"

#include <QGraphicsDropShadowEffect>
#include "util.h"
#include "newpassworddialog.h"


ModifyMyselfDialog::ModifyMyselfDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifyMyselfDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    ui->passwordWidget->hide();

    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    connect(ui->submitBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onCancelBtnClicked);
    connect(ui->passwordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);
    connect(ui->changePasswordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);

}

ModifyMyselfDialog::~ModifyMyselfDialog()
{
    delete ui;
}

void ModifyMyselfDialog::onSubmitBtnClicked()
{
    LOG() << "提交";
}

void ModifyMyselfDialog::onCancelBtnClicked()
{
    this->close();
}

void ModifyMyselfDialog::showPasswordDlg()
{
    NewPasswordDialog* dialog = new NewPasswordDialog();
    dialog->exec();

    // 获取修改后的密码
    QString currentPassword = dialog->getNewPassword();
    if(currentPassword.isEmpty()) {
        LOG() << "取消修改密码";
        return ;
    }
    LOG() << "新密码已经设置" << currentPassword;

    ui->passwordBtn->hide();
    ui->passwordWidget->show();

    delete dialog;
}
