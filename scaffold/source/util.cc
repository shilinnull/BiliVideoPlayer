#include "util.h"
#include "log.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <atomic>

namespace biliutil {

std::optional<std::string> JSON::serialize(const Json::Value& val, bool styled) {
    Json::StreamWriterBuilder builder;
    if (!styled) {
        builder["indentation"] = "";
    }
    std::unique_ptr<Json::StreamWriter> swp(builder.newStreamWriter());
    std::stringstream ss;
    int ret = swp->write(val, &ss);
    if (ret != 0) {
        ERR("序列化失败！");
        return std::optional<std::string>();
    }
    return ss.str();
}
std::optional<Json::Value> JSON::unserialize(const std::string& input) {
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value val;
    std::string errs;
    bool ret = reader->parse(input.c_str(), input.c_str() + input.size(), &val, &errs);
    if (ret == false) {
        ERR("{} 反序列化失败: {}", input, errs);
        return std::optional<Json::Value>();
    }
    return val;
}

}