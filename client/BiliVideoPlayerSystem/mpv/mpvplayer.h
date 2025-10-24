#ifndef MPVPLAYER_H
#define MPVPLAYER_H

#include <QObject>
#include "./mpv/client.h"

class MpvPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MpvPlayer(QWidget* videoRenderWnd = nullptr, QObject *parent = nullptr);
    ~MpvPlayer();

    // 处理mpv具体的事件
    void handleMpvEvent(mpv_event* event);

    // 播放视频
    void startPlay(const QString& videoPath);

    void play();
    void pause();

    // 倍速播放
    void setPlaySpeed(double speed);

    // 静音功能
    void setMute(bool isMute);

    // 音量调节
    void setVolume(int64_t volume);

    // seek功能
    void setCurrentPlayPositon(int64_t seconds);

    // 获取整个视频的当前播放时间
    int64_t getPlayTime()const;

    // 获取视频首帧图
    static QString getVideoFirstFrame(const QString& videoPath);

private slots:
    void onMpvEvents();

signals:
    // 当mpv触发事件时，在回调函数中发射该信号，由用户程序处理mpv的事件
    void mpvEvents();

    // 通知界面更新当前播放时间
    void playPositionChanged(int64_t seconds);

    // 整个视频播放结束信号
    void endOfPlaylist();

    // 总时长变化
    void durationChanged(int64_t duration);

private:
    mpv_handle* mpv = nullptr;
    // 保存整个视频的当前播放时间
    int64_t curPlayTime = 0;
};

#endif // MPVPLAYER_H
