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

        // 对数组list进行操作
        // 1. 从左侧插入数据
        redis.lpush("skills", "C++");
        std::vector<std::string> arr = {"Python", "Java", "C"};
        redis.lpush("skills", arr.begin(), arr.end());
        // 2. 从右侧获取数据（队列，反过来就是栈）
        int len = redis.llen("skills");
        for (int i = len - 1; i >= 0; --i) {
            auto val = redis.lindex("skills", i);
            std::cout << *val << " "; 
        }
        std::cout << std::endl;
        // 3. 获取所有数据
        std::vector<std::string> skills;
        redis.lrange("skills", 0, -1, std::back_inserter(skills));
        for (auto &val : skills) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        // 5. 弹出左右两侧的数据（头删/尾删）
        redis.lpop("skills");
        redis.rpop("skills");
        // 6. 获取所有数据
        skills.clear();
        redis.lrange("skills", 0, -1, std::back_inserter(skills));
        for (auto &val : skills) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        redis.del("skills");
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}