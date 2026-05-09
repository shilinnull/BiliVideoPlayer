#ifndef AVPACKETQUEUE_H
#define AVPACKETQUEUE_H

#include <QQueue>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <SDL2/SDL.h>

/**
 * 线程安全的 AVPacket 队列（使用 SDL 互斥锁 + 条件变量）
 * 使用值拷贝（struct copy）而非引用计数，调用方需自行管理包的释放。
 */
class AvPacketQueue
{
public:
    explicit AvPacketQueue();

    /// 入队一个包（拷贝一份到队列中）
    void enqueue(AVPacket *packet);

    /// 出队一个包，isBlock=true 时队列空则阻塞等待
    void dequeue(AVPacket *packet, bool isBlock);

    /// 队列是否为空
    bool isEmpty();

    /// 清空队列并释放所有包
    void empty();

    /// 当前队列大小
    int queueSize();

private:
    SDL_mutex *mutex;
    SDL_cond *cond;
    QQueue<AVPacket> queue;
};

#endif // AVPACKETQUEUE_H
