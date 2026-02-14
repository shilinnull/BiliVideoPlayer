#include "../../source/util.h"

void serialize_test() {
    Json::Value val;
    val["name"] = "zhangsan";
    val["age"] = 18;
    val["score"].append(77.5);
    val["score"].append(88.5);
    val["score"].append(99.5);
    auto ret = biliutil::JSON::serialize(val);
    if (!ret) {
        return;
    }
    std::cout << *ret << std::endl;
}
void unserialize_test() {
    std::string str = R"({"age":18,"name":"zhangsan","score":[77.5,88.5,99.5]})";
    auto ret = biliutil::JSON::unserialize(str);
    if (!ret) {
        return;
    }
    std::cout << (*ret)["name"].asString() << std::endl;
    std::cout << (*ret)["age"].asInt() << std::endl;
    if (!(*ret)["score"].isNull() && (*ret)["score"].isArray()) {
        int sz = (*ret)["score"].size();
        for (int i = 0; i < sz; ++i) {
            std::cout << (*ret)["score"][i].asDouble() << std::endl;
        }
    }
}

int main()
{
    bililog::bililog_init();
    serialize_test();
    unserialize_test();
    return 0;
}