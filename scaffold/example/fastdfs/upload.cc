#include <fastcommon/logger.h>
#include <fastdfs/fdfs_client.h>
#include <iostream>

int main()
{
    // 1. 初始化日志输出模块
    g_log_context.log_level = LOG_ERR;
    log_init();
    // 2. 初始化客户端全局配置（1. 从配置文件进行初始化/ 2. 从缓冲区进行初始化）
    int ret = fdfs_client_init("./client.conf");
    if (ret != 0) {
        std::cout << "初始化全局配置失败: " << STRERROR(ret) << std::endl;
        return -1;
    }
    // 3. 获取tracker服务器连接句柄
    auto tracker_server = tracker_get_connection();
    if (tracker_server == nullptr) {
        std::cout << "获取tracker服务器连接句柄失败" << std::endl;
        return -1;
    }
    // 4. 进行文件的上传/下载（可以手动获取一个storage节点连接然后进行操作）
    char file_id[256];
    ret = storage_upload_by_filename1(tracker_server, nullptr, 0, "./makefile", nullptr, nullptr, 0, nullptr, file_id);
    if (ret != 0) {
        std::cout << "文件上传失败: " << STRERROR(ret) << std::endl;
        return -1;
    }
    std::cout << "文件上传成功: " << file_id << std::endl;
    // 5. 释放资源（关闭tracker服务器连接句柄， 销毁客户端全局配置）
    tracker_close_connection(tracker_server);
    fdfs_client_destroy();
    return 0;
}