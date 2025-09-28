#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "videobox.h"

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

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

    connect(ui->avatarBtn, &AvatarButton::clicked, this, &MyselfWidget::uploadAvatarBtnClicked);

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
