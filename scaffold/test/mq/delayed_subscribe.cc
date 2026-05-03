#include "../../source/mq.h"
#include "../../source/log.h"

int main() 
{
    Bililog::Bililog_init();
    // 1. 定义代理服务器URL
    const std::string url = "amqp://admin:123456@192.168.80.128:5672/";
    // 2. 定义声明配置信息
    Bilimq::declare_settings settings = {
        .exchange = "delayed-exchange",
        .exchange_type = "delayed",
        .queue = "delayed-queue",
        .binding_key = "delayed-binding-key",
        .delayed_ttl = 3000
    };
    // 3. 实例化MQ客户端对象
    auto mq_client = Bilimq::MQFactory::create<Bilimq::MQClient>(url);
    // 4. 实例化消息订阅者对象
    auto subscriber = Bilimq::MQFactory::create<Bilimq::Subscriber>(mq_client, settings);
    // 5. 订阅队列消息 
    subscriber->consume([](const char* msg, size_t len) {
        INF("收到消息: {}", std::string(msg, len));
    });
    mq_client->wait();
    return 0;
}