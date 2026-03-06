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
}
