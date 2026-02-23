#include "redis.h"

namespace Biliredis {
    std::shared_ptr<sw::redis::Redis> RedisFactory::create(const redis_settings& settings) {
        sw::redis::ConnectionOptions conn_opts = {
            .host = settings.host,
            .port = settings.port,
            .user = settings.user,
            .password = settings.passwd,
            .db = settings.db // 默认0号库
        };
        sw::redis::ConnectionPoolOptions pool_opts = {
            .size = settings.connection_pool_size
        };
        return std::make_shared<sw::redis::Redis>(conn_opts, pool_opts);
    }
}