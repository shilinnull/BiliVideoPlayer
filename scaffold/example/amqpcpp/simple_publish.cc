#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>
#include <iostream>

using namespace std;

int main() {
    const std::string url = "amqp://admin:123456@192.168.80.128:5672/";
    const std::string exchange = "my-exchange";
    const std::string queue = "my-queue";
    const std::string binding_key = "my-binding-key";
    // 实例化libev网络通信模块句柄
    auto* ev_loop = EV_DEFAULT; //网络通信事件循环句柄
    AMQP::LibEvHandler handler(ev_loop); //与amqpcpp库结合的句柄
    // 实例化Connection对象
    AMQP::TcpConnection connection(&handler, AMQP::Address(url));
    AMQP::TcpChannel channel(&connection);
    channel.declareExchange(exchange, AMQP::ExchangeType::direct)
           .onSuccess([&] {
               std::cout << "声明交换机成功:" << exchange << std::endl;
               channel.declareQueue(queue).onSuccess([&](const std::string& name,
                                                         uint32_t messagecount,
                                                         uint32_t consumercount) {
                   std::cout << "声明队列成功:" << queue << std::endl;
                   std::cout << "队列中有" << messagecount << "个消息" << std::endl;
                   std::cout << "队列中有" << consumercount << "个消费者" << std::endl;
                   channel.bindQueue(exchange, queue, binding_key).onSuccess([&]() {
                       std::cout << "绑定交换机和队列成功:" << std::endl;
                       bool ret = channel.publish(exchange, binding_key, "hello world");
                       if (ret == false) { std::cout << "发送消息失败!" << std::endl; }
                   }).onError([&](const char* message) {
                       std::cout << "绑定交换机和队列失败:" << message << std::endl;
                       abort(); // 终止程序
                   });
               }).onError([&](const char* message) {
                   std::cout << "声明队列失败:" << message << std::endl;
                   abort(); // 终止程序
               });
           }).onError([&](const char* message) {
               std::cout << "声明交换机失败:" << message << std::endl;
               abort(); // 终止程序
           });
    ev_run(ev_loop);

    return 0;
}
