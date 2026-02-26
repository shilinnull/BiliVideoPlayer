#include <iostream>
#include "person.pb.h"
#include <google/protobuf/util/json_util.h>

std::string serialize_test() {
    person::Student stu;
    stu.set_sn(1001);
    stu.set_name("zhangsan");
    stu.set_sex(person::SexType::male);
    stu.add_score(90);
    auto score = stu.mutable_score();
    score->Add(80);
    auto other = stu.mutable_other();
    (*other)[0] = "abc";
    other->insert({1, "def"});

    auto str = stu.SerializeAsString();
    std::cout << str << std::endl;
    return str;
}

void unserialize_test(const std::string& str) {
    person::Student stu;
    bool ret = stu.ParseFromString(str);
    if(ret == false) {
        std::cout << "unserialize failed" << std::endl;
        return ;
    }
    std::cout << stu.sn() << std::endl;
    std::cout << stu.name() << std::endl;
    std::cout << stu.sex() << std::endl;
    for(auto score : stu.score()) {
        std::cout << score << std::endl;
    }
    for(auto it = stu.other().begin(); it != stu.other().end(); it++) {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

void json2pb_test() {
    std::string json = R"({"sn": 10001, "name": "zhangsan", "sex": 2, "score": [99, 88, 77], "other": {"1": "abc", "2": "def"}})";
    person::Student stu;
    auto ret = google::protobuf::util::JsonStringToMessage(json, &stu);
    // if(ret != google::protobuf::util::OkStatus()) {
    if(ret.ok() == false) {
        std::cout << "json2pb failed" << std::endl;
        return ;
    }
    std::cout << stu.sn() << std::endl;
    std::cout << stu.name() << std::endl;
    std::cout << stu.sex() << std::endl;
    for(auto score : stu.score()) {
        std::cout << score << std::endl;
    }
    for(auto it = stu.other().begin(); it != stu.other().end(); it++) {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

void pb2json_test() {
    person::Student stu;
    stu.set_sn(10001);
    stu.set_name("zhangsan");
    stu.set_sex(person::SexType::male);
    stu.add_score(99);
    stu.add_score(88);
    stu.add_score(77);
    auto other = stu.mutable_other();
    other->insert({1, "def"});
    other->insert({2, "abc"});

    std::string json_str;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.add_whitespace = true;
    auto ret = google::protobuf::util::MessageToJsonString(stu, &json_str, options);
    if(ret.ok() == false) {
        std::cout << "pb2json failed" << std::endl;
        return ;
    }
    std::cout << json_str << std::endl;
}

int main() {
    auto str = serialize_test();
    unserialize_test(str);
    std::cout << "==========================" << std::endl;
    json2pb_test();
    std::cout << "==========================" << std::endl;
    pb2json_test();

    return 0;
}