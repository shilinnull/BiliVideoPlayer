#include "edituserdialog.h"
#include "ui_edituserdialog.h"

#include "bilivideoplayer.h"
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

    // 限制手机号
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phoneEdit->setValidator(validator);

    // 设置管理员角色
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);
    // 如果id不空，说明是编辑用户，否则是新增用户
    if(!adminInfo.userId.isEmpty()){
        ui->phoneEdit->setText(adminInfo.phone);
        // 目前角色只有一个
        if(model::Admin == adminInfo.roleType){
            ui->roleComboBox->setCurrentIndex(0);
        }
        ui->nameEdit->setText(adminInfo.nickName);
        if(model::enable == adminInfo.userStatu){
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

void EditUserDialog::setPhoneEditReadOnly(bool isReadOnly)
{
    ui->phoneEdit->setReadOnly(isReadOnly);
}

void EditUserDialog::onCancelBtnClicked()
{
    close();
}

void EditUserDialog::onSubmitBtnClicked()
{
    adminInfo.phone = ui->phoneEdit->text();
    // 角色只能选择平台管理员
    adminInfo.roleType = model::Admin;
    adminInfo.nickName = ui->nameEdit->text();
    model::AdminStatus adminStatus = model::enable;
    if(!ui->startRadioBtn->isChecked()){
        adminStatus = model::disable;
    }

    // 注意：禁止管理员自己禁用自己
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->userId == adminInfo.userId && adminStatus != adminInfo.userStatu){
        Toast::showMessage("禁止管理员启用或禁用自己！");
        return;
    }
    adminInfo.userStatu = adminStatus;
    adminInfo.remark = ui->commentTextEdit->toPlainText();
    isCommit = true;

    close();
}
