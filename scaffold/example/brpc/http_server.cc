#include <butil/logging.h>
#include <brpc/server.h>
#include "cal.pb.h"

class CalServiceTmpl : public Cal::CalService{
public:
    CalServiceTmpl() {}
    ~CalServiceTmpl() {}
    virtual void Add(::google::protobuf::RpcController* controller,
        const ::Cal::AddReq* request,
        ::Cal::AddRsp* response,
        ::google::protobuf::Closure* done) override {
        // 当done_guard被释放的时候执行done->Run()来完成本次rpc调用
        brpc::ClosureGuard done_guard(done);
        int result = request->num1() + request->num2();
        response->set_result(result);
    }
    virtual void Hello(::google::protobuf::RpcController* controller,
        const ::Cal::helloReq* request,
        ::Cal::helloRsp* response,
        ::google::protobuf::Closure* done) override {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = (brpc::Controller*)controller;
        const auto& headers = cntl->http_request();
        std::cout << "Method:" << brpc::HttpMethod2Str(headers.method()) << std::endl;
        std::cout << "Body:" << cntl->request_attachment().to_string() << std::endl;

        cntl->response_attachment().append("回显: " + cntl->request_attachment().to_string());
        cntl->http_response().set_status_code(200);
    }
};

int main(int argc, char* argv[]) {
    // 1. 实例化计算器服务对象
    CalServiceTmpl cal_service;
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
