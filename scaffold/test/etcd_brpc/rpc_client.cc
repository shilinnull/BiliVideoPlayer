#include <Bili_scaffold/etcd.h>
#include <Bili_scaffold/rpc.h>
#include <Bili_scaffold/log.h>
#include "cal.pb.h"

int main()
{
    Bililog::Bililog_init();
    std::string reg_center_addr = "htttp://192.168.65.128:2379";
    std::string reg_svc_name = "cal";
    // 0. 实例化服务节点管理对象
    Bilirpc::SvcChannels scs;
    // 1. 添加服务端节点管理： 192.168.65.130:9000
    scs.setWatch(reg_svc_name);
    auto online_cb = std::bind(&Bilirpc::SvcChannels::addNode, &scs, std::placeholders::_1, std::placeholders::_2);
    auto offline_cb = std::bind(&Bilirpc::SvcChannels::delNode, &scs, std::placeholders::_1, std::placeholders::_2);

    Bilisvc::SvcWatcher watcher(reg_center_addr, online_cb, offline_cb);
    watcher.watch();

    // 2. 获取节点对应的channel
    Bilirpc::ChannelPtr channel;
    channel = scs.getNode(reg_svc_name);
    while(!channel){
        ERR("没有可供调用的服务！");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        channel = scs.getNode(reg_svc_name);
    }
    // 3. 发起rpc请求：实例化stub对象，new出req，rsp，异步回调对象
    brpc::Controller* cntl = new brpc::Controller;
    cal::AddReq *request = new cal::AddReq;
    request->set_num1(22);
    request->set_num2(333);
    cal::AddRsp *response = new cal::AddRsp;
    auto closure = Bilirpc::ClosureFactory::create([=](){
        std::unique_ptr<brpc::Controller> cntl_guard(cntl);
        std::unique_ptr<cal::AddReq> req_guard(request);
        std::unique_ptr<cal::AddRsp> rsp_guard(response);
        if (cntl_guard->Failed() == true) {
            std::cout << "rpc请求失败: " << cntl_guard->ErrorText() << std::endl;
            return ;
        }
        std::cout << rsp_guard->result() << std::endl;
    });
    cal::CalService_Stub stub(channel.get());
    stub.Add(cntl, request, response, closure);
    std::cout << "===================================\n";
    getchar();
    return 0;
}