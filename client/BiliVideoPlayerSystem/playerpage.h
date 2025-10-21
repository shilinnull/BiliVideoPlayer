#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QMouseEvent>

#include "volume.h"
#include "playspeed.h"
#include "mpv/mpvplayer.h"

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();

    void moveWindows(const QPoint& point);
    void startPlaying(const QString &videoFilePath);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onVolumeBtnClicked();              // 音量
    void onSpeedBtnClicked();               // 倍速
    void onLikeImageBtnClicked();           // 点赞
    void onplayBtnClicked();                // 播放视频
    void onPlaySpeedChanged(double speed);  // 倍速播放

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;

    Volume* volume;                 // 音量调节
    PlaySpeed* playSpeed;           // 播放速度
    MpvPlayer* mpvPlayer = nullptr; // 封装mpv库，控制播放视频
    bool isPlay = false;            // 默认情况下暂停
};

#endif // PLAYERPAGE_H
