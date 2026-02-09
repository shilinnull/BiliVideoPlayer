#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QMouseEvent>
#include <QDialog>
#include <QFrame>

#include "volume.h"
#include "playspeed.h"
#include "mpv/mpvplayer.h"
#include "model/datacenter.h"
#include "model/data.h"
#include "login.h"

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(const model::VideoInfo& videoInfo, QWidget *parent = nullptr);
    ~PlayerPage();

    void moveWindows(const QPoint& point);
    void startPlaying();
    void buildBulletScreenData();                   // 加载弹幕数据
    void showBulletScreen();						// 显示弹幕
    void setUserIcon(const QPixmap& userPixmap);          // 设置用户头像

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onVolumeBtnClicked();                      // 音量
    void onSpeedBtnClicked();                       // 倍速
    void onLikeImageBtnClicked();                   // 点赞
    void onplayBtnClicked();                        // 播放视频
    void onPlaySpeedChanged(double speed);          // 倍速播放
    void setVolume(int volumeRatio);                // 音量调整
    void onPlayPositionChanged(int64_t playTime);   // 播放位置改变
    void onEndOfPlayList();                         // 所有视频分片播放结束
    void onSetPlayProgress(double playRatio);		// 设置播放进度
    void onBulletScreenClicked();					// 弹幕开关控制
    void onSendBulletScreenBtnClicked(const QString& text);// 发送弹幕

private:
    QString secondToTime(int64_t second) const;           // 转换时间
    void initBarrageArea();							// 弹幕区域布局
    void updateVideoInfoUI();                       // 设置视频信息
    void updataPlayCount();                         // 更新播放数
    void onQuitBtnClicked();                        // 退出
    void onUserAvatarClicked();                     // 用户头像点击
private:
    Ui::PlayerPage *ui;
    QPoint dragPos;
    Volume* volume;                 // 音量调节
    PlaySpeed* playSpeed;           // 播放速度
    Login* login;                   // 登录窗口
    MpvPlayer* mpvPlayer = nullptr; // 封装mpv库，控制播放视频
    bool isPlay = false;            // 默认情况下暂停
    model::VideoInfo videoInfo;     // 保存视频信息
    bool isUpdatePlayNum = false;   // 是否更新播放次数
    bool isLike = false;            // 检测是否被点赞过，默认没有
    int64_t likeCount = 0;          // 标记点赞数

    // 弹幕相关信息
    QDialog* barrageArea;
    QFrame* top;
    QFrame* middle;
    QFrame* bottom;
    bool isStartBS = true;
    QHash<int64_t, QList<model::BarrageInfo>> bulletScreens;    // 获取当前播放下的所有数据
signals:
    void increasePlayCount(const QString& videoId);
    void updateLikeNum(int64_t likeCount);
};

#endif // PLAYERPAGE_H
