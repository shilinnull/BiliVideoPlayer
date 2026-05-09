#include "audiodecoder.h"

#include <cstring>

#include <QDebug>

/* SDL 音频缓冲区最小大小（样本数） */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* 每秒最大回调次数，用于推算合理的缓冲区大小 */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

AudioDecoder::AudioDecoder(QObject *parent)
    : QObject(parent)
{
    audioBuf1_ = static_cast<uint8_t *>(av_malloc(kAudioBufSize));
    av_init_packet(&seekPacket_);
    seekPacket_.data = reinterpret_cast<uint8_t *>(const_cast<char *>("FLUSH"));
}

AudioDecoder::~AudioDecoder()
{
    closeAudio();
    av_free(audioBuf1_);
}

bool AudioDecoder::openAudio(AVFormatContext *fmtCtx, int index)
{
    if(index < 0 || static_cast<unsigned>(index) >= fmtCtx->nb_streams) {
        return false;
    }

    stop_ = false;
    readFinished_ = false;
    audioSrcFmt_ = AV_SAMPLE_FMT_NONE;
    audioSrcChannelLayout_ = 0;
    audioSrcFreq_ = 0;
    clock_ = 0.0;
    sendReturn_ = 0;
    audioBuf_ = nullptr;
    audioBufSize_ = 0;
    audioBufIndex_ = 0;

    fmtCtx->streams[index]->discard = AVDISCARD_DEFAULT;
    stream_ = fmtCtx->streams[index];

    // 分配并配置解码器上下文
    codecCtx_ = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codecCtx_, stream_->codecpar);

    const AVCodec *codec = avcodec_find_decoder(codecCtx_->codec_id);
    if(!codec) {
        qDebug() << "AudioDecoder: 未找到对应解码器，id=" << codecCtx_->codec_id;
        avcodec_free_context(&codecCtx_);
        return false;
    }

    if(avcodec_open2(codecCtx_, codec, nullptr) < 0) {
        qDebug() << "AudioDecoder: avcodec_open2 失败";
        avcodec_free_context(&codecCtx_);
        return false;
    }

    return reopenAudioDevice();
}

