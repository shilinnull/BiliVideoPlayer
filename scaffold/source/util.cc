#include "util.h"
#include "log.h"

namespace Biliutil {
    std::optional<std::string> JSON::serialize(const Json::Value& val, bool styled) {
        Json::StreamWriterBuilder builder;
        if (!styled) { builder["indentation"] = ""; }
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

    bool FUTIL::read(const std::string& filename, std::string& body) {
        std::ifstream ifs;
        ifs.open(filename, std::ios::in | std::ios::binary);
        if (ifs.is_open() == false) {
            ERR("打开文件失败: {}", filename);
            return false;
        }
        ifs.seekg(0, std::ios::end);
        size_t flen = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        body.resize(flen);
        ifs.read(&body[0], flen);
        if (ifs.good() == false) {
            ERR("读取文件数据失败: {}", filename);
            ifs.close();
            return false;
        }
        ifs.close();
        return true;
    }

    bool FUTIL::write(const std::string& filename, const std::string& body) {
        std::ofstream ofs;
        ofs.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (ofs.is_open() == false) {
            ERR("打开文件失败: {}", filename);
            return false;
        }
        ofs.write(body.c_str(), body.size());
        if (ofs.good() == false) {
            ERR("写入文件数据失败: {}", filename);
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }

    size_t STR::split(const std::string& src, const std::string& sep, std::vector<std::string>& dst) {
        size_t pos, idx = 0;
        while (1) {
            pos = src.find(sep, idx);
            if (pos == std::string::npos) { break; }
            // 跳过空白的
            if (pos == idx) {
                idx = pos + sep.size();
                continue;
            }
            dst.push_back(src.substr(idx, pos - idx));
            idx = pos + sep.size();
        }
        if (idx < src.size()) { dst.push_back(src.substr(idx)); }
        return dst.size();
    }
}
