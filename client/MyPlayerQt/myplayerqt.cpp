#include "myplayerqt.h"

#include <QDebug>
#include <QUrl>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>

MyPlayerQt::MyPlayerQt(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(1024, 860);
    // 创建视频渲染窗口对象，将视频渲染到当前窗口上
    videoWidget = new QVideoWidget(this);
    videoWidget->setFixedSize(1024, 800);
    videoWidget->move(0, 0);
    // 创建用户输入视频路径的编辑框
    videoPathEdit = new QLineEdit(this);
    videoPathEdit->setGeometry(5, 820, 800, 20);
    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cdUp();
    QString videoPath = currentDir.path();
    videoPathEdit->setText(videoPath + "/videos/111.mp4");
    // 创建播放按钮
    QPushButton* playBtn = new QPushButton(this);
    playBtn->setGeometry(810, 820, 60, 20);
    playBtn->setText("点击播放");
    // 创建媒体播放对象
    player = new QMediaPlayer(this);
    // 将视频数据输出到videoWidget中
    player->setVideoOutput(videoWidget);
    connect(playBtn, &QPushButton::clicked, this, &MyPlayerQt::onPlayBtnClicked);
}

MyPlayerQt::~MyPlayerQt() {}

void MyPlayerQt::onPlayBtnClicked()
{
    qDebug()<<videoPathEdit->text();
    // 设置视频源
    player->setSource(QUrl::fromLocalFile(videoPathEdit->text()));
    // 播放视频
    player->play();
    player->pause();
}
