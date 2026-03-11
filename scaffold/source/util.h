#pragma once
#include <jsoncpp/json/json.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <optional>
#include "log.h"

namespace Biliutil {
    class JSON {
    public:
        static std::optional<std::string> serialize(const Json::Value& val, bool styled = false);
        static std::optional<Json::Value> unserialize(const std::string& input);
    };

    class FUTIL {
    public:
        static bool read(const std::string& filename, std::string& body);
        static bool write(const std::string& filename, const std::string& body);
    };

    class STR {
    public:
        static size_t split(const std::string& src, const std::string& sep, std::vector<std::string>& dst);
    };

    const size_t UUID_SIZE = 16;
    enum UuidType {
        MIX = 0,
        CHAR,
        DIGIT
    };
    class Random {
    public:
        //16个字符长度的字母+数字随机字符串
        static std::string code(size_t len = UUID_SIZE, UuidType utype = UuidType::MIX);
        //生成一个指定区间的随机数
        static size_t number(size_t min, size_t max);
    };
}
