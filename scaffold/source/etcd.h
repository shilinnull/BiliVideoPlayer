#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>

namespace Bilisvc {
    extern void wait_for_connection(etcd::Client &client);
    class SvcProvider {
        public:
            using ptr = std::shared_ptr<SvcProvider>;
            SvcProvider(const std::string &reg_center_addr, const std::string &svc_name, const std::string &svc_addr);
            bool registry();
        private:
            std::string make_key();
        private:
            std::string _reg_center_addr; //注册中心地址
            std::string _instance_id; //当前节点的标识
            std::string _svc_name;  // 节点能够提供的服务名称
            std::string _svc_addr;  // 节点地址
            std::shared_ptr<etcd::KeepAlive> _keepalive; //租约保活对象
    };
    class SvcWatcher {
        public:
            using ptr = std::shared_ptr<SvcWatcher>;
            //回调函数，在产生事件时，传入两个数据：1-服务名称，2-节点地址
            using ModCallbck = std::function<void(std::string, std::string)>;
            SvcWatcher(const std::string &reg_center_addr, ModCallbck &&online_callback, ModCallbck &&offline_callback);
            // 数据监控并不针对单独数据，而是针对 / 目录
            bool watch();
        private:
            void callback(const etcd::Response &resp);
            std::string parse_key(const std::string &key);// 从key中获取服务名称
        private:
            std::string _reg_center_addr; //注册中心地址
            ModCallbck _online_callback;  //服务上线回调函数
            ModCallbck _offline_callback; //服务下线回调函数
            std::shared_ptr<etcd::Watcher> _watcher; // 服务上下线监控对象
    };
}