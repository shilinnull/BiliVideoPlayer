#include <brpc/channel.h>
#include "cal.pb.h"

using callback_t = std::function<void()>;
struct Object {
    callback_t callback;
};

void Callback(const Object obj) {
    obj.callback();
}

int main(int argc, char* argv[]) {
    brpc::ChannelOptions options;
    options.protocol = "baidu_std";
    brpc::Channel channel;
    channel.Init("192.168.80.128:9000", &options);
    Cal::CalService_Stub stub(&channel);
    brpc::Controller *cntl = new brpc::Controller;
    cntl->set_timeout_ms(4000);
    Cal::AddReq *request = new Cal::AddReq;
    Cal::AddRsp *response = new Cal::AddRsp;
    request->set_num1(10);
    request->set_num2(20);
    Object obj;
    obj.callback = [=]() {
        std::unique_ptr<brpc::Controller> cntl_guard(cntl);
        std::unique_ptr<Cal::AddReq> req_guard(request);
        std::unique_ptr<Cal::AddRsp> rsp_guard(response);
        if (cntl_guard->Failed() == true) {
            std::cout << "rpc请求失败: " << cntl_guard->ErrorText() << std::endl;
            return;
        }
        std::cout << rsp_guard->result() << std::endl;
    };
    google::protobuf::Closure* closure = brpc::NewCallback(Callback, obj);
    stub.Add(cntl, request, response, closure);
    std::cout << "=========================\n" << std::endl;
    getchar();
    return 0;
}
