#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "bilivideoplayer.h"
#include "model/datacenter.h"
#include "util.h"

#include <QFileDialog>

UploadVideoPage::UploadVideoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    // 获取所有分类，并且更新到页面
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTag = dataCenter->getKindAndTagsClassPtr();
    ui->kinds->addItems(kindAndTag->getAllKinds());
    ui->kinds->setCurrentIndex(-1);		// 默认设置不选中

    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);
    connect(ui->videoTittle, &QLineEdit::textChanged, this, &UploadVideoPage::onLineEditTextChanged);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &UploadVideoPage::onPlainEditTextChanged);
    connect(ui->changeButton, &QPushButton::clicked, this, &UploadVideoPage::onChangeBtnClicked);
    connect(ui->kinds, &QComboBox::currentTextChanged, this, &UploadVideoPage::onUpDateTags);
}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

void UploadVideoPage::addTagsByKind(const QString &kind)
{
    // 0. 先清空之前的标签
    QList<QPushButton*> tagBtnList = ui->tagWidget->findChildren<QPushButton*>();
    for(auto &tagBtn : tagBtnList) {
        ui->tagLayout->removeWidget(tagBtn);
        delete tagBtn;
    }
    // 将添加的弹簧删除
    QLayoutItem* item = ui->tagLayout->itemAt(ui->tagLayout->count() - 1);
    ui->tagLayout->removeItem(item);

    // 1. 判断是否为空
    if(kind.isEmpty()) {
        return ;
    }

    // 2. 根据kind获取标签
    auto kindAndTagPtr = model::DataCenter::getInstance()->getKindAndTagsClassPtr();
    auto kinds = model::DataCenter::getInstance()->getKindAndTagsClassPtr()->getAllKinds();
    auto tags = kindAndTagPtr->getTagsByKind(kind);

    // 3. 创建对应的QPushButton
    for(auto &tag : tags.keys()) {
        QPushButton* tagBtn = new QPushButton(ui->tagContent);
        tagBtn->setFixedSize(98, 49);
        tagBtn->setText(tag);
        tagBtn->setCheckable(true);		// 设置按钮选中或者未选中状态
        tagBtn->setStyleSheet("QPushButton{"
                              "border: 1px solid #FF6699;"
                              "border-radius: 4px;"
                              "color: #FF6699;"
                              "font-family: 微软雅黑;"
                              "font-size: 16px;"
                              "background-color: #FFFFFF;}"
                              "QPushButton:checked{"
                              "background-color: #FF6699;"
                              "color: #FFFFFF;}"
                              "QPushButton:unchecked{"
                              "background-color: #FFFFFF;"
                              "color: #FF6699;}");
        ui->tagLayout->addWidget(tagBtn);
    }
    // 插入一个空白间距
    ui->tagLayout->insertSpacing(tags.size(), ui->tagContent->width() - (98 + 20) * tags.size());
    ui->tagLayout->setSpacing(20);
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

void UploadVideoPage::onUpDateTags(const QString &kind)
{
    addTagsByKind(kind);
}
