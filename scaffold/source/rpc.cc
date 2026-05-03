#include "rpc.h"
#include "log.h"

namespace Bilirpc {
    Channels::Channels() : _idx(0) {};

    void Channels::insert(const std::string& addr) {
        std::unique_lock<std::mutex> lock(_mtx);
        // 实例化Channel对象
        auto channel = std::make_shared<brpc::Channel>();
        brpc::ChannelOptions options;
        options.protocol = "baidu_std";
        options.timeout_ms = 30000;
        channel->Init(addr.c_str(), &options);
        _channels.push_back(std::make_pair(addr, channel));
    }

    void Channels::remove(const std::string& addr) {
        std::unique_lock<std::mutex> lock(_mtx);
        for (auto vit = _channels.begin(); vit != _channels.end(); ++vit) {
            if (addr == vit->first) {
                _channels.erase(vit);
                break;
            }
        }
    }

    ChannelPtr Channels::select() {
        std::unique_lock<std::mutex> lock(_mtx);
        if (_channels.empty()) { return ChannelPtr(); }
        // 获取下标索引
        uint32_t idx = (_idx) % _channels.size();
        return _channels[idx].second;
    }

    std::optional<std::string> Channels::selectAddr() {
        std::unique_lock<std::mutex> lock(_mtx);
        if (_channels.empty()) { return std::optional<std::string>(); }
        uint32_t idx = (_idx) % _channels.size();
        return _channels[idx].first;
    }

    Channels::ptr SvcChannels::_Channels(const std::string& svc_name) {
        std::unique_lock<std::mutex> lock(_mtx);
        auto it = _maps.find(svc_name);
        if (it == _maps.end()) { return Channels::ptr(); }
        return it->second;
    }


    void SvcChannels::setWatch(const std::string& svc_name) {
        std::unique_lock<std::mutex> lock(_mtx);
        auto channels = std::make_shared<Channels>();
        _maps.insert(std::make_pair(svc_name, channels));
    }

    void SvcChannels::addNode(const std::string& svc_name, const std::string& addr) {
        Channels::ptr channels = _Channels(svc_name);
        if (!channels) {
            DBG("{} 有节点上线：{}, 未找到管理对象", svc_name, addr);
            return;
        }
        DBG("添加 {} 服务节点 {} 管理", svc_name, addr);
        return channels->insert(addr);
    }

    void SvcChannels::delNode(const std::string& svc_name, const std::string& addr) {
        Channels::ptr channels = _Channels(svc_name);
        if (!channels) {
            DBG("{} 有节点下线：{}, 未找到管理对象", svc_name, addr);
            return;
        }
        DBG("移除 {} 服务节点 {} 管理", svc_name, addr);
        return channels->remove(addr);
    }

    ChannelPtr SvcChannels::getNode(const std::string& svc_name) {
        Channels::ptr channels = _Channels(svc_name);
        if (!channels) { return ChannelPtr(); }
        return channels->select();
    }

    std::optional<std::string> SvcChannels::getNodeAddr(const std::string& svc_name) {
        Channels::ptr channels = _Channels(svc_name);
        if (!channels) { return std::optional<std::string>(); }
        return channels->selectAddr();
    }


    google::protobuf::Closure* ClosureFactory::create(callback_t&& cb) {
        auto obj = std::make_shared<Object>();
        obj->callback = std::move(cb);
        return brpc::NewCallback(&ClosureFactory::asyncCallback, obj);
    }

    void ClosureFactory::asyncCallback(const Object::ptr obj) { obj->callback(); }

    std::shared_ptr<brpc::Server> RpcServerFactory::create(int port, google::protobuf::Service* svc) {
        brpc::ServerOptions options;
        options.idle_timeout_sec = -1;
        auto server = std::make_shared<brpc::Server>();
        int ret = server->AddService(svc, brpc::SERVER_OWNS_SERVICE);
        if (ret == -1) {
            ERR("添加RPC服务失败!");
            abort();
        }
        ret = server->Start(port, &options);
        if (ret == -1) {
            ERR("启动服务器失败!");
            abort();
        }
        return server;
    }
}
