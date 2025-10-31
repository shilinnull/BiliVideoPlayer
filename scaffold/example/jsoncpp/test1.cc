#include <jsoncpp/json/json.h>
#include <iostream>

int main()
{
    Json::Value val;
    val["name"] = "zhangsan";
    val["age"] = 18;
    val["score"].append(77.5);
    val["score"].append(88.5);
    val["score"].append(99.5);
    std::string str = val.toStyledString();
    std::cout << str << std::endl;
    return 0;
}