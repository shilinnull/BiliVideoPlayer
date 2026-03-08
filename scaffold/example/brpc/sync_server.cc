#include <butil/logging.h>
#include <brpc/server.h>
#include "cal.pb.h"

class CalServiceTmp : public Cal::CalService {
public:
    CalServiceTmp() {}
    ~CalServiceTmp() {}
    void Add(::google::protobuf::RpcController* controller,
        const ::Cal::AddReq* request,
        ::Cal::AddRsp* response,
        ::google::protobuf::Closure* done) override {
        brpc::ClosureGuard done_guard(done);
        int result = request->num1() + request->num2();
        response->set_result(result);
        std::cout << "计算：" << request->num1() << " + " << request->num2() << " = " << result << std::endl;
    }
};

int main(int argc, char* argv[]) {
    CalServiceTmp calService;
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    brpc::Server server;
    int ret = server.AddService(&calService, brpc::SERVER_DOESNT_OWN_SERVICE);
    if (ret == -1) {
        std::cout << "AddService failed" << std::endl;
        return -1;
    }
    ret = server.Start(9000, &options);
    if (ret == -1) {
        std::cout << "Start failed" << std::endl;
        return -1;
    }
    server.RunUntilAskedToQuit();
    return 0;
}