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
                                 .onSuccess([&]() {
                                     std::cout << "绑定交换机和队列成功:" << std::endl;
                                     channel.consume(queue)
                                            .onMessage([&](const AMQP::Message& message, //收到的消息
                                                           uint64_t deliveryTag, //消息的唯一标识
                                                           bool redelivered) {
                                                std::string body(message.body(), message.bodySize());
                                                std::cout << "收到消息:" << body << std::endl;
                                                // 7.5 收到消息进行处理后，不要忘了对消息进行确认
                                                channel.ack(deliveryTag);
                                            })
                                            .onError([&](const char* message) {
                                                std::cout << "订阅队列消息失败:" << message << std::endl;
                                                abort(); // 终止程序
                                            })
                                            .onSuccess([&]() { std::cout << "订阅队列消息成功:" << std::endl; });
                                 })
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
    const std::string dlx_exchange = "dlx-exchange";
    const std::string dlx_queue = "dlx-queue";
    const std::string dlx_binding_key = "dlx-binding-key";

    auto* ev_loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(ev_loop);
    AMQP::TcpConnection connection(&handler, AMQP::Address(url));
    AMQP::TcpChannel channel(&connection);
    //1. 创建普通交换机和队列进行绑定，作为死信交换机队列存在
    declaredComponent(channel, dlx_exchange, dlx_queue, dlx_binding_key);
    //2. 订阅死信队列消息
    ev_run(ev_loop);

    return 0;
}
