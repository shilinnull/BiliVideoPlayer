#include "edituserdialog.h"
#include "ui_edituserdialog.h"

#include <QPoint>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTextCursor>

#include "bilivideoplayer.h"
#include "model/datacenter.h"
#include "toast.h"

EditUserDialog::EditUserDialog(QWidget *parent, const QString& text, model::AdminInfo& adminInfo)
    : QDialog(parent)
    , ui(new Ui::EditUserDialog)
    , adminInfo(adminInfo)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->titleLabel->setText(text);

    // 和窗口对齐
    BiliVideoPlayer* player = BiliVideoPlayer::getInstance();
    this->move(player->mapToGlobal(QPoint(0, 0)));

    // 限制邮箱
    QRegularExpression regExp(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->emailEdit->setValidator(validator);

    // 设置管理员角色
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);
    // 如果id不空，说明是编辑用户，否则是新增用户
    if(!adminInfo.userId.isEmpty()){
        ui->emailEdit->setText(adminInfo.email);
        // 目前角色只有一个
        if(model::Admin == adminInfo.roleType){
            ui->roleComboBox->setCurrentIndex(0);
        }

        ui->nameEdit->setText(adminInfo.nickName);
        if(model::Enable == adminInfo.userStatu){
            ui->startRadioBtn->setChecked(true);
        }else{
            ui->stopRadioBtn->setChecked(true);
        }
        ui->commentTextEdit->setPlainText(adminInfo.remark);
    }


    connect(ui->cancelBtn, &QPushButton::clicked, this, &EditUserDialog::onCancelBtnClicked);
    connect(ui->submitBtn, &QPushButton::clicked, this, &EditUserDialog::onSubmitBtnClicked);
    connect(ui->commentTextEdit, &QPlainTextEdit::textChanged, this, [=]{   // 编辑框内容发生变化
        QString text = ui->commentTextEdit->toPlainText();
        int wordCount = text.size();
        if(wordCount <= 10){
            ui->wordCount->setText(QString::number(wordCount) + "/10");
        } else {
            ui->commentTextEdit->setPlainText(text.mid(0, 10));
        }
        // 设置光标
        QTextCursor textCursor = ui->commentTextEdit->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); // 移动到最后
        ui->commentTextEdit->setTextCursor(textCursor);
    });
}

EditUserDialog::~EditUserDialog()
{
    delete ui;
}

bool EditUserDialog::getCommitResult() const
{
    return isCommit;
}

void EditUserDialog::setEmailEditReadOnly(bool isReadOnly)
{
    ui->emailEdit->setReadOnly(isReadOnly);
}

void EditUserDialog::onCancelBtnClicked()
{
    close();
}

void EditUserDialog::onSubmitBtnClicked()
{
    if(!ui->emailEdit->hasAcceptableInput()) {
        Toast::showMessage("管理员邮箱格式有误！");
        return ;
    }
    const model::AdminStatus oldStatus = adminInfo.userStatu;
    model::AdminStatus adminStatus = model::Enable;
    if(!ui->startRadioBtn->isChecked()) {
        adminStatus = model::Disable;
    }

    // 禁止管理员自己禁用自己
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->userId == adminInfo.userId && adminStatus != oldStatus){
        Toast::showMessage("禁止管理员启用或禁用自己！");
        return;
    }

    adminInfo.userStatu = adminStatus;
    adminInfo.email = ui->emailEdit->text();

    // 角色只能选择平台管理员
    adminInfo.roleType = model::Admin;
    adminInfo.nickName = ui->nameEdit->text();
    adminInfo.remark = ui->commentTextEdit->toPlainText();
    isCommit = true;

    close();
}
