#ifndef VIDEOPLAYER_MQ_H
#define VIDEOPLAYER_MQ_H

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Bilimq {
    const std::string DIRECT = "direct";
    const std::string FANOUT = "fanout";
    const std::string TOPIC = "topic";
    const std::string HEADERS = "headers";
    const std::string DELAYED = "delayed";
    const std::string DLX_PREFIX = "dlx_";
    const std::string DEAD_LETTER_EXCHANGE = "x-dead-letter-exchange";
    const std::string DEAD_LETTER_BINDING_KEY = "x-dead-letter-routing-key";
    const std::string MESSAGE_TTL = "x-message-ttl";
    struct declare_settings {
        std::string exchange;
        // 交换机类型： direct、fanout、topic、headers、delayed
        std::string exchange_type;
        std::string queue;
        std::string binding_key;
        size_t delayed_ttl = 0;

        std::string dlx_exchange() const ;
        std::string dlx_queue() const;
        std::string dlx_binding_key() const ;
    };
    extern AMQP::ExchangeType exchange_type(const std::string &type);
    using MessageCallback = std::function<void(const char*, size_t)>;

    class MQClient {
    public:
        using ptr = std::shared_ptr<MQClient>;
        MQClient(const std::string &url); // 构造成员，启动事件循环
        ~MQClient(); // 发送异步请求，结束事件循环，等待异步线程结束
        //声明交换机，声明队列，并绑定交换机和队列，如果是延时队列，则还要创建配套的死信交换机和队列
        //声明交换机和队列以及绑定成功后，需要等待，等待实际交换机和队列声明成功，再返回
        void declare(const declare_settings &settings);
        bool publish(const std::string &exchange, const std::string &routing_key, const std::string &body);
        void consume(const std::string &queue, const MessageCallback &callback);
        void start();
        void wait();
    private:
        static void callback(struct ev_loop *loop, ev_async *watcher, int32_t revents);
        // 声明常规交换机和队列，并进行绑定
        void _declared(const declare_settings &settings, AMQP::Table &args, bool is_dlx = false);
    private:
        std::mutex _declared_mtx;
        std::mutex _mtx;
        std::condition_variable _cv;
        struct ev_loop *_ev_loop;
        struct ev_async _ev_async;
        AMQP::LibEvHandler _handler;
        AMQP::TcpConnection _connection;
        AMQP::TcpChannel _channel;
        std::thread _async_thread;
    };

    class Publisher {
    public:
        using ptr = std::shared_ptr<Publisher>;
        //构造函数内，对成员进行初始化，并声明套件内的交换机和队列
        Publisher(const MQClient::ptr &mq_client, const declare_settings &settings);
        bool publish(const std::string &body);
    private:
        MQClient::ptr _mq_client;
        declare_settings _settings;
    };

    class Subscriber {
    public:
        using ptr = std::shared_ptr<Subscriber>;
        //构造函数内，对成员进行初始化，并声明套件内的交换机和队列
        Subscriber(const MQClient::ptr &mq_client, const declare_settings &settings);
        //订阅消息的时候，要注意：如果是延时队列，则实际订阅的是配套的死信队列
        void consume(MessageCallback &&cb);
    private:
        MQClient::ptr _mq_client;
        declare_settings _settings;
        MessageCallback _callback;
    };

    class MQFactory {
    public:
        // 提供一个通用的工厂实现各个不同类的对象的创建
        template<typename R, typename... Args>
        static std::shared_ptr<R> create(Args&&... args) {
            return std::make_shared<R>(std::forward<Args>(args)...);
        }
    };

} // Bilimq

#endif //VIDEOPLAYER_MQ_H