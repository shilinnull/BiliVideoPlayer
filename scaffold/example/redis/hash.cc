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

        // 对hash进行操作
        // 1. 插入一个hash键值对
        redis.hset("user", "name", "zhangsan");
        // 2. 批量插入多个hash键值对
        std::unordered_map<std::string, std::string> uinfo = {
            {"age", "18"},
            {"gender", "male"},
            {"hobby", "music"}
        };
        redis.hmset("user", uinfo.begin(), uinfo.end());
        // 3. 获取一个键值对
        auto val = redis.hget("user", "name");
        if (!val) {
            std::cout << "获取键值对失败" << std::endl;
            return -1;
        }
        std::cout << "name: " << *val << std::endl;
        // 4. 获取所有键值对
        std::unordered_map<std::string, std::string> results;
        redis.hgetall("user", std::inserter(results, results.begin()));
        for (auto &val : results) {
            std::cout << val.first << ": " << val.second << std::endl;
        }
        // 5. 对键值对中的值进行自增&自减操作
        redis.hincrby("user", "age", 1);
        std::cout << "age: " << *redis.hget("user", "age") << std::endl;
        redis.hincrby("user", "age", -5);
        std::cout << "age: " << *redis.hget("user", "age") << std::endl;

        redis.del("user");
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}