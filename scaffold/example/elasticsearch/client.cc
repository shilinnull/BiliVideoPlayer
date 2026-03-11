#include <cpr/response.h>
#include <elasticlient/client.h>
#include <iostream>
#include <thread>

const std::string index_name = "student";
const std::string type_name = "_doc";
const std::string index_id = "default_id";

void create_index(elasticlient::Client &client) {
    //1. 构造请求正文
    const std::string body = R"({
            "settings": {
                "analysis": {
                    "analyzer": {
                        "ikmax" : {
                            "type": "custom",
                            "tokenizer" : "ik_max_word"
                        }
                    }
                }
            },
            "mapping": {
                "dynamic": false,
                "properties": {
                    "name": {
                        "type": "text",
                        "boost": 3.0,
                        "analyzer": "ikmax"
                    },
                    "age": {
                        "type": "integer"
                    },
                    "phone": {
                        "type": "keyword", 
                        "boost": 1.0
                    },
                    "skills": { 
                        "type": "text"
                    },
                    "birth": {
                        "type": "date",
                        "index": false
                    }
                }
            }
        })";
    //2. 发送请求
    auto resp = client.index(index_name, type_name, index_id, body);
    //3. 获取结果判断是否创建索引成功 -- 判断响应状态码是否正常201
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "create index failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
}

void add_data(elasticlient::Client &client) {
    const std::string body = R"({
            "name": "张三",
            "age": 19,
            "phone": "13888888888",
            "skills": ["java","php","go"],
            "birth": "2007-05-21 12:35:32"
        })";
    auto resp = client.index(index_name, type_name, "2", body);
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "insert data failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
}


void mod_data(elasticlient::Client &client) {
    const std::string body = R"({
        "name": "李四",
        "age": 19,
        "phone": "13333333333",
        "skills": ["java","php","go"],
        "birth": "2007-05-21 12:35:32"
    })";
    // 使用index方式进行更新，必须是全量更新，它等价于，先删除，再新增，如果不是全量更新，则最终就只有更新字段
    // const std::string body = R"({
    //     "name": "李四"
    // })";
    auto resp = client.index(index_name, type_name, "2", body);
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "insert data failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
}

void remove_data(elasticlient::Client &client) {
    auto resp = client.remove(index_name, type_name, "2");
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "remove data failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
}
void remove_index(elasticlient::Client &client) {
    auto resp = client.performRequest(elasticlient::Client::HTTPMethod::DELETE, index_name, "");
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "remove index failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
}


void get_data(elasticlient::Client &client) {
    const std::string body = R"(
        {
            "query" :  {
                "match_all" :   {}
            }
        }
    )";
    auto resp = client.search(index_name, type_name, body);
    if (resp.status_code < 200 || resp.status_code >= 300) {
        std::cout << "remove data failed: " << resp.status_code << " "<< resp.text << std::endl;
        return;
    }
    std::cout << "resp:" << resp.text << std::endl;
}


int main()
{
    //1. 定义es服务器url
    const std::string url = "http://elastic:123456@192.168.80.128:9200/";
    //2. 实例化client对象
    elasticlient::Client  client({url});
    //3. 数据操作（创建索引，新增数据，全量查询，删除数据，删除索引）
    create_index(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));//避免还未处理完毕休眠一下
    add_data(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));//避免还未处理完毕休眠一下
    mod_data(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));//避免还未处理完毕休眠一下
    get_data(client);
    remove_data(client);
    remove_index(client);
    return 0;
}