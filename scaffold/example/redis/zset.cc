#include <sw/redis++/redis.h>
#include <iostream>
#include <chrono>
#include <unordered_set>


// 6. 迭代浏览集合中的数据
void print(sw::redis::Redis &redis, const std::string &key) {
    sw::redis::Cursor cursor = 0; //定义光标，默认从第0个元素开始进行匹配迭代
    std::vector<std::pair<std::string, double>> members; //定义输出集合
    while (true) {
        cursor = redis.zscan(key, cursor, "*", 10, std::back_inserter(members));
        if (cursor == 0) { break; }
    }
    for (auto &member : members) {
        std::cout << member.first << " : " << member.second << std::endl;
    }
}

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

        // 对有序集合zset的操作
        // 1. 向有序集合添加数据 - 单个添加/批量添加
        redis.zadd("score", "zhangsan", 34);
        std::unordered_map<std::string, double> m = {
            {"lisi", 95},
            {"wangwu", 56},
            {"zhaoliu", 78},
            {"tianqi", 89},
            {"xiaohong", 99},
            {"xiaoming", 88},
            {"xiaolan", 77},
            {"xiaowang", 66}
        };
        redis.zadd("score", m.begin(), m.end());
        print(redis, "score");
        std::cout << "---------------------新增数据完毕---------------\n";
        // 2. 获取集合中元素数量
        std::cout << "元素数量: " << redis.zcard("score") << std::endl;
        // 3. 获取指定元素权重得分（以是否能够得到得分判断元素是否存在）
        std::cout << "小红得分:" << *redis.zscore("score", "xiaohong") << std::endl;
        // 4. 获取指定元素权重排名（以是否能够得到排名判断元素是否存在）
        std::cout <<  "小红从低到高排名:" << *redis.zrank("score", "xiaohong") << std::endl;
        std::cout <<  "小红从高到低排名:" << *redis.zrevrank("score", "xiaohong") << std::endl;
        // 5. 获取集合数据 - 以得分排名获取区间元素/以得分获取区间元素（从低到高/从高到低） 
        std::vector<std::string> result;
        redis.zrange("score", 0, 2, std::back_inserter(result));
        std::cout << "从低到高排名前3名: ";
        for (auto &member : result) {
            std::cout << member << " ";
        }
        std::cout << std::endl;
        result.clear();
        redis.zrevrange("score", 0, 2, std::back_inserter(result));
        std::cout << "从高到低排名前3名: ";
        for (auto &member : result) {
            std::cout << member << " ";
        }
        std::cout << std::endl;

        result.clear();
        auto interval = sw::redis::BoundedInterval<double>(0, 60, sw::redis::BoundType::RIGHT_OPEN);
        redis.zrangebyscore("score", interval, std::back_inserter(result));
        std::cout << "60分以下的学生(从低到高): ";
        for (auto &member : result) {
            std::cout << member << " ";
        }
        std::cout << std::endl;
        result.clear();
        redis.zrevrangebyscore("score", interval, std::back_inserter(result));
        std::cout << "60分以下的学生(从高到低): ";
        for (auto &member : result) {
            std::cout << member << " ";
        }
        std::cout << std::endl;
        // 8. 删除指定元素
        std::cout << "---------------------删除小红------------------" << std::endl;
        redis.zrem("score", "xiaohong");
        print(redis, "score");
        std::cout << "---------------------删除数据完毕---------------\n";
        // 11. 弹出集合中最高/最低分元素
        auto max = redis.zpopmax("score");
        if (max) { std::cout << "弹出最高分: " << max->first << " : " << max->second << std::endl; }
        auto min = redis.zpopmin("score");
        if (min) { std::cout << "弹出最低分: " << min->first << " : " << min->second << std::endl; }
        print(redis, "score");
        // 9. 以权重得分删除指定区间元素
        std::cout << "---------------------删除70~80的学生------------------" << std::endl;
        auto interval1 = sw::redis::BoundedInterval<double>(70, 80, sw::redis::BoundType::OPEN);
        redis.zremrangebyscore("score", interval1);
        print(redis, "score");
        std::cout << "---------------------删除数据完毕---------------\n";
        // 10. 以权重排名删除指定区间元素
        redis.zremrangebyrank("score", 0, 2);
        print(redis, "score");
        //13. 修改元素权重
        redis.zincrby("score", 10, "tianqi");
        print(redis, "score");
        redis.zincrby("score", -20, "tianqi");
        print(redis, "score");

        redis.del("score");
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}