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

        // 对无序集合set的操作
        // 1. 向集合中添加一个元素
        redis.sadd("skills", "C++");
        // 2. 批量向集合中添加多个元素
        std::unordered_set<std::string> skills = {"Python", "Java", "Go"};
        redis.sadd("skills", skills.begin(), skills.end());
        // 3. 获取集合中元素的个数
        auto count = redis.scard("skills");
        std::cout << "skills size: " << count << std::endl;
        // 4. 判断集合中是否包含某个元素
        bool ret = redis.sismember("skills", "C++");
        std::cout << "skills contains C++: " << ret << std::endl;
        ret = redis.sismember("skills", "C");
        std::cout << "skills contains C: " << ret << std::endl;
        // 5. 获取集合中所有元素
        std::unordered_set<std::string> skills2;
        redis.smembers("skills", std::inserter(skills2, skills2.begin()));
        for (auto &val : skills2) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        redis.del("skills");
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}