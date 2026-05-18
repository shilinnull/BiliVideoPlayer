#include "player.h"
#include "audiodecoder.h"
#include "util.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QUuid>
#include <QWidget>

#include <chrono>
#include <thread>

namespace {
QString findFfmpegExecutable()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString currentDir = QDir::currentPath();
    const QStringList candidates = {
        QDir(appDir).filePath("ffmpeg.exe"),
        QDir(appDir).filePath("ffmpeg/ffmpeg.exe"),
        QDir(currentDir).filePath("ffmpeg/ffmpeg.exe")
    };
    for(const auto& path : candidates) {
        if(QFileInfo::exists(path)) {
            return path;
        }
    }
    return QString();
}
}

FFmpegPlayer::FFmpegPlayer(QWidget* videoRenderWnd, QObject* parent)
    : QObject(parent)
    , state(std::make_unique<FFmpegPlayerState>(this))
    , audioDecoder(new AudioDecoder(this))
{

    // FFmpeg 4.x 需要手动注册，FFmpeg 5+ 自动完成
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 64, 100)
    av_register_all();
#endif
#pragma GCC diagnostic pop

    avformat_network_init();

    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        LOG() << "SDL 初始化失败:" << SDL_GetError();
    }

    // 音频解码完毕信号处理
    // 当 AudioDecoder 队列耗尽且文件读完时触发
    connect(audioDecoder, &AudioDecoder::playFinished, this, [this](int seq) {
        // 丢弃 seek 之前残留的过期信号（flushSeq 在 seek 时已递增）
        if(seq != audioDecoder->flushSeq()) {
            return;
        }
        // 通知 UI 播放结束（只通知一次）
        if(!state->endEmitted.exchange(true)) {
            QMetaObject::invokeMethod(this, [this]() {
                emit endOfPlaylist();
            }, Qt::QueuedConnection);
        }
    });

    // 倍速切换失败时转发信号给 UI
    connect(audioDecoder, &AudioDecoder::speedResetToDefault, this, &FFmpegPlayer::speedResetToDefault);

    // 位置定时器，每 500ms 发射一次当前播放时间
    positionTimer = new QTimer(this);
    positionTimer->setInterval(kPositionUpdateMs);
    connect(positionTimer, &QTimer::timeout, this, &FFmpegPlayer::onPositionTick);
    positionTimer->start();
}

FFmpegPlayer::~FFmpegPlayer()
{
    stopPlayback();

    SDL_Quit();
}

bool FFmpegPlayer::isPlaying() const
{
    return state && !state->paused.load();
}

/**
 * 停止播放并等待所有线程退出。
 * 流程：
 * 1. 设置 stop 标志 → interruptCb 返回 1 → 中断 av_read_frame
 * 2. 通知视频线程唤醒
 * 3. 通知音频解码器停止
 * 4. join 解码线程和视频线程
 * 5. 释放所有 FFmpeg 和 SDL 资源
 */
void FFmpegPlayer::stopPlayback()
{
    if(!state) {
        return;
    }

    state->stop.store(true);
    state->paused.store(true);
    state->videoCond.notify_all();

    audioDecoder->stopAudio();

    if(state->decodeThread.joinable()) {
        state->decodeThread.join();
    }
    if(state->videoThread.joinable()) {
        state->videoThread.join();
    }

    audioDecoder->closeAudio();

    if(state->swsCtx) {
        sws_freeContext(state->swsCtx);
        state->swsCtx = nullptr;
    }

    if(state->videoCtx) {
        avcodec_free_context(&state->videoCtx);
    }
    if(state->formatCtx) {
        avformat_close_input(&state->formatCtx);
    }

    {
        std::lock_guard<std::mutex> lock(state->videoMutex);
        while(!state->videoQueue.empty()) {
            VideoFrame vf = state->videoQueue.front();
            state->videoQueue.pop_front();
            if(vf.frame) {
                av_frame_free(&vf.frame);
            }
        }
    }

    state->resetClocks();
    state->stop.store(false);
    state->endEmitted.store(false);
    state->renderedFirstFrame.store(false);
    state->decodeError.store(false);
    state->frameDecoded.store(false);
}

