#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "bilivideoplayer.h"

#include <QFileDialog>

UploadVideoPage::UploadVideoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);
    connect(ui->videoTittle, &QLineEdit::textChanged, this, &UploadVideoPage::onLineEditTextChanged);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &UploadVideoPage::onPlainEditTextChanged);
    connect(ui->changeButton, &QPushButton::clicked, this, &UploadVideoPage::onChangeBtnClicked);
}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

void UploadVideoPage::onCommitBtnClicked()
{
    // 视频上传成功，切换到我的页面
    emit switchMySelfPage(MyselfPage);
}

void UploadVideoPage::onLineEditTextChanged(const QString &text)
{
    // 先要从wordLeft中获取最大的输入字数
    QString wordLleftText = ui->leftWord->text();
    // 6/80
    int linePos = wordLleftText.indexOf('/');
    int totalCount = wordLleftText.mid(linePos + 1).toInt();

    // 检测text中的字数是否超过wordLeft，如果没有超过，将text的长度和最大字数拼接起来
    int wordCount = text.length();
    if(wordCount <= totalCount){
        ui->leftWord->setText(QString::number(wordCount) + "/" + QString::number(totalCount));
    }else{
        LOG()<<"视频标题超过80个字";
        ui->videoTittle->setText(text.mid(0, totalCount));
    }
}

void UploadVideoPage::onPlainEditTextChanged()
{
    // 获取已经输入的文本
    QString text = ui->plainTextEdit->toPlainText();
    int wordCount = text.size();

    // 获取最大字数
    QString leftWord = ui->briefLeftWord->text();
    int linePos = leftWord.indexOf('/');
    QString totalCount = leftWord.mid(linePos + 1);

    // 检测实际输入文本长度是否超过最大长度
    if(wordCount <= totalCount.toInt()) {
        ui->briefLeftWord->setText(QString::number(wordCount) + "/" + totalCount);
    } else {
        ui->plainTextEdit->setPlainText(text.mid(0, totalCount.toInt()));
        // 设置滚动条一直在底部
        QTextCursor textCursor = ui->plainTextEdit->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); // 移动到最后
        ui->plainTextEdit->setTextCursor(textCursor);
    }
}

void UploadVideoPage::onChangeBtnClicked()
{
    QString coverImagePath = QFileDialog::getOpenFileName(nullptr, "选择视频封面", "", "Images (*.png *.xpm *.jpg)");
    if(coverImagePath.isEmpty()){
        LOG()<<"用户取消封面图的选择";
        return;
    }

    QPixmap pixmap(coverImagePath);
    pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->imageLabel->setPixmap(pixmap);

    repaint();
}
