#ifndef MYPLAYERQT_H
#define MYPLAYERQT_H

#include <QWidget>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLineEdit>

class MyPlayerQt : public QWidget
{
    Q_OBJECT

public:
    MyPlayerQt(QWidget *parent = nullptr);
    ~MyPlayerQt();

private slots:
    void onPlayBtnClicked();

private:
    QMediaPlayer *player;       // 用于播放音频视频
    QVideoWidget *videoWidget;  // 渲染视频

    QLineEdit *videoPathEdit;   // 输入视频路径

};
#endif // MYPLAYERQT_H