void FFmpegPlayer::startPlay(const QString& videoPath)
{
    if(!state) {
        return;
    }

    stopPlayback();

    state->sourceUrl = videoPath.toUtf8().toStdString();
    state->paused.store(true);
    state->decodeError.store(false);

    // ========== 解码线程 ==========
    // 职责：解复用（av_read_frame）、视频解码、音视频包分发
    state->decodeThread = std::thread([this]() {
        auto* st = state.get();
        if(!st) {
            return;
        }

        // 1. 打开输入文件/m3u8 URL
        st->formatCtx = avformat_alloc_context();
        if(!st->formatCtx) {
            LOG() << "avformat_alloc_context 失败";
            st->decodeError.store(true);
            st->videoCond.notify_all();
            return;
        }
        st->formatCtx->interrupt_callback.callback = interruptCb;
        st->formatCtx->interrupt_callback.opaque = state.get();

        if(avformat_open_input(&st->formatCtx, st->sourceUrl.c_str(), nullptr, nullptr) < 0) {
            LOG() << "avformat_open_input 失败";
            st->decodeError.store(true);
            st->videoCond.notify_all();
            return;
        }

        if(avformat_find_stream_info(st->formatCtx, nullptr) < 0) {
            LOG() << "avformat_find_stream_info 失败";
            st->decodeError.store(true);
            st->videoCond.notify_all();
            return;
        }

        // 2. 查找最佳音视频流
        st->videoStreamIndex = av_find_best_stream(st->formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        st->audioStreamIndex = av_find_best_stream(st->formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

        if(st->videoStreamIndex < 0 && st->audioStreamIndex < 0) {
            LOG() << "未找到音视频流";
            st->decodeError.store(true);
            st->videoCond.notify_all();
            return;
        }

        {
            LOG() << "找到流: video=" << st->videoStreamIndex
                  << " audio=" << st->audioStreamIndex;
        }

        // 3. 初始化视频解码器
        if(st->videoStreamIndex >= 0) {
            AVStream* stream = st->formatCtx->streams[st->videoStreamIndex];
            const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if(codec) {
                st->videoCtx = avcodec_alloc_context3(codec);
                if(avcodec_parameters_to_context(st->videoCtx, stream->codecpar) < 0) {
                    LOG() << "视频参数复制失败";
                    avcodec_free_context(&st->videoCtx);
                } else if(avcodec_open2(st->videoCtx, codec, nullptr) < 0) {
                    LOG() << "视频解码器打开失败";
                    avcodec_free_context(&st->videoCtx);
                } else {
                    st->videoTimeBase = stream->time_base;
                }
            } else {
                LOG() << "未找到视频解码器";
            }
        }

        // 4. 初始化音频解码器
        bool audioReady = false;
        if(st->audioStreamIndex >= 0) {
            audioReady = audioDecoder->openAudio(st->formatCtx, st->audioStreamIndex);
            if(!audioReady) {
                LOG() << "音频解码器打开失败，继续无音频播放";
            }
        }

        if(st->videoCtx == nullptr && st->audioStreamIndex < 0) {
            LOG() << "音视频解码器均打开失败";
            st->decodeError.store(true);
            st->videoCond.notify_all();
            return;
        }

        // 5. 发射视频总时长
        if(st->formatCtx->duration > 0) {
            int64_t duration = static_cast<int64_t>(st->formatCtx->duration / AV_TIME_BASE);
            QMetaObject::invokeMethod(this, [this, duration]() {
                emitDuration(duration);
            }, Qt::QueuedConnection);
        }

        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();
        int readErrorRetries = 0;
        const int maxReadErrorRetries = 10;

        // 6. 主循环：读取包 → 分发到视频/音频
        while(!st->stop.load()) {

            // ---- 处理 seek 跳转 ----
            if(st->seekRequest.load()) {
                int64_t target = st->seekTarget.load();
                int64_t ts = target * AV_TIME_BASE;
                int seekRet = avformat_seek_file(st->formatCtx, -1, INT64_MIN, ts, INT64_MAX, 0);
                if(seekRet < 0 && st->videoStreamIndex >= 0) {
                    // 如果 avformat_seek_file 失败，退回到按视频流时间基 seek
                    AVRational tb = st->formatCtx->streams[st->videoStreamIndex]->time_base;
                    int64_t streamTs = av_rescale_q(ts, AV_TIME_BASE_Q, tb);
                    seekRet = av_seek_frame(st->formatCtx, st->videoStreamIndex, streamTs, AVSEEK_FLAG_BACKWARD);
                }
                if(seekRet >= 0) {
                    // 刷新视频解码器
                    if(st->videoCtx) avcodec_flush_buffers(st->videoCtx);

                    // 清空视频帧队列
                    {
                        std::lock_guard<std::mutex> lock(st->videoMutex);
                        while(!st->videoQueue.empty()) {
                            VideoFrame vf = st->videoQueue.front();
                            st->videoQueue.pop_front();
                            if(vf.frame) av_frame_free(&vf.frame);
                        }
                    }
                    // 刷新音频并重置时钟
                    audioDecoder->flushAudio();
                    st->resetClocks();
                    st->renderedFirstFrame.store(false);
                    st->endEmitted.store(false);
                } else {
                    LOG() << "seek 失败，目标=" << target << "秒, ret=" << seekRet;
                }
                st->seekRequest.store(false);
            }

            int ret = av_read_frame(st->formatCtx, packet);
            if(ret < 0) {
                if(st->stop.load()) {
                    break;
                }
                if(ret == AVERROR(EAGAIN)) {
                    // HLS 在缓冲下一个分片时会返回 EAGAIN，这不是错误，继续重试
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                if(ret == AVERROR_EOF) {
                    // 正常结束，稍后处理
                } else {
                    LOG() << "av_read_frame 错误:" << ret;
                    if(++readErrorRetries < maxReadErrorRetries) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }
                }

                // ---- 到达文件末尾：排空解码器中缓存的帧 ----
                bool hasDrained = false;
                if(st->videoCtx) {
                    avcodec_send_packet(st->videoCtx, nullptr);
                    while(avcodec_receive_frame(st->videoCtx, frame) == 0) {
                        convertAndQueueFrame(frame);
                        hasDrained = true;
                    }
                }

                audioDecoder->readFileFinished();

                if(hasDrained) {
                    for(int i = 0; i < 10 && !st->stop.load(); ++i) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }

                // 如果没有音频流，音频解码器不会发射 playFinished 信号，
                // 需要在这里直接通知 UI 播放结束
                if(!audioReady) {
                    if(!st->endEmitted.exchange(true)) {
                        QMetaObject::invokeMethod(this, [this]() {
                            emit endOfPlaylist();
                        }, Qt::QueuedConnection);
                    }
                }

                // EOF 之后保持线程存活，等待用户 seek 或停止
                while(!st->stop.load()) {
                    if(st->seekRequest.load()) {
                        st->endEmitted.store(false);
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

                if(st->seekRequest.load()) {
                    continue;
                }

                // 停止请求
                break;
            }
            readErrorRetries = 0;

            // ---- 分发包：视频包送解码，音频包送 AudioDecoder ----
            if(packet->stream_index == st->videoStreamIndex && st->videoCtx) {
                int sendRet = avcodec_send_packet(st->videoCtx, packet);
                if(sendRet == AVERROR(EAGAIN)) {
                    // 解码器队列满，先取帧腾出空间
                    while(true) {
                        int recvRet = avcodec_receive_frame(st->videoCtx, frame);
                        if(recvRet != 0) break;
                        convertAndQueueFrame(frame);
                    }
                    sendRet = avcodec_send_packet(st->videoCtx, packet);
                }

                if(sendRet == 0) {
                    while(true) {
                        int recvRet = avcodec_receive_frame(st->videoCtx, frame);
                        if(recvRet != 0) {
                            break;
                        }
                        convertAndQueueFrame(frame);
                    }
                } else if(sendRet < 0) {
                    LOG() << "avcodec_send_packet(视频) 返回" << sendRet;
                }
            } else if(packet->stream_index == st->audioStreamIndex && audioReady) {
                // 音频包直接转发给 AudioDecoder 的队列
                // AvPacketQueue 内部做值拷贝，所以这里不能 av_packet_unref
                audioDecoder->packetEnqueue(packet);
                continue;
            }

            av_packet_unref(packet);
        }

        av_frame_free(&frame);
        av_packet_free(&packet);
    });

    // ========== 视频线程 ==========
    // 职责：从视频帧队列取帧 → 音视频同步 → 发送 QImage 到主线程
    state->videoThread = std::thread([this]() {
        auto* st = state.get();
        if(!st) {
            return;
        }

        auto lastFrameTime = std::chrono::steady_clock::now();
        double lastPtsSec = 0.0;

        while(!st->stop.load()) {
            if(st->decodeError.load()) {
                break;
            }

            // 暂停时不消费帧（但保留首帧用于预览）
            if(st->paused.load() && st->renderedFirstFrame.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            VideoFrame vf;
            {
                std::unique_lock<std::mutex> lock(st->videoMutex);
                if(st->videoQueue.empty()) {
                    st->videoCond.wait_for(lock, std::chrono::milliseconds(20));
                    continue;
                }
                vf = st->videoQueue.front();
                st->videoQueue.pop_front();
            }

            if(vf.frame == nullptr) {
                continue;
            }

            // -- 音视频同步：将视频帧的 PTS 与音频时钟对齐 --
            double audioClock = st->audioClock.load();
            if(st->audioStreamIndex < 0) {
                // 无音频流：用视频自身的 PTS 作为时钟
                audioClock = vf.ptsSec;
                st->audioClock.store(audioClock);
            } else {
                audioClock = audioDecoder->getAudioClock();
                st->audioClock.store(audioClock);
            }
            double avDelay = vf.ptsSec - audioClock;

            // -- 墙钟帧率限制（安全网），防止解码过快导致画面撕裂 --
            double wallDelay = 0.0;
            if(st->renderedFirstFrame.load()) {
                double ptsDiff = vf.ptsSec - lastPtsSec;
                if(ptsDiff > 0.0) {
                    auto now = std::chrono::steady_clock::now();
                    double elapsed = std::chrono::duration<double>(now - lastFrameTime).count();
                    wallDelay = ptsDiff - elapsed;
                    if(wallDelay < 0.0) {
                        wallDelay = 0.0;
                    }
                }
            }

            // -- 帧策略 --
            // 首帧：立即显示
            // 前 10 帧（预热期）：不丢弃，放松同步
            // 正常：如果视频落后音频超过阈值则丢帧
            if(!st->renderedFirstFrame.load()) {
                avDelay = 0.0;
                wallDelay = 0.0;
                st->renderedFirstFrame.store(true);
            } else if(st->framesSincePlay.load() < 10) {
                st->framesSincePlay.fetch_add(1);
                if(avDelay < -kFrameDropThresholdSec) {
                    avDelay = 0.0;
                }
            } else if(avDelay < -kFrameDropThresholdSec) {
                av_frame_free(&vf.frame);
                continue;
            }

            // 取两个延迟中的较大值，保证不会跑得太快
            double delay = std::max(avDelay, wallDelay);
            if(delay > 0.0) {
                SDL_Delay(static_cast<Uint32>(delay * 1000));
            }

            // 将 RGB32 帧转为 QImage 并发射到主线程
            QImage tmp(vf.frame->data[0], vf.frame->width, vf.frame->height,
                       vf.frame->linesize[0], QImage::Format_RGB32);
            QImage image = tmp.copy();
            QMetaObject::invokeMethod(this, [this, image]() {
                emit frameReady(image);
            }, Qt::QueuedConnection);

            av_frame_free(&vf.frame);

            lastFrameTime = std::chrono::steady_clock::now();
            lastPtsSec = vf.ptsSec;
        }
    });
}

void FFmpegPlayer::play()
{
    if(!state) {
        return;
    }
    state->paused.store(false);
    state->framesSincePlay.store(0);
    audioDecoder->pauseAudio(false);
}

void FFmpegPlayer::pause()
{
    if(!state) {
        return;
    }
    state->paused.store(true);
    audioDecoder->pauseAudio(true);
}

void FFmpegPlayer::setPlaySpeed(double speed)
{
    if(!state) {
        return;
    }
    if(speed <= 0.0) {
        return;
    }
    audioDecoder->setSpeed(speed);
}

void FFmpegPlayer::setVolume(int64_t volume)
{
    if(volume < 0) volume = 0;
    if(volume > 100) volume = 100;
    // 将 0-100 映射到 0-SDL_MIX_MAXVOLUME (128)
    int sdlVol = static_cast<int>(volume * SDL_MIX_MAXVOLUME / 100);
    audioDecoder->setVolume(sdlVol);
}

void FFmpegPlayer::setCurrentPlayPositon(int64_t seconds)
{
    if(!state) {
        return;
    }
    state->seekTarget.store(seconds);
    state->seekRequest.store(true);
}

int64_t FFmpegPlayer::getPlayTime() const
{
    return curPlayTime;
}

void FFmpegPlayer::onPositionTick()
{
    if(!state) {
        return;
    }

    int64_t time = static_cast<int64_t>(state->audioClock.load());
    curPlayTime = time;
    emit playPositionChanged(curPlayTime);
}

void FFmpegPlayer::emitDuration(int64_t duration)
{
    durationSec = duration;
    emit durationChanged(durationSec);
}

void FFmpegPlayer::convertAndQueueFrame(AVFrame* frame)
{
    auto* st = state.get();
    if(!st) return;

    if(!st->swsCtx) {
        st->swsCtx = sws_getContext(
            frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
            frame->width, frame->height, AV_PIX_FMT_RGB32,
            SWS_BILINEAR, nullptr, nullptr, nullptr);
    }

    AVFrame* yuvFrame = av_frame_alloc();
    yuvFrame->format = AV_PIX_FMT_RGB32;
    yuvFrame->width = frame->width;
    yuvFrame->height = frame->height;
    if(av_frame_get_buffer(yuvFrame, 32) < 0) {
        av_frame_free(&yuvFrame);
        return;
    }

    sws_scale(st->swsCtx, frame->data, frame->linesize, 0, frame->height,
              yuvFrame->data, yuvFrame->linesize);

    VideoFrame vf;
    vf.frame = yuvFrame;
    vf.ptsSec = framePtsSec(frame, st->videoTimeBase);

    {
        std::lock_guard<std::mutex> lock(st->videoMutex);
        st->videoQueue.push_back(vf);
        st->frameDecoded.store(true);
    }
    st->videoCond.notify_one();
}

QString FFmpegPlayer::getVideoFirstFrame(const QString& videoPath)
{
    const QString ffmpegPath = findFfmpegExecutable();
    if(ffmpegPath.isEmpty()) {
        LOG() << "未找到 ffmpeg.exe，无法自动截取封面";
        return "";
    }

    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if(tempDir.isEmpty()) {
        tempDir = QCoreApplication::applicationDirPath();
    }
    const QString firstFrame = QDir(tempDir).filePath(
        QString("BiliVideoPlayer_firstFrame_%1.png").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));

    QStringList cmd;
    cmd << "-ss" << "00:00:00"
        << "-i" << videoPath
        << "-vframes" << "1"
        << "-y"
        << firstFrame;

    QProcess ffmpegProcess;
    ffmpegProcess.start(ffmpegPath, cmd);

    if(!ffmpegProcess.waitForFinished(-1)) {
        LOG() << "ffmpeg 进程执行失败";
        return "";
    }
    if(ffmpegProcess.exitStatus() != QProcess::NormalExit || ffmpegProcess.exitCode() != 0) {
        LOG() << "ffmpeg 截帧失败" << ffmpegProcess.readAllStandardError();
        return "";
    }
    if(!QFileInfo::exists(firstFrame)) {
        LOG() << "ffmpeg 未生成封面图";
        return "";
    }

    return firstFrame;
}
