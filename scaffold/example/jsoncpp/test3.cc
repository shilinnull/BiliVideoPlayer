#include <jsoncpp/json/json.h>
#include <iostream>
int main()
{
    std::string str = R"({"age":18,"name":"zhangsan","score":[77.5,88.5,99.5]})";
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value val;
    std::string errs;
    bool ret = reader->parse(str.c_str(), str.c_str() + str.size(), &val, &errs);
    if (ret == false) {
        std::cout << "parse json failed, errmsg: " << errs << std::endl;
        return -1;
    }
    std::cout << val["name"].asString() << std::endl;
    std::cout << val["age"].asInt() << std::endl;
    if (!val["score"].isNull() && val["score"].isArray()) {
        int sz = val["score"].size();
        for (int i = 0; i < sz; ++i) {
            std::cout << val["score"][i].asDouble() << std::endl;
        }
    }
    return 0;
}