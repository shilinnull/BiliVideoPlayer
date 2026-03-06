#include "mq.h"
#include "log.h"

namespace Bilimq {
    std::string declare_settings::dlx_exchange() const { return DLX_PREFIX + exchange; }
    std::string declare_settings::dlx_queue() const { return DLX_PREFIX + queue; }
    std::string declare_settings::dlx_binding_key() const { return DLX_PREFIX + binding_key; }

    AMQP::ExchangeType exchange_type(const std::string& type) {
        if (type == DIRECT) { return AMQP::ExchangeType::direct; }
        else if (type == FANOUT) { return AMQP::ExchangeType::fanout; }
        else if (type == TOPIC) { return AMQP::ExchangeType::topic; }
        else if (type == HEADERS) { return AMQP::ExchangeType::headers; }
        else if (type == DELAYED) { return AMQP::ExchangeType::direct; }
        ERR("交换机类型错误: {}", type);
        abort();
    }

    // 构造成员，启动事件循环
    MQClient::MQClient(const std::string& url)
        : _ev_loop(EV_DEFAULT)
          , _handler(_ev_loop)
          , _connection(&_handler, AMQP::Address(url))
          , _channel(&_connection)
          , _async_thread(std::thread([this]() { ev_run(_ev_loop); })) {}

    // 发送异步请求，结束事件循环，等待异步线程结束
    MQClient::~MQClient() {
        ev_async_init(&_ev_async, MQClient::callback);
        ev_async_start(_ev_loop, &_ev_async);
        ev_async_send(_ev_loop, &_ev_async);
        _async_thread.join(); // 等待异步线程结束
    }

    void MQClient::callback(struct ev_loop* loop, ev_async* watcher, int32_t revents) {
        //这个接口不能跨线程调用
        ev_break(loop, EVBREAK_ALL);
    }

    //声明交换机，声明队列，并绑定交换机和队列，如果是延时队列，则还要创建配套的死信交换机和队列
    //声明交换机和队列以及绑定成功后，需要等待，等待实际交换机和队列声明成功，再返回
    void MQClient::_declared(const declare_settings& settings, AMQP::Table& args, bool is_dlx) {
        std::unique_lock<std::mutex> declared_lock(_declared_mtx); //这个锁用于互斥
        std::unique_lock<std::mutex> lock(_mtx); //这个锁用于进行条件控制
        //1. 根据参数is_dlx，判断是否是死信队列，取出要使用的的交换机名称，队列名称，绑定关键字
        std::string exchange, queue, binding_key;
        if (is_dlx) {
            exchange = settings.dlx_exchange();
            queue = settings.dlx_queue();
            binding_key = settings.dlx_binding_key();
        }
        else {
            exchange = settings.exchange;
            queue = settings.queue;
            binding_key = settings.binding_key;
        }
        AMQP::ExchangeType type = exchange_type(settings.exchange_type);
        // 声明交换机，声明队列，并绑定交换机和队列
        _channel.declareExchange(exchange, type)
                .onSuccess([=, this]() {
                    _channel.declareQueue(queue, args)
                            .onSuccess([=, this](const std::string& name,
                                                 uint32_t messagecount,
                                                 uint32_t consumercount) {
                                _channel.bindQueue(exchange, queue, binding_key)
                                        .onSuccess([=, this]() {
                                            // 声明成功，通知外部
                                            // std::unique_lock<std::mutex> lock(_mtx);
                                            _cv.notify_all();
                                        })
                                        .onError([=, this](const char* message) {
                                            ERR("绑定队列失败: {}-{} - {}", exchange, queue, message);
                                            _cv.notify_all();
                                            abort();
                                        });
                            })
                            .onError([=, this](const char* message) {
                                ERR("声明队列失败: {} - {}", queue, message);
                                _cv.notify_all();
                                abort(); // 终止程序
                            });
                })
                .onError([=, this](const char* message) {
                    ERR("声明交换机失败: {} - {}", exchange, message);
                    _cv.notify_all();
                    abort(); // 终止程序
                });
        _cv.wait(lock);
    }

    void MQClient::declare(const declare_settings& settings) {
        //1. 判断当前是否是延时队列
        AMQP::Table args;
        if (settings.exchange_type == DELAYED) {
            //声明死信交换机和队列，并进行绑定
            _declared(settings, args, true);
            // 声明常规交换机和队列，并进行绑定
            args[DEAD_LETTER_EXCHANGE] = settings.dlx_exchange();
            args[DEAD_LETTER_BINDING_KEY] = settings.dlx_binding_key();
            args[MESSAGE_TTL] = settings.delayed_ttl; //过期时间5s
        }
        _declared(settings, args, false);
    }

    bool MQClient::publish(const std::string& exchange, const std::string& routing_key, const std::string& body) {
        return _channel.publish(exchange, routing_key, body);
    }

    void MQClient::consume(const std::string& queue, const MessageCallback& callback) {
        std::unique_lock<std::mutex> declared_lock(_declared_mtx);
        std::unique_lock<std::mutex> lock(_mtx); //这个锁用于进行条件控制
        _channel.consume(queue)
                .onMessage([=, this](const AMQP::Message& message, //收到的消息
                                     uint64_t deliveryTag, //消息的唯一标识
                                     bool redelivered) {
                    callback(message.body(), message.bodySize());
                    // 7.5 收到消息进行处理后，不要忘了对消息进行确认
                    _channel.ack(deliveryTag);
                })
                .onError([&](const char* message) {
                    ERR("订阅队列消息失败: {}", message);
                    _cv.notify_all();
                    abort(); // 终止程序
                })
                .onSuccess([&]() {
                    DBG("订阅队列消息成功: {}", queue);
                    _cv.notify_all();
                });
        _cv.wait(lock);
    }

    // 有些场景下，我们程序只进行消息的处理，没有其他业务，因此就希望能够阻塞当前主线程
    void MQClient::wait() { _async_thread.join(); }

    //构造函数内，对成员进行初始化，并声明套件内的交换机和队列
    Publisher::Publisher(const MQClient::ptr& mq_client, const declare_settings& settings)
        : _mq_client(mq_client)
          , _settings(settings) { _mq_client->declare(_settings); }

    bool Publisher::publish(const std::string& body) {
        //发布消息的时候，不管是延时队列，还是普通队列，都是将消息发布到普通交换机
        return _mq_client->publish(_settings.exchange, _settings.binding_key, body);
    }

    //构造函数内，对成员进行初始化，并声明套件内的交换机和队列
    Subscriber::Subscriber(const MQClient::ptr& mq_client, const declare_settings& settings)
        : _mq_client(mq_client)
          , _settings(settings) { _mq_client->declare(_settings); }

    //订阅消息的时候，要注意：如果是延时队列，则实际订阅的是配套的死信队列
    void Subscriber::consume(MessageCallback&& cb) {
        _callback = std::move(cb);
        //如果是延时队列，顶订阅的是死信队列消息，否则订阅的是常规队列消息
        if (_settings.exchange_type == DELAYED) { _mq_client->consume(_settings.dlx_queue(), _callback); }
        else { _mq_client->consume(_settings.queue, _callback); }
    }
} // Bilimq
