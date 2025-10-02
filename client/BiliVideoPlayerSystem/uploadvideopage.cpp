#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "bilivideoplayer.h"

UploadVideoPage::UploadVideoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);
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
