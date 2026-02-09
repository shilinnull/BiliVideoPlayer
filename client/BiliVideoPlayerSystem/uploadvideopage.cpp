#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "bilivideoplayer.h"
#include "model/datacenter.h"
#include "util.h"
#include "toast.h"

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
    // 默认情况下，上传视频成功图片隐藏，视频上传成功后显⽰
    ui->downIcon->hide();

    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);
    connect(ui->videoTittle, &QLineEdit::textChanged, this, &UploadVideoPage::onLineEditTextChanged);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &UploadVideoPage::onPlainEditTextChanged);
    connect(ui->changeButton, &QPushButton::clicked, this, &UploadVideoPage::onChangeBtnClicked);
    connect(ui->kinds, &QComboBox::currentTextChanged, this, &UploadVideoPage::onUpdateTags);
    connect(dataCenter, &model::DataCenter::uploadVideoDone, this, &UploadVideoPage::onUploadVideoDone);
    connect(dataCenter, &model::DataCenter::uploadPhotoDone, this, [=](const QString& coverImageId, QWidget* wndPtr){
        if(wndPtr != ui->imageLabel) {
            return ;
        }
        this->coverImageId = coverImageId;
        isUploadPhotoOk = true;     // 上传图片成功

    });
    connect(dataCenter, &model::DataCenter::uploadVideoDescDone, this, [=]{
        resetPage();        // 清空上传视频页面
        emit switchMySelfPage(MyselfPage);
    });
}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

void UploadVideoPage::setVideoTitle(const QString &videoFilePath)
{
    resetPage();
    this->videoFilePath = videoFilePath;

    // 截图文件名设置到界面
    int start = videoFilePath.lastIndexOf("/") + 1;
    QString videoTitle = videoFilePath.mid(start);
    ui->videoTittle->setText(videoTitle);
    ui->fileName->setText(videoTitle);
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
    if(!isUploadVideoOk) {
        Toast::showMessage("等待视频上传完成！");
        return ;
    }
    if(!isUploadPhotoOk) {
        Toast::showMessage("等待封面图上传完成！");
        return ;
    }
    if(!isDurationOk) {
        Toast::showMessage("等待视频总时长完成！");
        return ;
    }
    // 只有上面完成任务才能继续后续的上传视频
    if(isUploadVideoOk && isUploadPhotoOk && isDurationOk) {
        model::VideoDesc videoDesc;
        videoDesc.videoId = videoId;
        videoDesc.photoId = coverImageId;
        videoDesc.title = ui->videoTittle->text();
        videoDesc.desc = ui->plainTextEdit->toPlainText();
        videoDesc.kind = ui->kinds->currentText();
        videoDesc.duration = duration;

        // 获取视频标签
        QList<QPushButton*> tagBtns = ui->tagContent->findChildren<QPushButton*>();
        for(auto& tagBtn : tagBtns){
            if(tagBtn->isChecked()){
                QString tag = tagBtn->text();
                videoDesc.tags.append(tag);
            }
        }

        if(videoDesc.tags.size() > 5){
            Toast::showMessage("最多只能选择5个标签");
            return;
        }

        // 新增视频信息到服务器
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->uploadVideoDescAsync(videoDesc);
    }
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

    QPixmap pixmap;
    if(!pixmap.load(coverImagePath) || pixmap.isNull()){
        LOG()<<"封面图加载失败:"<<coverImagePath;
        return;
    }

    // 上传服务器
    uploadPhoto(coverImagePath);

    pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->imageLabel->setPixmap(pixmap);

    repaint();
}

void UploadVideoPage::onUpdateTags(const QString &kind)
{
    addTagsByKind(kind);
}

void UploadVideoPage::onUploadVideoDone(const QString &videoId)
{
    ui->uploadProgress->setText("上传完成");
    ui->downIcon->show();
    this->videoId = videoId;
    isUploadVideoOk = true;
    // 设置视频首帧
    QString firstFrame= MpvPlayer::getVideoFirstFrame(videoFilePath);
    QPixmap pixmap(firstFrame);
    pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ui->imageLabel->setPixmap(pixmap);
    // 上传到服务器
    uploadPhoto(firstFrame);
    // 然后删除图片
    QFile::remove(firstFrame);

    // 获取视频总时长
    mpvPlayer = new MpvPlayer();
    mpvPlayer->startPlay(videoFilePath);
    mpvPlayer->pause();
    connect(mpvPlayer, &MpvPlayer::durationChanged, this, &UploadVideoPage::getDurationDone);
}

void UploadVideoPage::uploadPhoto(const QString &photoPath)
{
    QByteArray fileData = loadFileToByteArray(photoPath);
    if(fileData.isEmpty()) {
        LOG() << "读取视频封面图片失败";
        return ;
    }
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->uploadPhotoAsync(fileData, ui->imageLabel);
}

void UploadVideoPage::resetPage()
{
    ui->imageLabel->setStyleSheet("#imageLabel{"
                                  "border-image: url(:/images/uploadVideoPage/fenmian.png);}");
    ui->videoTittle->setText("");
    ui->plainTextEdit->setPlainText("");
    ui->uploadProgress->setText("上传中");
    ui->downIcon->hide();

    addTagsByKind("");
    isUploadPhotoOk = false;
    isUploadVideoOk = false;
    isDurationOk = false;
    videoId = "";
    coverImageId = "";
    duration = 0;

    // 默认不选中
    ui->kinds->setCurrentIndex(-1);
}

void UploadVideoPage::getDurationDone(int64_t duration)
{
    this->duration = duration;
    LOG() << "要上传的视频总时长为：" << duration;
    this->isDurationOk = true;
    delete mpvPlayer;
    mpvPlayer = nullptr;
}
