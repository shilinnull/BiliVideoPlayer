#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <QObject>
#include <atomic>
#include <cstdint>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include <SDL2/SDL.h>

#include "avpacketqueue.h"

/**
 * 音频解码与 SDL 输出
 * 内部维护一个 SDL 音频设备和一个 SWR 重采样器。
 * 从 AvPacketQueue 取包 → avcodec_send_packet / avcodec_receive_frame → swr_convert → SDL 回调输出。
 */
class AudioDecoder : public QObject
{
    Q_OBJECT
public:
    explicit AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

    /// 打开音频解码器，绑定到指定流。成功返回 true。
    bool openAudio(AVFormatContext *fmtCtx, int streamIndex);
    void closeAudio();

    void pauseAudio(bool pause);
    void stopAudio();

    int volume() const { return volume_; }
    void setVolume(int vol);   // 0..SDL_MIX_MAXVOLUME

    bool isMuted() const { return muted_; }
    void setMuted(bool mute);

    /// 设置播放倍速（会关闭并重新打开 SDL 设备和 SWR）
    void setSpeed(double speed);
    double speed() const { return speed_; }

    /// 获取当前音频时钟（秒），用于音视频同步
    double getAudioClock();

    /// 向音频包队列中放入一个包
    void packetEnqueue(AVPacket *pkt);

    /// 标记文件读取完毕，音频解码完队列中的包后会触发 playFinished 信号
    void readFileFinished();

    /// 清空内部缓冲和时钟，用于 seek 后的刷新
    void flushAudio();

    /// 当前刷新序号（每次 flushAudio 递增），用于过滤掉 seek 前残留的信号
    int flushSeq() const { return flushSeq_.load(); }

signals:
    /// 音频包队列已空且文件读取完毕时发射。
    /// 携带 flushSeq，接收方可借此丢弃 seek 之前的过期信号。
    void playFinished(int flushSeq);

private:
    int decodeAudio();
    static void audioCallback(void *userdata, uint8_t *stream, int len);

    bool reopenAudioDevice();

    // ---- 状态 ----
    bool stop_ = false;
    bool pause_ = true;
    bool readFinished_ = false;

    double clock_ = 0.0;
    int volume_ = SDL_MIX_MAXVOLUME;
    bool muted_ = false;
    double speed_ = 1.0;

    // 流 / 解码器
    AVStream *stream_ = nullptr;
    AVCodecContext *codecCtx_ = nullptr;

    // SDL 音频设备
    SDL_AudioDeviceID audioDevice_ = 0;
    SDL_AudioSpec audioSpec_{};
    uint32_t audioDeviceFormat_ = AUDIO_S16SYS;
    uint8_t audioDepth_ = 2;
    AVSampleFormat audioDstFmt_ = AV_SAMPLE_FMT_S16;
    int64_t audioDstChannelLayout_ = 0;

    // SWR 重采样器
    SwrContext *swrCtx_ = nullptr;
    AVSampleFormat audioSrcFmt_ = AV_SAMPLE_FMT_NONE;
    int64_t audioSrcChannelLayout_ = 0;
    int audioSrcFreq_ = 0;

    // 解码缓冲区（av_malloc 分配，保证对齐以便 SWR 使用）
    static constexpr int kAudioBufSize = 192000;
    uint8_t *audioBuf1_ = nullptr;
    uint8_t *audioBuf_ = nullptr;
    uint32_t audioBufSize_ = 0;
    uint32_t audioBufIndex_ = 0;

    // 音频包队列（线程安全，使用 SDL 互斥锁+条件变量）
    AvPacketQueue packetQueue_;
    AVPacket packet_;
    AVPacket seekPacket_;
    int sendReturn_ = 0;

    /// 刷新序号，每次 flushAudio() 递增。用于过滤 seek 前残留的 playFinished 信号
    std::atomic<int> flushSeq_{0};
};

#endif // AUDIODECODER_H
