#include "mpvplayer.h"
#include "util.h"
#include <QWidget>
#include <QDir>
#include <QProcess>

static void wakeup(void* ctx)
{
    MpvPlayer* mpv = (MpvPlayer*)ctx;
    emit mpv->mpvEvents();
}

MpvPlayer::MpvPlayer(QWidget* videoRenderWnd, QObject *parent)
    : QObject{parent}
{
    // 设置程序区域
    // LC_NUMRIC: 设置数字格式(包括小数点和千分位符)
    // "C": 表示使用C标准的默认区域设置
    std::setlocale(LC_NUMERIC, "C");

    // 创建mpv实例
    mpv = mpv_create();
    if(nullptr == mpv){
        LOG()<<"mpv实例创建失败";
        return;
    }

    // 设置视频渲染窗口--将窗口的id告知给mpv
    if(videoRenderWnd){
        int64_t wid = videoRenderWnd->winId();
        mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    }else{
        // 此处不需要播放视频，让视频在mpv后台加载成功即可
        // 禁止视频画面 以及 音频输出
        // vo: 表示视频输出  ao：表示音频输出
        // vo: null 表示禁止视频输出
        // ao: null 表示禁止音频输出
        mpv_set_option_string(mpv, "vo", "null");
        mpv_set_option_string(mpv, "ao", "null");
    }

    // 初始化mpv实例
    if(mpv_initialize(mpv) < 0){
        LOG()<<"mpv初始化失败";
        mpv_destroy(mpv);
        return;
    }

    // 订阅time-pos属性
    // 注意：MPV_FORMAT_INT64  每秒触发一次
    //      MPV_FORMAT_DOUBLE 每秒中会触发多次
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_INT64);

    // 订阅 duration 属性
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);

    // 设置mpv事件触发时的回调函数
    connect(this, &MpvPlayer::mpvEvents, this, &MpvPlayer::onMpvEvents, Qt::QueuedConnection);
    mpv_set_wakeup_callback(mpv, wakeup, this);
}

MpvPlayer::~MpvPlayer()
{
    if(mpv){
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }
}

void MpvPlayer::onMpvEvents()
{
    // 循环处理所有事件，直到mpv事件队列为空
    while(mpv){
        mpv_event* event = mpv_wait_event(mpv, 0);
        if(MPV_EVENT_NONE == event->event_id){
            break;
        }

        // 获取到具体的事件，处理该事件
        handleMpvEvent(event);
    }
}

void MpvPlayer::handleMpvEvent(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        // 属性发生变化的事件
        mpv_event_property* eventProperty = (mpv_event_property*)event->data;
        // bug：在没有播放视频时，该属性可能会发生，此时eventProperty->data为空
        // 该种情况需要过滤掉，否则程序会崩溃
        if(nullptr == eventProperty->data){
            return;
        }

        if(0 == strcmp(eventProperty->name, "time-pos")){
            int64_t seconds = *((int64_t*)eventProperty->data);
            emit playPositionChanged(seconds);
        }
        break;
    }
    case MPV_EVENT_END_FILE:
    {
        mpv_event_end_file* endFile = (mpv_event_end_file*)event->data;
        if(endFile->reason == MPV_END_FILE_REASON_EOF){
            // 检测是否最后一个视频分片播放结束
            int64_t playlistCount = -1;
            int64_t playlistPos = -1;
            mpv_get_property(mpv, "playlist-count", MPV_FORMAT_INT64, &playlistCount);
            mpv_get_property(mpv, "playlist-pos", MPV_FORMAT_INT64, &playlistPos);

            if(playlistCount > 0 && playlistPos == playlistCount-1){
                LOG()<<"整个视频播放结束";
                emit endOfPlaylist();
            }else{
                LOG()<<"单个视频分片播放结束";
            }
        }
        break;
    }
    case MPV_EVENT_SHUTDOWN:
    {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
        break;
    }
    default:
        break;
    }
}

void MpvPlayer::startPlay(const QString &videoPath)
{
    // 发送加载视频命令，播放视频
    // 注意：mpv在加载视频时，视频路径中如果包含中文，需要将其转成utf8格式编码
    const QByteArray fileName = videoPath.toUtf8();
    const char* args[] = {"loadfile", fileName.data(), NULL};
    mpv_command_async(mpv, 0, args);
}

void MpvPlayer::play()
{
    int pause = 0;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &pause);
}

void MpvPlayer::pause()
{
    int pause = 1;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &pause);
}

void MpvPlayer::setPlaySpeed(double speed)
{
    mpv_set_property_async(mpv, 0, "speed", MPV_FORMAT_DOUBLE, &speed);
}

void MpvPlayer::setMute(bool isMute)
{
    int flag = isMute? 1 : 0;
    mpv_set_property_async(mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
}

void MpvPlayer::setVolume(int64_t volume)
{
    mpv_set_property_async(mpv, 0, "volume", MPV_FORMAT_INT64, &volume);
}

void MpvPlayer::setCurrentPlayPositon(int64_t seconds)
{
    mpv_set_property_async(mpv, 0, "time-pos", MPV_FORMAT_INT64, &seconds);
}

int64_t MpvPlayer::getPlayTime() const
{
    return curPlayTime;
}

QString MpvPlayer::getVideoFirstFrame(const QString &videoPath)
{
    QString ffmpegPath = QDir::currentPath() + "/ffmpeg/ffmpeg.exe";
    QString firstFrame = QDir::currentPath() + "/firstFrame.png";

    // 构造截图命令
    // 注意：不要将参数的参数对应的值放到一个字符串中
    // 猜测：ffmpeg工具在解析命令时，可能是将参数的和值分开来解析的，即需要先解析出参数，然后在解析出参数的值
    // 放到一个字符串中，可能就会被当成某个参数 或者 某个参数的值来进行处理
    QStringList cmd;
    cmd<<"-ss"<<"00:00:00"
        <<"-i"<<videoPath
        <<"-vframes"<<"1"
        <<firstFrame;

    // 创建进程，让该进程调用ffmpeg工具完成截图
    QProcess ffmpegProcess;
    ffmpegProcess.start(ffmpegPath, cmd);

    // 等待进程截图完成, -1: 无限等待，直到进程结束
    if(!ffmpegProcess.waitForFinished(-1)){
        LOG()<<"ffmpeg 进程执行失败";
        return "";
    }

    // 返回视频首帧图的路径
    return firstFrame;
}
