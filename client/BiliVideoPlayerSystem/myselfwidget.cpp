#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "videobox.h"
#include "modifymyselfdialog.h"
#include "bilivideoplayer.h"

#include <QFileDialog>

MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);
    initUI();
}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}

void MyselfWidget::uploadViewBtnClicked()
{
    // 1. 打开文件对话框，让用户选择要上传的文件
    QString videoFilePath = QFileDialog::getOpenFileName(nullptr, "上传视频", "", "Videos (*.mp4 *.rmvb *.avi *.mov)");
    if(!videoFilePath.isEmpty()) {
        // 视频限制大小 4G
        QFileInfo fileInfo(videoFilePath);
        int64_t fileSize = fileInfo.size();
        if(fileSize > 4 * 1024 * 1024) {
            LOG() << "视频文件必须小于4G";
            return ;
        }
        emit switchUploadVideoPage(UploadPage);
    }
}

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

    connect(ui->avatarBtn, &AvatarButton::clicked, this, &MyselfWidget::uploadAvatarBtnClicked);
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this, &MyselfWidget::uploadViewBtnClicked);

#ifdef TEST_UI
    for(int i = 0; i < 16; i++) {
        VideoBox* videoBox = new VideoBox();
        ui->layout->addWidget(videoBox, i / 4, i % 4);
    }
#endif
}

void MyselfWidget::uploadAvatarBtnClicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, "选择头像", "", "Image File(*jpg *.png)");
    if(filename.isNull()) {
        LOG() << "取消选择头像";
        return ;
    }
    QByteArray fileDate = std::move(loadFileToByteArray(filename));
    if(fileDate.isEmpty()) {
        LOG() << "头像文件读取失败";
        return ;
    }
    ui->avatarBtn->setIcon(std::move(makeCircleIcon(fileDate, ui->avatarBtn->width() / 2)));
}

void MyselfWidget::settingBtnClicked()
{
    ModifyMyselfDialog* dialog = new ModifyMyselfDialog();
    dialog->exec();
    delete dialog;
}
