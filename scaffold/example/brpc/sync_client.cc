#include <brpc/channel.h>
#include "cal.pb.h"

int main(int argc, char *argv[])
{
    // 0. 实例化ChannelOptions进行参数配置
    brpc::ChannelOptions options;
    options.protocol = "baidu_std";
    // 1. 实例化Channel信道对象
    brpc::Channel chennel;
    chennel.Init("192.168.80.128:9000", &options);
    // 3. 实例化CalService_stub对象--用于发起rpc请求。
    Cal::CalService_Stub stub(&chennel);
    brpc::Controller cntl;
    Cal::AddReq request;
    Cal::AddRsp response;
    request.set_num1(10);
    request.set_num2(20);
    stub.Add(&cntl, &request, &response, NULL);
    if (cntl.Failed() == true) {
        std::cout << "rpc请求失败: " << cntl.ErrorText() << std::endl;
        return -1;
    }
    std::cout << response.result() << std::endl;
    return 0;
}