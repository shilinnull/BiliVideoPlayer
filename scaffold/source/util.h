/*
    util工具封装
        1. json序列化和反序列化
*/

#pragma once
#include <jsoncpp/json/json.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <optional>
#include "log.h"

namespace biliutil {
class JSON {  
    public:
        static std::optional<std::string> serialize(const Json::Value& val, bool styled = false);
        static std::optional<Json::Value> unserialize(const std::string& input);
};

}