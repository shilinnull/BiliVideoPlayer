#include <brpc/channel.h>
#include "cal.pb.h"

int main(int argc, char *argv[]) {
    // 0. 实例化ChannelOptions进行参数配置,设置协议为HTTP协议请求
    brpc::ChannelOptions channelOptions;
    channelOptions.protocol = brpc::PROTOCOL_HTTP;
    // 1. 实例化Channel信道对象
    brpc::Channel channel;
    channel.Init("192.168.80.128:9000", &channelOptions);
    // 2. 实例化CalService_stub对象 用于发起rpc请求。
    brpc::Controller cntl;
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.http_request().uri().set_path("/CalService/Hello");
    cntl.http_request().SetHeader("Content-Type", "text/plain");
    cntl.request_attachment().append("Hello world");
    channel.CallMethod(nullptr, &cntl, nullptr, nullptr, nullptr);
    if (cntl.Failed() == false){
        std::cout << cntl.response_attachment() << std::endl;
    }else {
        std::cout << cntl.ErrorText() << std::endl;
    }
    return 0;
}

