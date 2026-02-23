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
        redis.set("score", "100");
        //watch操作
        // 1. 样例： 对stu中的age字段进行增长
        // 操作流程：
        //  1. 通过redis对象，创建transaction对象
        //  2. 通过transaction对象，创建redis对象（这个redis对象与transaction对象共享连接）
        //  3. 通过redis对象执行watch操作( 1. transaction中没有watch操作； 2. transaction操作无法立即获取结果，但是redis操作可以)
        //  4. 通过redis对象执行exists判断字段是否存在
        //  5. 若字段存在，则通过transaction对象，执行incrby操作（事务执行过程中，若key值发生改变，则会抛出异常）
        {
            auto transaction = redis.transaction();
            auto redis = transaction.redis();
            while(true){
                try {
                    redis.watch("score");
                    bool res = redis.exists("score");
                    if(!res){
                        //如果不存在，则可以从数据库加载数据到缓存，然后再执行事务，或者什么都不做
                        break;
                    }
                    //transaction.incrby("score", 1).exec();
                    transaction.incrby("score", 1);
                    transaction.exec();
                    break; //执行成功则跳出循环
                }catch(const sw::redis::WatchError &e) {
                    //如果事务执行失败：原因可能会有很多，当前最可能的原因：key值被修改
                    std::cout << "transaction error: " << e.what() << std::endl;
                    //输出错误原因后，重新循环上去，重新进行操作
                } catch (const sw::redis::Error &e) {
                    std::cout << "transaction error: " << e.what() << std::endl;
                    throw;
                }
            }
        }
    } catch (const sw::redis::Error &e) {
        std::cout << "redis error: " << e.what() << std::endl;
    }
    return 0;
}