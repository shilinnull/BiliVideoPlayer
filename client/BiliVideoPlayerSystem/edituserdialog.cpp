#include "edituserdialog.h"
#include "ui_edituserdialog.h"

#include "bilivideoplayer.h"

EditUserDialog::EditUserDialog(QWidget *parent, const QString& text)
    : QDialog(parent)
    , ui(new Ui::EditUserDialog)
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

    // 设置用户角色
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);

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

void EditUserDialog::onCancelBtnClicked()
{
    close();
}

void EditUserDialog::onSubmitBtnClicked()
{
    LOG() << "点击了提交按钮";
    close();
}
