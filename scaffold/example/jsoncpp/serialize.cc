#include <iostream>
#include <jsoncpp/json/json.h>

int main(int argc, char* argv[])
{
    //组织json对象数据
    Json::Value item;
    item["street"] = "123 Main St";
    item["city"] = "Anytown";
    item["state"] = "CA";

    Json::Value value;
    value["name"] = "John Doe";
    value["age"] = 30;
    value["is_student"] = false;
    value["skills"].append("JavaScript");
    value["skills"].append("Python");
    value["skills"].append("C++");
    value["address"] = item;

    //实例化工厂对象
    Json::StreamWriterBuilder swb;
    //是否启用个性化设置：默认会添加一些空白字符让序列化出来的内容阅读性更好
    // swb.settings_["commentStyle"] = "None";
    // swb.settings_["indentation"] = "";
    //通过工厂对象创建StreamWriter对象
    std::unique_ptr<Json::StreamWriter> psw(swb.newStreamWriter());
    std::stringstream ss;
    try{
        //序列化，并获取序列化后的内容进行输出
        psw->write(value, &ss);
        std::cout << ss.str() << std::endl;
    }catch(Json::Exception &e) {
        std::cout << "Error:" << e.what() <<  std::endl;
    }
    return 0;
}