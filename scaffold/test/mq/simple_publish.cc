#include "../../source/mq.h"
#include "../../source/log.h"

int main() 
{
    Bililog::Bililog_init();
    // 1. 定义代理服务器URL
    const std::string url = "amqp://admin:123456@192.168.80.128:5672/";
    // 2. 定义声明配置信息
    Bilimq::declare_settings settings = {
        .exchange = "test-exchange",
        .exchange_type = "direct",
        .queue = "test-queue",
        .binding_key = "test-binding-key"
    };
    // 3. 实例化MQ客户端对象
    auto mq_client = Bilimq::MQFactory::create<Bilimq::MQClient>(url);
    // 4. 实例化消息发布者对象
    auto publisher = Bilimq::MQFactory::create<Bilimq::Publisher>(mq_client, settings);
    // 5. 消息发布 
    publisher->publish("hello world");
    return 0;
}