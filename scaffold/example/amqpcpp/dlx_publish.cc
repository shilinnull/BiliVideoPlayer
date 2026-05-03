#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>
#include <iostream>

void declaredComponent(AMQP::TcpChannel& channel, const std::string& exchange, const std::string& queue,
                       const std::string& binding_key) {
    channel.declareExchange(exchange, AMQP::ExchangeType::direct)
           .onSuccess([&]() {
               std::cout << "声明交换机成功:" << exchange << std::endl;
               // 5. 声明队列
               channel.declareQueue(queue)
                      .onSuccess([&](const std::string& name,
                                     uint32_t messagecount,
                                     uint32_t consumercount) {
                          std::cout << "声明队列成功:" << queue << std::endl;
                          std::cout << "队列中有" << messagecount << "个消息" << std::endl;
                          std::cout << "队列中有" << consumercount << "个消费者" << std::endl;

                          // 6. 绑定交换机和队列（采用直接交换）
                          channel.bindQueue(exchange, queue, binding_key)
                                 .onSuccess([&]() { std::cout << "绑定交换机和队列成功:" << std::endl; })
                                 .onError([&](const char* message) {
                                     std::cout << "绑定交换机和队列失败:" << message << std::endl;
                                     abort(); // 终止程序
                                 });
                      })
                      .onError([&](const char* message) {
                          std::cout << "声明队列失败:" << message << std::endl;
                          abort(); // 终止程序
                      });
           })
           .onError([&](const char* message) {
               std::cout << "声明交换机失败:" << message << std::endl;
               abort(); // 终止程序
           });
}

int main() {
    const std::string url = "amqp://admin:123456@192.168.80.128:5672/";
    const std::string delayed_exchange = "delayed-exchange";
    const std::string delayed_queue = "delayed-queue";
    const std::string delayed_binding_key = "delayed-binding-key";
    const std::string dlx_exchange = "dlx-exchange";
    const std::string dlx_queue = "dlx-queue";
    const std::string dlx_binding_key = "dlx-binding-key";

    auto* ev_loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(ev_loop);
    AMQP::TcpConnection connection(&handler, AMQP::Address(url));
    AMQP::TcpChannel channel(&connection);
    //1. 创建普通交换机和队列进行绑定，作为死信交换机队列存在
    declaredComponent(channel, dlx_exchange, dlx_queue, dlx_binding_key);
    //2. 创建常规交换机和队列并进行绑定（创建队列的时候别忘了设置参数-设置消息过期时间，以及关联死信队列）
    channel.declareExchange(delayed_exchange, AMQP::ExchangeType::direct)
           .onSuccess([&]() {
               std::cout << "声明交换机成功:" << delayed_exchange << std::endl;
               // 5. 声明队列
               AMQP::Table args;
               args["x-dead-letter-exchange"] = dlx_exchange;
               args["x-dead-letter-routing-key"] = dlx_binding_key;
               args["x-message-ttl"] = 5000; //过期时间5s
               channel.declareQueue(delayed_queue, args)
                      .onSuccess([&](const std::string& name,
                                     uint32_t messagecount,
                                     uint32_t consumercount) {
                          // 6. 绑定交换机和队列（采用直接交换）
                          channel.bindQueue(delayed_exchange, delayed_queue, delayed_binding_key)
                                 .onSuccess([&]() {
                                     std::cout << "绑定交换机和队列成功,发布消息到延时队列" << std::endl;
                                     //3. 发送消息到常规交换机，消息过期时间到了，消息会被发送到死信交换机，死信交换机会将消息发送到死信队列
                                     channel.publish(delayed_exchange, delayed_binding_key, "hello world");
                                 })
                                 .onError([&](const char* message) {
                                     std::cout << "绑定交换机和队列失败:" << message << std::endl;
                                     abort(); // 终止程序
                                 });
                      });
           });

    ev_run(ev_loop);
    return 0;
}
