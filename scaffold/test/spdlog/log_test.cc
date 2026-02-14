#include <bite_scaffold/log.h>
#include <gflags/gflags.h>

//1. 通过gflags定义要捕获的参数
DEFINE_bool(log_async, true, "是否启用异步日志");
DEFINE_int32(log_level, 1, "日志输出等级: 1-debug;2-info;3-warn;4-error;6-off");
DEFINE_string(log_format, "[%H:%M:%S][%-7l]%v", "日志输出格式");
DEFINE_string(log_path, "stdout", "日志输出路径");

int main(int argc, char *argv[])
{
    //2. 解析命令行参数
    google::ParseCommandLineFlags(&argc, &argv, true);
    //3. 初始化日志器参数配置结构
    bitelog::log_settings settings = {
        .async = FLAGS_log_async,
        .level = FLAGS_log_level,
        .format = FLAGS_log_format,
        .path = FLAGS_log_path
    };
    //4. 初始化日志器
    bitelog::bitelog_init(settings);
    //5. 输出日志
    DBG("{}今年{}岁", "小明", 18);
    INF("{}今年{}岁", "小红", 19);
    WRN("{}今年{}岁", "小刚", 20);
    ERR("{}今年{}岁", "小李", 21);
    DBG("HELLO");
    return 0;
}