#include <sstream>
#include "log.h"
#include "fdfs.h"

namespace Bilifdfs {
    void FDFSClient::init(const fdfs_settings &settings) {
        g_log_context.log_level = LOG_ERR;
        log_init();
        // 针对settings中的配置，组成一个配置信息字符串
        std::stringstream ss;
        for (auto &tracker : settings.trackers) {
            ss << "tracker_server = " << tracker << "\n";
        }
        ss << "connect_timeout = " << settings.connect_timeout << "\n";
        ss << "network_timeout = " << settings.network_timeout << "\n";
        ss << "use_connection_pool = " << settings.use_connection_pool << "\n";
        ss << "connection_pool_max_idle_time = " << settings.connection_pool_max_idle_time << "\n";
        int ret = fdfs_client_init_from_buffer(ss.str().c_str());
        if (ret != 0) {
            ERR("初始化全局配置失败: {}", STRERROR(ret));
            abort();
        }
    }
    void FDFSClient::destroy() {
        fdfs_client_destroy();
    }
    std::optional<std::string> FDFSClient::upload_from_buff(const std::string &buff) {
        auto tracker_server = tracker_get_connection();
        if (tracker_server == nullptr) {
            ERR("获取tracker服务器连接句柄失败");
            return std::optional<std::string>();
        }
        char file_id[256];
        int ret = storage_upload_by_filebuff1(tracker_server, nullptr, 0, buff.c_str(), buff.size(), nullptr, nullptr, 0, nullptr, file_id);
        if (ret!= 0) {
            ERR("文件上传失败: {}", STRERROR(ret));
            return std::optional<std::string>();
        }
        
        tracker_close_connection(tracker_server);
        return std::string(file_id);
    }
    std::optional<std::string> FDFSClient::upload_from_file(const std::string &path) {
        auto tracker_server = tracker_get_connection();
        if (tracker_server == nullptr) {
            ERR("获取tracker服务器连接句柄失败");
            return std::optional<std::string>();
        }
        char file_id[256];
        int ret = storage_upload_by_filename1(tracker_server, nullptr, 0, path.c_str(), nullptr, nullptr, 0, nullptr, file_id);
        if (ret != 0) {
            ERR("文件上传失败: {}", STRERROR(ret));
            return std::optional<std::string>();
        }
        tracker_close_connection(tracker_server);
        return std::string(file_id);
    }
    bool FDFSClient::download_to_buff(const std::string &file_id, std::string &buff) {
        auto tracker_server = tracker_get_connection();
        if (tracker_server == nullptr) {
            ERR("获取tracker服务器连接句柄失败");
            return false;
        }
        int64_t file_size;
        char *file_buff = nullptr;
        int ret = storage_download_file1(tracker_server, nullptr, file_id.c_str(), &file_buff, &file_size);
        if (ret!= 0) {
            ERR("文件下载失败: {}", STRERROR(ret));
            return false;
        }
        buff.assign(file_buff, file_size);
        free(file_buff);
        tracker_close_connection(tracker_server);
        return true;
    }
    bool FDFSClient::download_to_file(const std::string &file_id, const std::string &path) {
        auto tracker_server = tracker_get_connection();
        if (tracker_server == nullptr) {
            ERR("获取tracker服务器连接句柄失败");
            return false;
        }
        int64_t file_size;
        int ret = storage_download_file_to_file1(tracker_server, nullptr, file_id.c_str(), path.c_str(), &file_size);
        if (ret != 0) {
            ERR("文件下载失败: {}", STRERROR(ret));
            return false;
        }
        tracker_close_connection(tracker_server);
        return true;
    }
    bool FDFSClient::remove(const std::string &file_id) {
        auto tracker_server = tracker_get_connection();
        if (tracker_server == nullptr) {
            ERR("获取tracker服务器连接句柄失败");
            return false;
        }
        int ret = storage_delete_file1(tracker_server, nullptr, file_id.c_str());
        if (ret != 0) {
            ERR("文件删除失败: {}", STRERROR(ret));
            return false;
        }
        tracker_close_connection(tracker_server);
        return true;
    }
}