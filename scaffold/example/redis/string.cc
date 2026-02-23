#include <sw/redis++/redis.h>
#include <iostream>
#include <chrono>

int main()
{
    try {
        // 1. 实例化redis客户端对象
        sw::redis::ConnectionOptions conn_opts = {
            .host = "192.168.80.128",
            .password = "123456"
        };
        sw::redis::ConnectionPoolOptions pool_opts = {
            .size = 10,
            .connection_idle_time = std::chrono::milliseconds(3600)
        };
        sw::redis::Redis redis(conn_opts, pool_opts);
        // 2. 通过客户端对象进行字符串键值对操作
        // 1. 添加一个键值对
        bool ret = redis.set("name", "zhangsan");
        if (!ret) {
            std::cout << "添加键值对失败" << std::endl;
            return -1;
        }
        // 2. 获取一个键值对
        auto val = redis.get("name");
        if (!val) {
            std::cout << "获取键值对失败" << std::endl;
            return -1;
        }
        std::cout << "name: " << *val << std::endl;
        // 3. 删除一个键值对
        redis.del("name");
        // 4. 对键值对中的数据进行自增&自减操作
        redis.set("age", "18");
        redis.incrby("age", 1);
        std::cout << "age: " << *redis.get("age") << std::endl;
        redis.decrby("age", 1);
        std::cout << "age: " << *redis.get("age") << std::endl;
        redis.del("age");
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}