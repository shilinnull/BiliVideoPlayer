#include "../../source/redis.h"

#include <iostream>

int main()
{
    try {
        Biliredis::redis_settings settings = {
            .host = "192.168.80.128",
            .passwd = "123456",
            .connection_pool_size = 3
        };
        auto redis = Biliredis::RedisFactory::create(settings);
        {
            auto tx = redis->transaction(false, false);
            auto r = tx.redis();

            bool ret = r.set("name", "zhangsan");
            if (!ret) {
                std::cout << "添加键值对失败" << std::endl;
                return -1;
            }
            // 2. 获取一个键值对
            auto val = r.get("name");
            if (!val) {
                std::cout << "获取键值对失败" << std::endl;
                return -1;
            }
            std::cout << "name: " << *val << std::endl;
            r.flushall();
        }
        
    }catch (const sw::redis::Error &e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}