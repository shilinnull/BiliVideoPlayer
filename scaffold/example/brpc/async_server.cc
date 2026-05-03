#include <butil/logging.h>
#include <brpc/server.h>
#include <thread>
#include "cal.pb.h"

class CalServiceImpl : public Cal::CalService {
public:
    CalServiceImpl() {}
    ~CalServiceImpl() {}
    void Add(::google::protobuf::RpcController* controller,
        const ::Cal::AddReq* request,
        ::Cal::AddRsp* response,
        ::google::protobuf::Closure* done) override {
        std::thread thr([=]() {
            brpc::ClosureGuard done_guard(done);
            int result = request->num1() + request->num2();
            response->set_result(result);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        });
        thr.detach();
        std::cout << "==============================\n";
    }
};

int main(int argc, char* argv[]) {
    // 1. 实例化计算服务对象
    CalServiceImpl cal_service;
    // 2. 定义服务器配置对象
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    // 3. 实例化服务器对象
    brpc::Server server;
    // 4. 向服务器添加服务
    int ret = server.AddService(&cal_service, brpc::SERVER_DOESNT_OWN_SERVICE);
    if (ret == -1) {
        std::cout << "AddService failed" << std::endl;
        return -1;
    }
    // 4. 启动服务器
    ret = server.Start(9000, &options);
    if (ret == -1) {
        std::cout << "Start failed" << std::endl;
        return -1;
    }
    // 5. 等待服务器停止
    server.RunUntilAskedToQuit();
    return 0;
}
