#include <sw/redis++/redis.h>
#include <sw/redis++/queued_redis.h>
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
        // transaction操作
        // 注意：当前我们默认使用连接池中的连接构造transaction对象，因此注意transaction对象的作用域，生命周期，使用范围
        {
            // 创建transaction对象
            auto pipe = redis.transaction(false, false);
            pipe.set("key1", "value1");
            pipe.set("key2", "value2");
            pipe.set("key3", "value3");
            pipe.set("key4", "value4");
            pipe.get("key4");
            pipe.get("key3");
            auto reply = pipe.exec();
            std::cout << "结果数量：" << reply.size() << std::endl;
            
            std::cout << reply.get<bool>(0) << std::endl;
            std::cout << reply.get<bool>(1) << std::endl;
            std::cout << reply.get<bool>(2) << std::endl;
            std::cout << reply.get<bool>(3) << std::endl;
            auto val4 = reply.get<sw::redis::OptionalString>(4);
            std::cout << "key4: " << *val4 << std::endl;
            auto val3 = reply.get<sw::redis::OptionalString>(5);
            std::cout << "key3: " << *val3 << std::endl;
        }
        {
            auto pipe = redis.transaction(false, false);
            pipe.hset("stu", "name", "zhangsan");
            pipe.hset("stu", "age", "20");
            pipe.hset("stu", "gender", "male");
            pipe.hgetall("stu");
            auto reply = pipe.exec();

            std::unordered_map<std::string, std::string> stu;
            reply.get(3, std::inserter(stu, stu.begin()));
            for (auto &val : stu) {
                std::cout << val.first << ": " << val.second << std::endl;
            }
        }
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}