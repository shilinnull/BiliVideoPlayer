#include "modifymyselfdialog.h"
#include "ui_modifymyselfdialog.h"

#include <QIcon>

#include "model/datacenter.h"
#include "newpassworddialog.h"
#include "util.h"
#include "toast.h"
#include "bilivideoplayer.h"

ModifyMyselfDialog::ModifyMyselfDialog(bool isModifyPassword, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifyMyselfDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    // 如果已经修改过密码，则不显示修改密码，显示已经修改过的密码
    if(isModifyPassword) {
        ui->passwordWidget->show();
        ui->passwordBtn->hide();
    } else {
        ui->passwordWidget->hide();
        ui->passwordBtn->show();
    }

    // 显示到主窗口的位置上
    BiliVideoPlayer* biliplayer = BiliVideoPlayer::getInstance();
    QPoint point = biliplayer->mapToGlobal(QPoint(0, 0));
    point.setX(point.x() + (biliplayer->width() - width()) / 2);
    point.setY(point.y() + (biliplayer->height() - height()) / 2);
    this->move(point);

    // 加载个人数据到界面上
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    ui->emailNumberLabel->setText(hideEmail(myself->email));
    ui->nicknameEdit->setText(myself->nickname);
    ui->nicknameEdit->setMaxLength(16); // 昵称最长是16个字符

    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    connect(ui->submitBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onCancelBtnClicked);
    connect(ui->passwordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);
    connect(ui->changePasswordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);
    connect(dataCenter, &model::DataCenter::setPasswordDone, this, [=] {
        LOG() << "修改密码成功";
    });
    connect(dataCenter, &model::DataCenter::setNicknameFailed, this, [this](const QString& reason){
        Toast::showMessage(reason);
    });
}

ModifyMyselfDialog::~ModifyMyselfDialog()
{
    delete ui;
}

void ModifyMyselfDialog::onSubmitBtnClicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    if(!newPassword.isEmpty()){
        dataCenter->setPasswordAsync(newPassword);
    }
    // 获取新昵称
    auto myself = dataCenter->getMyselfInfo();
    const QString newNickName = ui->nicknameEdit->text().trimmed();
    if(newNickName.size() > 16) {
        Toast::showMessage("昵称最多16个字符！");
        return ;
    }
    if(newNickName != myself->nickname) {
        // 发送设置昵称的请求
        dataCenter->setNickNameAsync(newNickName);
    }
    close();
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
        delete dialog;
        return ;
    }
    newPassword = currentPassword;      // 新密码
    LOG() << "新密码已经设置" << newPassword;

    ui->passwordBtn->hide();
    ui->passwordWidget->show();

    delete dialog;
}
