#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>

void callback(const etcd::Response &resp) {
    if (resp.is_ok() == false) {
        std::cout << "监控出错: " << resp.error_message() << std::endl;
        return;
    }
    const auto &events = resp.events();
    for (auto &e : events) {
        if (e.event_type() == etcd::Event::EventType::PUT) {
            std::cout << e.kv().key() << "数据改变:" << e.prev_kv().as_string() << "->" << e.kv().as_string() << std::endl;
        }else if (e.event_type() == etcd::Event::EventType::DELETE_) {
            std::cout << e.prev_kv().key() << "数据删除!" << std::endl;
        }else {
            std::cout << "无效类型\n";
        }
    }
}

void get(std::shared_ptr<etcd::Watcher> &watcher, const std::string &url, const std::string &key) {
    etcd::Client  client(url);
    //浏览目录下的数据 -- ls
    auto resp = client.ls("/name").get();
    if (resp.is_ok() == true) {
        auto values = resp.values(); //获取结果数据
        for (auto &v : values) {
            std::cout << v.key() << " = " << v.as_string() << std::endl;
        }
    }
    //数据监控
    watcher.reset( new etcd::Watcher(url, "/", callback, true)); //true表示递归监控目录下所有数据
    watcher->Wait([&](bool cond) {
        if (cond == true) {
            //当前任务是被主动取消的
            return;
        }
        get(watcher, url, key);
    });
}

int main()
{
    const std::string url = "http://192.168.80.128:2379";
    const std::string key = "/";
    std::shared_ptr<etcd::Watcher> watcher;
    get(watcher, url, key);
    std::cout << "回车退出...\n";
    getchar();
    return 0;
}