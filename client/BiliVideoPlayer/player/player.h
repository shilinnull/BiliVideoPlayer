#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

#include <QImage>
#include <QObject>
#include <memory>

class QWidget;
class QString;
class QTimer;

class AudioDecoder;
struct FFmpegPlayerState;

class FFmpegPlayer : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegPlayer(QWidget* videoRenderWnd = nullptr, QObject *parent = nullptr);
    ~FFmpegPlayer();

    /// 开始播放视频（支持本地文件路径和 HLS/m3u8 URL）
    void startPlay(const QString& videoPath);

    /// 播放 / 暂停
    void play();
    void pause();

    /// 设置播放倍速
    void setPlaySpeed(double speed);

    /// 设置音量（0-100），内部映射到 SDL_MIX_MAXVOLUME
    void setVolume(int64_t volume);

    /// 跳转到指定时间位置（单位：秒）
    void setCurrentPlayPositon(int64_t seconds);

    /// 获取当前播放时间（秒）
    int64_t getPlayTime()const;

    /// 是否正在播放
    bool isPlaying() const;

    /// 停止播放并等待线程退出
    void stopPlayback();

    /// 获取视频第一帧作为封面（使用 ffmpeg 命令行工具）
    static QString getVideoFirstFrame(const QString& videoPath);

signals:
    /// 视频帧就绪（由视频线程通过 QueuedConnection 发射到主线程）
    void frameReady(QImage image);

    /// 播放位置变化通知（每 500ms 发射一次）
    void playPositionChanged(int64_t seconds);

    /// 所有视频分片播放结束
    void endOfPlaylist();

    /// 视频总时长（秒）
    void durationChanged(int64_t duration);

private:
    void onPositionTick();
    void emitDuration(int64_t duration);

    std::unique_ptr<FFmpegPlayerState> state;
    AudioDecoder* audioDecoder = nullptr;
    QTimer* positionTimer = nullptr;
    int64_t curPlayTime = 0;
    int64_t durationSec = 0;
};

#endif // FFMPEGPLAYER_H
