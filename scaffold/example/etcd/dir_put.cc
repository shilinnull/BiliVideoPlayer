#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>

void put(std::shared_ptr<etcd::KeepAlive> &keepalive, const std::string &url, const std::string &key, const std::string &val) {
    etcd::Client  client(url);
    //2. 创建租约
    auto lease_resp = client.leasegrant(3).get();
    if (lease_resp.is_ok() == false) {
        std::cout << "创建租约失败: " << lease_resp.error_message() << std::endl;
        return ;
    }
    auto lease_id = lease_resp.value().lease();
    //3. 添加数据，并设置租约
    auto resp = client.put("/name/1234", "xiaoming", lease_id).get();
    if (resp.is_ok() == false) {
        std::cout << "添加数据失败: " << resp.error_message() << std::endl;
        return ;
    }
    
    auto handler = [&](const std::exception_ptr &eptr){
        put(keepalive, url, key, val);
    };
    keepalive.reset(new etcd::KeepAlive(url, handler, 3, lease_id));
}

int main()
{
    const std::string url = "http://192.168.65.130:2379";
    const std::string key = "/name/1234";
    const std::string val = "xiaoming";
    std::shared_ptr<etcd::KeepAlive> keepalive;
    put(keepalive, url, key, val);
    std::cout << "回车退出，租约失效...\n";
    getchar();
    return 0;
}