bool AudioDecoder::reopenAudioDevice()
{
    if(audioDevice_ != 0) {
        SDL_CloseAudioDevice(audioDevice_);
        audioDevice_ = 0;
    }
    if(swrCtx_) {
        swr_free(&swrCtx_);
    }
    audioBuf_ = nullptr;
    audioBufSize_ = 0;
    audioBufIndex_ = 0;
    clock_ = 0.0;

    if(!codecCtx_) {
        return false;
    }

    // -- 确定输出声道布局 --
    int wantedChannels = codecCtx_->channels;
    if(wantedChannels <= 0) {
        wantedChannels = 2;
    }
    audioDstChannelLayout_ = av_get_default_channel_layout(wantedChannels);
    if(!audioDstChannelLayout_) {
        audioDstChannelLayout_ = av_get_default_channel_layout(2);
    }
    audioDstChannelLayout_ &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;

    const int outChannels = av_get_channel_layout_nb_channels(audioDstChannelLayout_);

    // 根据倍速调整采样率
    int sampleRate = static_cast<int>(codecCtx_->sample_rate * speed_);
    if(sampleRate <= 0) {
        sampleRate = codecCtx_->sample_rate;
    }

    // -- 打开 SDL 音频设备 --
    // 如果第一次尝试失败，依次尝试不同的声道/采样率组合
    int nextSampleRates[] = {0, 44100, 48000, 96000, 192000};
    int nextSampleRateIdx = 4;
    while(nextSampleRateIdx && nextSampleRates[nextSampleRateIdx] >= sampleRate) {
        --nextSampleRateIdx;
    }

    int nextNbChannels[] = {0, 0, 1, 6, 2, 6, 4, 6};

    SDL_AudioSpec wanted{};
    wanted.freq = sampleRate;
    wanted.format = audioDeviceFormat_;
    wanted.channels = static_cast<Uint8>(outChannels);
    wanted.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE,
                           2 << av_log2(sampleRate / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted.callback = audioCallback;
    wanted.userdata = this;

    while(true) {
        audioDevice_ = SDL_OpenAudioDevice(nullptr, 0, &wanted, &audioSpec_, 0);
        if(audioDevice_ == 0) {
            qDebug() << "AudioDecoder: SDL_OpenAudioDevice 失败 ("
                     << wanted.channels << "ch," << wanted.freq << "Hz):" << SDL_GetError();
            // 尝试下一个声道数
            wanted.channels = nextNbChannels[FFMIN(7, wanted.channels)];
            if(!wanted.channels) {
                wanted.freq = nextSampleRates[nextSampleRateIdx--];
                wanted.channels = static_cast<Uint8>(outChannels);
                if(!wanted.freq) {
                    qDebug() << "AudioDecoder: 没有找到可用的 SDL 音频配置";
                    avcodec_free_context(&codecCtx_);
                    return false;
                }
            }
            audioDstChannelLayout_ = av_get_default_channel_layout(wanted.channels);
            continue;
        }

        // 如果 SDL 改变了格式，用新格式重试
        if(audioSpec_.format != audioDeviceFormat_) {
            qDebug() << "AudioDecoder: SDL 将格式从" << audioDeviceFormat_
                     << "改为" << audioSpec_.format;
            audioDeviceFormat_ = audioSpec_.format;
            SDL_CloseAudioDevice(audioDevice_);
            audioDevice_ = 0;
            wanted.format = audioDeviceFormat_;
            continue;
        }
        break;
    }

    if(audioSpec_.channels != wanted.channels) {
        audioDstChannelLayout_ = av_get_default_channel_layout(audioSpec_.channels);
    }

    // 将 SDL 格式映射为 AVSampleFormat
    switch(audioDeviceFormat_) {
    case AUDIO_U8:
        audioDstFmt_ = AV_SAMPLE_FMT_U8;
        audioDepth_ = 1;
        break;
    case AUDIO_S16SYS:
        audioDstFmt_ = AV_SAMPLE_FMT_S16;
        audioDepth_ = 2;
        break;
    case AUDIO_S32SYS:
        audioDstFmt_ = AV_SAMPLE_FMT_S32;
        audioDepth_ = 4;
        break;
    case AUDIO_F32SYS:
        audioDstFmt_ = AV_SAMPLE_FMT_FLT;
        audioDepth_ = 4;
        break;
    default:
        audioDstFmt_ = AV_SAMPLE_FMT_S16;
        audioDepth_ = 2;
        break;
    }

    // -- 初始化 SWR（音频重采样器） --
    int64_t inLayout = codecCtx_->channel_layout;
    if(inLayout == 0) {
        inLayout = av_get_default_channel_layout(codecCtx_->channels);
    }

    swrCtx_ = swr_alloc_set_opts(nullptr,
                                  audioDstChannelLayout_, audioDstFmt_, audioSpec_.freq,
                                  inLayout, codecCtx_->sample_fmt, codecCtx_->sample_rate,
                                  0, nullptr);
    if(!swrCtx_ || swr_init(swrCtx_) < 0) {
        qDebug() << "AudioDecoder: swr_init 失败";
        if(swrCtx_) {
            swr_free(&swrCtx_);
        }
        SDL_CloseAudioDevice(audioDevice_);
        audioDevice_ = 0;
        return false;
    }

    // 保存当前输入格式以便检测后续格式变化
    audioSrcFmt_ = codecCtx_->sample_fmt;
    audioSrcChannelLayout_ = inLayout;
    audioSrcFreq_ = codecCtx_->sample_rate;

    // 开始播放
    SDL_PauseAudioDevice(audioDevice_, 0);

    return true;
}

void AudioDecoder::closeAudio()
{
    stop_ = true;

    if(audioDevice_ != 0) {
        SDL_CloseAudioDevice(audioDevice_);
        audioDevice_ = 0;
    }

    if(codecCtx_) {
        avcodec_free_context(&codecCtx_);
    }
    if(swrCtx_) {
        swr_free(&swrCtx_);
    }

    packetQueue_.empty();
    clock_ = 0.0;
}

void AudioDecoder::pauseAudio(bool pause)
{
    pause_ = pause;
}

void AudioDecoder::stopAudio()
{
    stop_ = true;
}

void AudioDecoder::setVolume(int vol)
{
    if(vol < 0) vol = 0;
    if(vol > SDL_MIX_MAXVOLUME) vol = SDL_MIX_MAXVOLUME;
    volume_ = vol;
}

void AudioDecoder::setMuted(bool mute)
{
    muted_ = mute;
}

void AudioDecoder::setSpeed(double speed)
{
    if(speed <= 0.0) return;
    if(qFuzzyCompare(speed, speed_)) return;
    speed_ = speed;
    if(!reopenAudioDevice()) {
        qDebug() << "AudioDecoder: 重新打开音频设备失败，倍速" << speed;
        speed_ = 1.0;
    }
}

double AudioDecoder::getAudioClock()
{
    if(codecCtx_) {
        int hwBufSize = audioBufSize_ - audioBufIndex_;
        int bytesPerSec = codecCtx_->sample_rate * codecCtx_->channels * audioDepth_;
        if(bytesPerSec > 0) {
            clock_ -= static_cast<double>(hwBufSize) / bytesPerSec;
        }
    }
    return clock_;
}

/**
 * 刷新音频状态，用于 seek 后清空所有缓冲。
 *
 * 注意：这里故意不使用 SDL_LockAudioDevice，否则可能死锁：
 * SDL 回调可能正阻塞在 decodeAudio() → dequeue(true) → SDL_CondWait 中，
 * 如果这边 LockAudioDevice，那边 SDL_CondWait 又持有音频设备锁，就会相互等待。
 *
 * 不加锁的情况下，audioBuf_/audioBufSize_/clock_ 等成员在 x86_64 上是
 * 简单类型，读写天然原子，最坏情况：回调看到一次过时的值（约 30ms 的静音或旧帧）。
 */
void AudioDecoder::flushAudio()
{
    audioBuf_ = nullptr;
    audioBufSize_ = 0;
    audioBufIndex_ = 0;
    clock_ = 0.0;
    sendReturn_ = 0;
    stop_ = false;
    readFinished_ = false;
    flushSeq_.fetch_add(1);

    packetQueue_.empty();
    // 放入 FLUSH 包，告诉 decodeAudio() 调用 avcodec_flush_buffers
    packetQueue_.enqueue(&seekPacket_);
}

void AudioDecoder::packetEnqueue(AVPacket *pkt)
{
    packetQueue_.enqueue(pkt);
}

void AudioDecoder::readFileFinished()
{
    readFinished_ = true;
}

// ---- SDL 音频回调（静态函数，由 SDL 音频线程调用） ----

void AudioDecoder::audioCallback(void *userdata, uint8_t *stream, int len)
{
    auto *decoder = static_cast<AudioDecoder *>(userdata);
    if(!decoder) {
        SDL_memset(stream, 0, len);
        return;
    }

    while(len > 0) {
        if(decoder->stop_) {
            SDL_memset(stream, 0, len);
            return;
        }

        if(decoder->pause_) {
            SDL_memset(stream, 0, len);
            return;
        }

        // 需要更多解码数据？
        if(decoder->audioBufIndex_ >= decoder->audioBufSize_) {
            int decoded = decoder->decodeAudio();
            if(decoded < 0) {
                decoder->audioBufSize_ = 1024;
                decoder->audioBuf_ = nullptr;
            } else {
                decoder->audioBufSize_ = static_cast<uint32_t>(decoded);
            }
            decoder->audioBufIndex_ = 0;
        }

        int left = decoder->audioBufSize_ - decoder->audioBufIndex_;
        if(left > len) {
            left = len;
        }

        if(decoder->audioBuf_ && !decoder->muted_) {
            SDL_memset(stream, 0, left);
            SDL_MixAudioFormat(stream,
                                decoder->audioBuf_ + decoder->audioBufIndex_,
                                decoder->audioDeviceFormat_, static_cast<Uint32>(left),
                                decoder->volume_);
        } else {
            SDL_memset(stream, 0, left);
        }

        len -= left;
        stream += left;
        decoder->audioBufIndex_ += left;
    }
}

// ---- 解码一帧音频（在 SDL 回调内部调用） ----

int AudioDecoder::decodeAudio()
{
    AVFrame *frame = av_frame_alloc();
    if(!frame) {
        return -1;
    }

    if(stop_) {
        av_frame_free(&frame);
        return -1;
    }

    // 检查队列——如果为空且文件已读完，播放结束
    if(packetQueue_.queueSize() <= 0) {
        if(readFinished_) {
            stop_ = true;
            emit playFinished(flushSeq_.load());
        }
        av_frame_free(&frame);
        return -1;
    }

    // 取一个新包（如果上一个已消费完）
    // 阻塞式出队是安全的：flushAudio() 不再持有 SDL 音频锁，不会死锁
    if(sendReturn_ != AVERROR(EAGAIN)) {
        packetQueue_.dequeue(&packet_, true);
    }

    // 处理 FLUSH 包（seek 刷新）
    if(packet_.data && !strcmp(reinterpret_cast<const char *>(packet_.data), "FLUSH")) {
        avcodec_flush_buffers(codecCtx_);
        av_packet_unref(&packet_);
        av_frame_free(&frame);
        sendReturn_ = 0;
        return -1;
    }

    // 发送包到解码器
    sendReturn_ = avcodec_send_packet(codecCtx_, &packet_);
    if(sendReturn_ < 0 && sendReturn_ != AVERROR(EAGAIN) && sendReturn_ != AVERROR_EOF) {
        av_packet_unref(&packet_);
        av_frame_free(&frame);
        return sendReturn_;
    }

    // 接收已解码的帧
    int ret = avcodec_receive_frame(codecCtx_, frame);
    if(ret < 0 && ret != AVERROR(EAGAIN)) {
        av_packet_unref(&packet_);
        av_frame_free(&frame);
        return ret;
    }

    // 从帧 PTS 更新时钟
    if(frame->pts != AV_NOPTS_VALUE) {
        clock_ = av_q2d(stream_->time_base) * frame->pts;
    }

    // -- 检查音频格式变化，必要时重建 SWR --
    int64_t inChannelLayout = (frame->channel_layout &&
                               frame->channels == av_get_channel_layout_nb_channels(frame->channel_layout))
        ? frame->channel_layout
        : av_get_default_channel_layout(frame->channels);

    if(frame->format != audioSrcFmt_ ||
       inChannelLayout != audioSrcChannelLayout_ ||
       frame->sample_rate != audioSrcFreq_ ||
       !swrCtx_)
    {
        if(swrCtx_) {
            swr_free(&swrCtx_);
        }

        swrCtx_ = swr_alloc_set_opts(nullptr,
                                      audioDstChannelLayout_, audioDstFmt_, audioSpec_.freq,
                                      inChannelLayout, static_cast<AVSampleFormat>(frame->format), frame->sample_rate,
                                      0, nullptr);
        if(!swrCtx_ || swr_init(swrCtx_) < 0) {
            av_packet_unref(&packet_);
            av_frame_free(&frame);
            return -1;
        }

        audioSrcFmt_ = static_cast<AVSampleFormat>(frame->format);
        audioSrcChannelLayout_ = inChannelLayout;
        audioSrcFreq_ = frame->sample_rate;
    }

    // -- 重采样 --
    int resampledDataSize = 0;

    if(swrCtx_) {
        // audioBuf1_ 使用 av_malloc 分配（16 字节对齐）
        const uint8_t **in = const_cast<const uint8_t **>(frame->extended_data);
        uint8_t *out[] = { audioBuf1_ };

        int outCount = kAudioBufSize / audioSpec_.channels / av_get_bytes_per_sample(audioDstFmt_);
        int samples = swr_convert(swrCtx_, out, outCount, in, frame->nb_samples);
        if(samples < 0) {
            av_packet_unref(&packet_);
            av_frame_free(&frame);
            return -1;
        }

        if(samples == outCount) {
            qDebug() << "AudioDecoder: 重采样缓冲区可能不足，重置 SWR";
            swr_init(swrCtx_);
        }

        audioBuf_ = audioBuf1_;
        resampledDataSize = samples * audioSpec_.channels * av_get_bytes_per_sample(audioDstFmt_);
    } else {
        audioBuf_ = frame->data[0];
        resampledDataSize = av_samples_get_buffer_size(nullptr, frame->channels,
                                                        frame->nb_samples,
                                                        static_cast<AVSampleFormat>(frame->format), 1);
    }

    // 根据实际重采样数据量推进时钟
    clock_ += static_cast<double>(resampledDataSize) / (audioDepth_ * codecCtx_->channels * codecCtx_->sample_rate);

    if(sendReturn_ != AVERROR(EAGAIN)) {
        av_packet_unref(&packet_);
    }

    av_frame_free(&frame);

    return resampledDataSize;
}
