#include "etcd.h"
#include "log.h"
#include "util.h"
#include <thread>
#include <sstream>

namespace Bilisvc {
void wait_for_connection(etcd::Client &client) {
    // wait until the client connects to etcd server
    while (!client.head().get().is_ok()) {
        WRN("连接etcd服务器失败...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

SvcProvider::SvcProvider(const std::string &reg_center_addr,
    const std::string &svc_name, const std::string &svc_addr)
    : _reg_center_addr(reg_center_addr)
    , _instance_id(Biliutil::Random::code())
    , _svc_name(svc_name)
    , _svc_addr(svc_addr) {}
std::string SvcProvider::make_key() {
    std::stringstream ss;
    ss << "/" << _svc_name << "/" << _instance_id;
    return ss.str();
}
bool SvcProvider::registry() {
    //1. 实例化etcd客户端对象
    etcd::Client  client(_reg_center_addr);
    wait_for_connection(client); //等待连接服务器成功
    //2. 创建3s租约
    auto lease_resp = client.leasegrant(3).get();
    if (lease_resp.is_ok() == false) {
        ERR("创建租约失败: {}", lease_resp.error_message());
        return false;
    }
    auto lease_id = lease_resp.value().lease();
    //3. 重组key值, 根据租约向服务器添加数据
    auto resp = client.put(make_key(), _svc_addr, lease_id).get();
    if (resp.is_ok() == false) {
        ERR("添加数据失败: {}", lease_resp.error_message());
        return false;
    }
    //5. 实例化保活对象，对租约进行保活
    auto handler = [this](const std::exception_ptr &eptr){
        this->registry();
    };
    _keepalive.reset(new etcd::KeepAlive(_reg_center_addr, handler, 3, lease_id));
    return true;
}

SvcWatcher::SvcWatcher(const std::string &reg_center_addr,
    SvcWatcher::ModCallbck &&online_callback, 
    SvcWatcher::ModCallbck &&offline_callback)
    : _reg_center_addr(reg_center_addr)
    , _online_callback(std::move(online_callback))
    , _offline_callback(std::move(offline_callback)){ }
    
std::string SvcWatcher::parse_key(const std::string &key) {
    // /user/instance_id
    std::vector<std::string> arry;
    Biliutil::STR::split(key, "/", arry);
    return arry[0];
}
void SvcWatcher::callback(const etcd::Response &resp) {
    if (resp.is_ok() == false) {
        ERR("监控出错: {}", resp.error_message());
        return;
    }
    const auto &events = resp.events();
    for (auto &e : events) {
        if (e.event_type() == etcd::Event::EventType::PUT) {
            std::string svc_name = parse_key(e.kv().key());
            std::string svc_addr = e.kv().as_string();
            DBG(" {} 服务上线节点: {}", svc_name, svc_addr);
            if (_online_callback) _online_callback(svc_name, svc_addr);
        }else if (e.event_type() == etcd::Event::EventType::DELETE_) {
            std::string svc_name = parse_key(e.prev_kv().key());
            std::string svc_addr = e.prev_kv().as_string();
            DBG(" {} 服务下线节点: {}", svc_name, svc_addr);
            if (_offline_callback) _offline_callback(svc_name, svc_addr);
        }else {
            WRN("无效通知类型");
        }
    }
}
// 数据监控并不针对单独数据，而是针对 / 目录
bool SvcWatcher::watch() {
    //1. 先实例化etcd客户端对象，并等待连接成功
    etcd::Client  client(_reg_center_addr);
    wait_for_connection(client); //等待连接服务器成功
    //2. 浏览根目录，获取当前所有已经提供服务的节点信息
    auto resp = client.ls("/").get();
    if (resp.is_ok() == true) {
        auto values = resp.values(); //获取结果数据
        for (auto &v : values) {
            std::string svc_name = parse_key(v.key());
            std::string svc_addr = v.as_string();
            DBG("已有 {} 节点: {}", svc_name, svc_addr);
            if (_online_callback) _online_callback(svc_name, svc_addr);
        }
    }
    //3. 监控根目录，获取服务上下线的通知进行对应处理
    auto cb = std::bind(&SvcWatcher::callback, this, std::placeholders::_1);
    _watcher.reset( new etcd::Watcher(_reg_center_addr, "/", cb, true)); //true表示递归监控目录下所有数据
    _watcher->Wait([this](bool cond) {
        if (cond == true) { return; }
        this->watch();
    });
    return true;
}

}