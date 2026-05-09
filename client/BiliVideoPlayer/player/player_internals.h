#ifndef PLAYER_INTERNALS_H
#define PLAYER_INTERNALS_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <SDL2/SDL.h>

class FFmpegPlayer;

/// 位置定时器间隔（毫秒）
constexpr int kPositionUpdateMs = 500;

/// 音视频不同步阈值（秒），超过此值则丢弃视频帧
constexpr double kFrameDropThresholdSec = 0.2;

/// 解码后的视频帧，携带 PTS 时间戳
struct VideoFrame {
    AVFrame* frame = nullptr;
    double ptsSec = 0.0;           // 帧的显示时间（秒）
};

/// 从 AVFrame 中提取 PTS（秒），优先使用 best_effort_timestamp
inline double framePtsSec(const AVFrame* frame, AVRational timeBase)
{
    if(frame == nullptr) {
        return 0.0;
    }
    int64_t pts = frame->best_effort_timestamp;
    if(pts == AV_NOPTS_VALUE) {
        pts = frame->pts;
    }
    if(pts == AV_NOPTS_VALUE) {
        return 0.0;
    }
    return pts * av_q2d(timeBase);
}

/// 播放器内部状态，包含所有控制标志、队列和线程
struct FFmpegPlayerState {
    explicit FFmpegPlayerState(FFmpegPlayer* /*owner*/) {}

    // ---- FFmpeg 流相关 ----
    AVFormatContext* formatCtx = nullptr;   // 解复用上下文
    AVCodecContext* videoCtx = nullptr;     // 视频解码器上下文
    SwsContext* swsCtx = nullptr;           // 颜色空间转换（YUV→RGB32）
    int videoStreamIndex = -1;              // 视频流索引
    int audioStreamIndex = -1;              // 音频流索引
    AVRational videoTimeBase{};             // 视频流时间基

    // ---- 线程 ----
    std::thread decodeThread;               // 解复用+视频解码线程
    std::thread videoThread;                // 视频渲染+音视频同步线程

    // ---- 视频帧队列 ----
    std::mutex videoMutex;
    std::condition_variable videoCond;
    std::deque<VideoFrame> videoQueue;

    // ---- 控制标志 ----
    std::atomic<bool> stop{false};                     // 请求停止
    std::atomic<bool> paused{true};                    // 暂停中
    std::atomic<bool> seekRequest{false};              // 请求跳转
    std::atomic<int64_t> seekTarget{0};                // 跳转目标时间（秒）
    std::atomic<bool> endEmitted{false};               // 是否已发射结束信号
    std::atomic<bool> renderedFirstFrame{false};       // 是否已渲染第一帧
    std::atomic<bool> decodeError{false};              // 解码出错
    std::atomic<bool> frameDecoded{false};             // 至少解码了一帧
    std::atomic<double> audioClock{0.0};               // 音频时钟（秒），用于音视频同步
    std::atomic<int> framesSincePlay{0};               // 点击播放后的帧数计数（用于预热期放松同步）

    std::string sourceUrl;                             // 视频源地址

    /// 重置所有时钟
    void resetClocks()
    {
        audioClock.store(0.0);
    }
};

/**
 * FFmpeg I/O 中断回调
 * 当 stop 标志置位时返回 1，从而中断 av_read_frame() 等阻塞操作，
 * 使 stopPlayback() 能够迅速退出线程。
 */
inline int interruptCb(void* opaque)
{
    auto* st = static_cast<FFmpegPlayerState*>(opaque);
    return st->stop.load() ? 1 : 0;
}

#endif // PLAYER_INTERNALS_H
