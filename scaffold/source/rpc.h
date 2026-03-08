#ifndef VIDEOPLAYER_RPC_H
#define VIDEOPLAYER_RPC_H

#include <butil/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include <mutex>
#include <optional>

namespace Bilirpc {
    using ChannelPtr = std::shared_ptr<brpc::Channel>;

    // Channel集合类
    class Channels {
    public:
        using ptr = std::shared_ptr<Channels>;
        Channels();
        void insert(const std::string& addr); // 新增节点
        void remove(const std::string& addr); // 删除节点
        ChannelPtr select(); // 获取节点信道
        std::optional<std::string> selectAddr(); // 获取节点地址
    private:
        std::mutex _mtx;
        std::string _svc_name;
        uint32_t _idx;
        std::vector<std::pair<std::string, ChannelPtr>> _channels;
    };

    // Channel管理类
    class SvcChannels {
    public:
        using Ptr = std::shared_ptr<SvcChannels>;
        SvcChannels() = default;
        void setWatch(const std::string& svc_name);
        void addNode(const std::string& svc_name, const std::string& addr);
        void delNode(const std::string& svc_name, const std::string& addr);
        ChannelPtr getNode(const std::string& svc_name);
        std::optional<std::string> getNodeAddr(const std::string& svc_name);

    private:
        Channels::ptr _Channels(const std::string& svc_name);

    private:
        std::mutex _mtx;
        std::unordered_map<std::string, Channels::ptr> _maps;
    };

    class ClosureFactory {
    public:
        using callback_t = std::function<void()>;
        static google::protobuf::Closure* create(callback_t&& cb);

    private:
        struct Object {
            using ptr = std::shared_ptr<Object>;
            callback_t callback;
        };

        static void asyncCallback(const Object::ptr obj);
    };

    class RpcServerFactory {
    public:
        static std::shared_ptr<brpc::Server> create(int port, google::protobuf::Service* svc);
    };
}


#endif //VIDEOPLAYER_RPC_H
