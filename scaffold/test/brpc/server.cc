#include "../../source/log.h"
#include "../../source/rpc.h"
#include "cal.pb.h"

class CalServiceImpl : public cal::CalService {
public:
    CalServiceImpl() {}
    ~CalServiceImpl() {}
    void Add(::google::protobuf::RpcController* controller,
        const ::cal::AddReq* request,
        ::cal::AddRsp* response,
        ::google::protobuf::Closure* done) override {
        //当done_guard被释放的时候执行done->Run()来完成本次rpc调用
        brpc::ClosureGuard done_guard(done);
        int result = request->num1() + request->num2();
        response->set_result(result);
    }
    void Hello(::google::protobuf::RpcController* controller,
        const ::cal::helloReq* request,
        ::cal::helloRsp* response,
        ::google::protobuf::Closure* done) override {
        //别忘了设置Closure管理
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = (brpc::Controller*)controller;
        const auto& headers = cntl->http_request();
        std::cout << "Method:" << brpc::HttpMethod2Str(headers.method()) << std::endl;
        std::cout << "Body:" << cntl->request_attachment().to_string() << std::endl;

        cntl->response_attachment().append("回显:" + cntl->request_attachment().to_string());
        cntl->http_response().set_status_code(200);
    }
};

int main(int argc, char** argv) {
    CalServiceImpl* cal_service = new CalServiceImpl();
    auto server = Bilirpc::RpcServerFactory::create(9000, cal_service);
    server->RunUntilAskedToQuit();
}
