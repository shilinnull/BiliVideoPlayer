#include "odb.h"

namespace Biliodb {
    std::shared_ptr<odb::database> DBFactory::mysql(const mysql_settings& settings) {
        // 1. 创建连接池
        std::unique_ptr<odb::mysql::connection_factory> pool(
            new odb::mysql::connection_pool_factory(settings.connection_pool_size));
        // 2. 创建数据库操作句柄
        auto handler = std::make_shared<odb::mysql::database>(settings.user,
                                                              settings.passwd, settings.db, settings.host,
                                                              settings.port, nullptr,
                                                              settings.cset, 0, std::move(pool));
        return handler;
    }
}
