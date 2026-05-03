#ifndef VIDEOPLAYER_REDIS_H
#define VIDEOPLAYER_REDIS_H

#include <sw/redis++/redis.h>
#include <sw/redis++/queued_redis.h>

namespace Biliredis {
    struct redis_settings {
        int db = 0;
        int port = 6379;
        std::string host;
        std::string user = "default";
        std::string passwd;
        size_t connection_pool_size = 3;
    };

    class RedisFactory {
    public:
        static std::shared_ptr<sw::redis::Redis> create(const redis_settings& settings);
    };
}

#endif //VIDEOPLAYER_REDIS_H