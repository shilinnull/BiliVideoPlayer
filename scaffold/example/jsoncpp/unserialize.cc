#include <iostream>
#include <jsoncpp/json/json.h>

using namespace std;

int main(int argc, char* argv[])
{
    //组织一个json格式的字符串
    string str = R"(
        {
            "address":{
                "city":"Anytown",
                "state":"CA",
                "street":"123 Main St"
            },
            "age":30,
            "is_student":false,
            "name":"John Doe",
            "skills":["JavaScript","Python","C++"]
        }
    )";
    //实例化中间转出对象
    Json::Value value;
    //实例化工厂对象
    Json::CharReaderBuilder build;
    //通过工厂对象创建反序列化对象
    unique_ptr<Json::CharReader> pcr(build.newCharReader());
    string err;
    //反序列化
    bool ret = pcr->parse(&str[0], &str[str.size()], &value, &err);
    if (ret == false) {
        cout << "Error: " << err << endl;
        return -1;
    }
    //打印反序列化后得到的各个数据，看看是否正确
    if (!value["name"].isNull()) 
        cout << "name: " << value["name"].asString() << endl;
    if (!value["age"].isNull()) 
        cout << "age: " << value["age"].asInt() << endl;
    if (!value["is_student"].isNull()) 
        cout << "is_student: " << value["is_student"].asBool() << endl;
    if (!value["weight"].isNull()) 
        cout << "weight: " << value["weight"].asFloat() << endl;
    if (!value["skills"].isNull() && value["skills"].isArray()){
        int sz = value["skills"].size();
        for (int i = 0; i < sz; i++) {
            cout << "skills: " << value["skills"][i].asString() << endl;
        }
    }
    if (!value["address"].isNull() && value["address"].isObject()){
        cout << "address.street: ";
        cout << value["address"]["street"].asString() << endl;
        cout << "address.city: ";
        cout << value["address"]["city"].asString() << endl;
        cout << "address.state: ";
        cout << value["address"]["state"].asString() << endl;
    }
    return 0;
}