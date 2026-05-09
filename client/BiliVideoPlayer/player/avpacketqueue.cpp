#include "avpacketqueue.h"

AvPacketQueue::AvPacketQueue()
{
    mutex = SDL_CreateMutex();
    cond  = SDL_CreateCond();
}

void AvPacketQueue::enqueue(AVPacket *packet)
{
    SDL_LockMutex(mutex);
    queue.enqueue(*packet);
    SDL_CondSignal(cond);
    SDL_UnlockMutex(mutex);
}

void AvPacketQueue::dequeue(AVPacket *packet, bool isBlock)
{
    SDL_LockMutex(mutex);
    while(1) {
        if(!queue.isEmpty()) {
            *packet = queue.dequeue();
            SDL_UnlockMutex(mutex);
            return;
        } else if(!isBlock) {
            break;
        } else {
            SDL_CondWait(cond, mutex);
        }
    }
    SDL_UnlockMutex(mutex);
}

void AvPacketQueue::empty()
{
    SDL_LockMutex(mutex);
    while(queue.size() > 0) {
        AVPacket pkt = queue.dequeue();
        av_packet_unref(&pkt);
    }
    SDL_UnlockMutex(mutex);
}

bool AvPacketQueue::isEmpty()
{
    SDL_LockMutex(mutex);
    bool empty = queue.isEmpty();
    SDL_UnlockMutex(mutex);
    return empty;
}

int AvPacketQueue::queueSize()
{
    SDL_LockMutex(mutex);
    int size = queue.size();
    SDL_UnlockMutex(mutex);
    return size;
}
