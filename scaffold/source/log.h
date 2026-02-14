#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace bililog {
struct log_settings {
    // 是否启用异步日志
    bool async = false;
    // 日志输出等级: 1-debug;2-info;3-warn;4-error; 6-off
    int level = 1;
    // 日志输出格式 [%H:%M:%S][%-7l]: %v
    std::string format = "[%H:%M:%S][%-7l]: %v";
    // 日志输出目标 stdout
    std::string path = "stdout";
};
// 声明全局日志器
extern std::shared_ptr<spdlog::logger> g_logger;
// 声明全局日志器初始化接口
extern void bililog_init(const log_settings &settings = log_settings());
// 封装日志输出宏
#define FMT_PREFIX std::string("[{}:{}]: ")
#define DBG(fmt, ...) \
    bililog::g_logger->debug(FMT_PREFIX + fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INF(fmt, ...) \
    bililog::g_logger->info(FMT_PREFIX + fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WRN(fmt, ...) \
    bililog::g_logger->warn(FMT_PREFIX + fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERR(fmt, ...) \
    bililog::g_logger->error(FMT_PREFIX + fmt, __FILE__, __LINE__, ##__VA_ARGS__)
}   // namespace bililog
