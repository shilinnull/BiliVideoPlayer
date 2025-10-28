#include "log.h"
namespace my_log {
std::shared_ptr<spdlog::logger> g_logger;
void my_log_init(const log_settings &settings) {
    //1. 判断日志器类型 - async/sync
    //2. 判断输出目标 -- stdout, file
    //3. 创建日志器
    if (settings.async == true) {
        if (settings.path == "stdout") {
            g_logger = spdlog::stdout_color_mt<spdlog::async_factory>("stdout_logger");
        } else {
            g_logger = spdlog::basic_logger_mt<spdlog::async_factory>("file_logger", settings.path);
        }
    } else {
        if (settings.path == "stdout") {
            g_logger = spdlog::stdout_color_mt("stdout_logger");
        } else {
            g_logger = spdlog::basic_logger_mt("file_logger", settings.path);
        }
    }
    //4. 设置日志等级
    g_logger->set_level(spdlog::level::level_enum(settings.level));
    //5. 设置日志格式
    g_logger->set_pattern(settings.format);
}
}   // namespace my_log