#include "../../source/elastic.h"
#include "../../source/log.h"

const std::string index_name = "student";
const std::string url = "http://elastic:123456@192.168.80.128:9200/";

void create(const Biliics::ESClient::ptr &client) {
    Biliics::Indexer indexer(index_name);
    indexer.tokenizer("ikmax");
    auto name = indexer.field("name");
    name->analyzer("ikmax");
    name->boost(2.0);
    indexer.field("phone")->type("keyword");
    indexer.field("age")->type("integer");
    indexer.field("skills")->type("text");
    indexer.field("birth")->type("date");
    indexer.field("birth")->index("false");
    auto res = client->create(indexer);
    if (res == false) {
        return ;
    }
}
// 2. 新增数据
void insert(const Biliics::ESClient::ptr &client) {
    Biliics::Inserter inserter(index_name, "1");
    inserter.add("name", "张三");
    inserter.add("phone", "15566667777");
    inserter.add("age", 18);
    inserter.add("birth", "2018-08-19 22:22:33");
    inserter.append("skills", "C++");
    inserter.append("skills", "Java");
    inserter.append("skills", "Python");
    auto res = client->insert(inserter);
    if (res == false) {
        return ;
    }
}
// 3. 更新数据
void update1(const Biliics::ESClient::ptr &client) {
    Biliics::Updater updater(index_name, "1");
    updater.add("name", "李四");
    updater.add("phone", "15566668888");
    updater.add("age", 20);
    updater.append("skills", "Go");
    updater.append("skills", "Rust");
    auto res = client->update(updater);
    if (res == false) {
        return ;
    }
}
void update2(const Biliics::ESClient::ptr &client) {
    Biliics::Inserter inserter(index_name, "1");
    inserter.add("name", "王五");
    inserter.add("phone", "15566669999");
    inserter.add("age", 18);
    inserter.add("birth", "2018-08-19 22:22:33");
    inserter.append("skills", "C++");
    inserter.append("skills", "Java");
    inserter.append("skills", "Python");
    auto res = client->insert(inserter);
    if (res == false) {
        return ;
    }
}
// 4. 查询数据
void select1(const Biliics::ESClient::ptr &client) {
    Biliics::Searcher searcher(index_name);
    auto query = searcher.query();
    query->match("name")->setValue("李四");
    searcher.from(0);
    searcher.size(1);
    auto res = client->search(searcher);
    if (!res) {
        return ;
    }
    std::cout << *Biliutil::JSON::serialize(*res) << std::endl;
}
void select2(const Biliics::ESClient::ptr &client) {
    Biliics::Searcher searcher(index_name);
    auto query = searcher.query();
    // auto must = query->must();
    // must->match("name")->setValue("zhangsan");

    auto should = query->should();
    should->terms("phone")->setValue("15566669999");
    should->terms("phone")->setValue("15533333333");

    // auto must_not = query->must_not();
    // must_not->match("name")->setValue("wangwu");

    auto res = client->search(searcher);
    if (!res) {
        return ;
    }
    std::cout << *Biliutil::JSON::serialize(*res) << std::endl;
}
// 5. 删除数据
void remove1(const Biliics::ESClient::ptr &client) {
    Biliics::Deleter deleter(index_name, "1");
    auto res = client->remove(deleter);
    if (res == false) {
        return ;
    }
}
// 6. 删除索引 
void remove2(const Biliics::ESClient::ptr &client) {
    auto res = client->remove(index_name);
    if (res == false) {
        return ;
    }
}


int main()
{
    Bililog::Bililog_init();
    std::vector<std::string> urls = {url};
    auto client = std::make_shared<Biliics::ESClient>(urls);
    create(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    insert(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    update1(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    select1(client);
    update2(client);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    select2(client);
    remove1(client);
    remove2(client);
    return 0;
}