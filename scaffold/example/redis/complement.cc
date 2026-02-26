#include <sw/redis++/redis.h>
#include <iostream>
#include <chrono>
#include <unordered_set>

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

        // 补充操作
        //  1. 字段生命周期控制（设置过期时间/移除过期时间）
        redis.set("name", "zhangsan");
        redis.expire("name", std::chrono::seconds(3));
        redis.persist("name");
        //  2. 判断字段是否存在
        bool ret = redis.exists("name");
        std::cout << "name exists: " << ret << std::endl;
        std::cout << "age exists: " << redis.exists("age") << std::endl;
        //  3. zset字典序操作 (补充两点： 1. 元素字典序区间数据获取-注意事项：元素权重必须相同；  2. 设置区间数据获取时的偏移量和个数)
        redis.zadd("score", "a", 100);
        redis.zadd("score", "d", 100);
        redis.zadd("score", "c", 100);
        redis.zadd("score", "b", 100);
        redis.zadd("score", "e", 100);
        redis.zadd("score", "f", 100);
        sw::redis::LimitOptions limit;
        limit.offset = 1;
        limit.count = 3;
        sw::redis::BoundedInterval<std::string> bounded_interval("b", "f", sw::redis::BoundType::CLOSED);
        std::vector<std::string> members;
        redis.zrangebylex("score", bounded_interval, limit, std::back_inserter(members));
        for (auto &member : members) {
            std::cout << member << " ";
        }
        std::cout << std::endl;
        //  4. 库的清理操作
        redis.flushall();
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}