#pragma once
#include <iostream>
#include <vector>
#include <optional>
extern "C" {
#include <fastdfs/fdfs_client.h>
#include <fastcommon/logger.h>
}

namespace Bilifdfs {
    struct fdfs_settings {
        std::vector<std::string> trackers;
        int connect_timeout = 30;
        int network_timeout = 30;
        bool use_connection_pool = true;
        int connection_pool_max_idle_time = 3600;
    };

    class FDFSClient {
        public:
            // 初始化客户端全局配置
            static void init(const fdfs_settings &settings);
            static void destroy();
            static std::optional<std::string> upload_from_buff(const std::string &buff);
            static std::optional<std::string> upload_from_file(const std::string &path);
            static bool download_to_buff(const std::string &file_id, std::string &buff);
            static bool download_to_file(const std::string &file_id, const std::string &path);
            static bool remove(const std::string &file_id);
    };
}