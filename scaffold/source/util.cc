#include "util.h"
#include "log.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <atomic>

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

    std::string Random::code(size_t len, UuidType utype) {
        static const char* digit_arr = "1234567890";
        static const char* char_arr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static const char* mix_arr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
        static std::atomic<unsigned char> number_base(0);
        static const size_t MIN_LEN = 6;
        // 生成随机字符串：不断产生随机数(0~数组长度), 从指定的字符串中提取出字符，组合成一个随机字符串
        // 1. 根据utype,决定从哪个字符串中提取字符
        std::string array;
        if (utype == UuidType::DIGIT) { array = digit_arr; }
        else if (utype == UuidType::CHAR){ array = char_arr; }
        else { array = mix_arr; }

        std::stringstream result;
        // 2. 循环len，获取随机数字，从字符串中提取字符 -- 提取出len-4个字符
        //  1. 生成一个机器随机数，作为伪随机数种子
        std::random_device rd;
        //  2. 根据种子，构造伪随机数引擎
        std::mt19937 generator(rd());
        for (int i = 0; i < len; i++) {
            //  2. 生成伪随机数，并对字符串长度进行取模
            int idx = generator() % array.size();
            result << array[idx];
        }
        // 要产生的字符串长度甚至不足要求的最小长度，则直接返回随机字符串，不再做编号处理
        if (len <= MIN_LEN) {
            return result.str();
        }
        // 3. 设置一个4个字符的编号字符串进行连接
        int num = number_base.fetch_add(1);
        result << std::setw(4) << std::setfill('0') << num;
        // 4. 最终组合成为一个随机字符串，进行返回即可
        return result.str();
    }
    size_t Random::number(size_t min, size_t max) {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }
}
