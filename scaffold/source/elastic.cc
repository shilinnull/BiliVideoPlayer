#include "elastic.h"
#include "util.h"
#include "log.h"
#include <optional>

namespace Biliics {
    
    Base::Base(const std::string &key): _key(key) {}
    const std::string& Base::key() const { return _key; }
    const Json::Value Base::val() const { return _val; }
    std::string Base::to_string() const { 
        Json::Value root;
        root[_key] = _val;
        return *Biliutil::JSON::serialize(root, true);
    }
    
    Object::Object(const std::string &key): Base(key) {}
    std::shared_ptr<Object> Object::newObject(const std::string &key) {
        Object::ptr sub;
        sub = this->getElement<Object>(key);
        if (sub) {
            return sub;
        }
        sub = std::make_shared<Object>(key);
        this->addElement(sub);
        return sub;
    }
    std::shared_ptr<Array> Object::newArray(const std::string &key) {
        Array::ptr array;
        array = this->getElement<Array>(key);
        if (array) {
            return array;
        }
        array = std::make_shared<Array>(key);
        this->addElement(array);
        return array;
    }
    void Object::addElement(const Base::ptr &sub) {
        _subs[sub->key()] = sub;
    }
    const Json::Value Object::val() const { 
        Json::Value root = _val; //将当前val的数据赋值给root
        for (auto &i: _subs) {   //当前对象也可能有自己子对象，把子对象的val拿出来赋值到root
            root[i.first] = i.second->val();
        }
        return root;
    }
    std::string Object::to_string() const {
        Json::Value root = _val; //先拿本身的数据
        for (auto &i: _subs) {   //再拿自己的子对象的数据
            root[i.first] = i.second->val();
        }
        return *Biliutil::JSON::serialize(root, true);
    }


    Array::Array(const std::string &key): Base(key) {}
    std::shared_ptr<Object> Array::newObject(const std::string &key) {
        auto obj = std::make_shared<Object>(key);
        this->addElement(obj);
        return obj;
    }
    std::shared_ptr<Array> Array::newArray(const std::string &key) {
        auto arr = std::make_shared<Array>(key);
        this->addElement(arr);
        return arr;
    }
    void Array::addElement(const Base::ptr &sub) {
        _subs.push_back(sub);
    }
    const Json::Value Array::val() const {
        Json::Value root = _val;
        for (auto &i: _subs) {
            root.append(i->val());
        }
        return root;
    }
    std::string Array::to_string() const {
        Json::Value root = _val;
        for (auto &i: _subs) {
            root.append(i->val());
        }
        return *Biliutil::JSON::serialize(root, true);
    }

    
    Tokenizer::Tokenizer(const std::string &str): Object(str) {
        this->tokenizer("ik_max_word");
        this->type("custom");
    }
    void Tokenizer::tokenizer(const std::string &t) {
        this->add("tokenizer", t);
    }
    void Tokenizer::type(const std::string &type) {
        this->add("type", type);
    }

    Analyzer::Analyzer(): Object("analyzer") {}
    Tokenizer::ptr Analyzer::tokenizer(const std::string &tokenizer_name) {
        Tokenizer::ptr tokenizer;
        tokenizer = this->getElement<Tokenizer>(tokenizer_name);
        if (tokenizer) {
            return tokenizer;
        }
        tokenizer = std::make_shared<Tokenizer>(tokenizer_name);
        this->addElement(tokenizer);
        return tokenizer;
    }
    
    Analysis::Analysis(): Object("analysis") {}
    Analyzer::ptr Analysis::analyzer() {
        Analyzer::ptr analyzer;
        analyzer = this->getElement<Analyzer>("analyzer");
        if (analyzer) {
            return analyzer;
        }
        analyzer = std::make_shared<Analyzer>();
        this->addElement(analyzer);
        return analyzer;
    }

    Settings::Settings(): Object("settings") {}
    Analysis::ptr Settings::analysis() {
        Analysis::ptr analysis;
        analysis = this->getElement<Analysis>("analysis"); 
        if (analysis) {
            return analysis;
        }
        analysis = std::make_shared<Analysis>();
        this->addElement(analysis);
        return analysis;
    }

    
    Field::Field(const std::string &key): Object(key){
        this->type("text");
    }
    void Field::type(const std::string &type) {
        this->add("type", type);
    }
    void Field::boost(double boost) {
        this->add("boost", boost);
    }
    void Field::index(bool flag) {
        this->add("index", flag);
    }
    void Field::analyzer(const std::string &analizer_name) {
        this->add("analyzer", analizer_name);
    }

    Properties::Properties(): Object("properties") {}
    Field::ptr Properties::field(const std::string &field_name) {
        Field::ptr field;
        field = this->getElement<Field>(field_name);
        if (field) {
            return field;
        }
        field = std::make_shared<Field>(field_name);
        this->addElement(field);
        return field;
    }

    Mappings::Mappings(): Object("mapping") {
        this->dynamic(false);
    }
    void Mappings::dynamic(bool flag) {
        this->add("dynamic", flag);
    }
    Properties::ptr Mappings::properties() {
        Properties::ptr properties;
        properties = this->getElement<Properties>("properties");
        if (properties) {
            return properties;
        }
        properties = std::make_shared<Properties>();
        this->addElement(properties);
        return properties;
    }

    Term::Term(const std::string &field): Object("term"), _field(field) {}
    Terms::Terms(const std::string &field): Object("terms"), _field(field) {}
    Match::Match(const std::string &field): Object("match"), _field(field) {}
    MultiMatch::MultiMatch():Object("multi_match") {}
    void MultiMatch::appendField(const std::string &field) { this->append("fields", field); }
    Range::Range(const std::string &field): Object("range") {
        _sub = std::make_shared<Object>(field);
        this->addElement(_sub);
    }

    
    QObject::QObject(const std::string &key): Object(key) {}
    Term::ptr QObject::term(const std::string &field) {
        Term::ptr term;
        term = this->getElement<Term>("term");
        if (term) {
            return term;
        }
        term = std::make_shared<Term>(field);
        this->addElement(term);
        return term;
    }
    Terms::ptr QObject::terms(const std::string &field) {
        Terms::ptr terms;
        terms = this->getElement<Terms>("terms");
        if (terms) {
            return terms;
        }
        terms = std::make_shared<Terms>(field);
        this->addElement(terms);
        return terms;
    }
    Match::ptr QObject::match(const std::string &field) {
        Match::ptr match;
        match = this->getElement<Match>("match");
        if (match) {
            return match;
        }
        match = std::make_shared<Match>(field);
        this->addElement(match);
        return match;
    }
    MultiMatch::ptr QObject::multi_match() {
        MultiMatch::ptr multi_match;
        multi_match = this->getElement<MultiMatch>("multi_match");
        if (multi_match) {
            return multi_match;
        }
        multi_match = std::make_shared<MultiMatch>();
        this->addElement(multi_match);
        return multi_match;
    }
    Range::ptr QObject::range(const std::string &field) {
        Range::ptr range;
        range = this->getElement<Range>("range");
        if (range) {
            return range;
        }
        range = std::make_shared<Range>(field);
        this->addElement(range);
        return range;
    }

    
    QArray::QArray(const std::string &key): Array(key) {}
    Term::ptr QArray::term(const std::string &field) {
        auto tmp = std::make_shared<Term>(field);
        auto obj = this->newObject("");
        obj->addElement(tmp);
        return tmp;
    }
    Terms::ptr QArray::terms(const std::string &field) {
        auto tmp = std::make_shared<Terms>(field);
        auto obj = this->newObject("");
        obj->addElement(tmp);
        return tmp;
    }
    Match::ptr QArray::match(const std::string &field) {
        auto tmp = std::make_shared<Match>(field);
        auto obj = this->newObject("");
        obj->addElement(tmp);
        return tmp;
    }
    MultiMatch::ptr QArray::multi_match() {
        auto tmp = std::make_shared<MultiMatch>();
        auto obj = this->newObject("");
        obj->addElement(tmp);
        return tmp;
    }
    Range::ptr QArray::range(const std::string &field) {
        auto tmp = std::make_shared<Range>(field);
        auto obj = this->newObject("");
        obj->addElement(tmp);
        return tmp;
    }

    Must::Must(): QArray("must") {}
    Should::Should(): QArray("should") {}
    MustNot::MustNot(): QArray("must_not") {}
    
    QBool::QBool(): QObject("bool") {}
    void QBool::minimum_should_match(size_t count) {
        this->add("minimum_should_match", count);
    }
    Must::ptr QBool::must() {
        Must::ptr tmp;
        tmp = this->getElement<Must>("must");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<Must>();
        this->addElement(tmp);
        return tmp;
    }
    Should::ptr QBool::should() {
        Should::ptr tmp;
        tmp = this->getElement<Should>("should");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<Should>();
        this->addElement(tmp);
        return tmp;
    }
    MustNot::ptr QBool::must_not()  {
        MustNot::ptr tmp;
        tmp = this->getElement<MustNot>("must_not");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<MustNot>();
        this->addElement(tmp);
        return tmp;
    }


    Query::Query(): QObject("query"){}
    void Query::match_all() {
        this->add("match_all", Json::Value(Json::ValueType::objectValue));
    }
    QBool::ptr Query::qbool()  {
        QBool::ptr tmp;
        tmp = this->getElement<QBool>("bool");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<QBool>();
        this->addElement(tmp);
        return tmp;
    }
    Must::ptr Query::must() {
        return this->qbool()->must();
    }
    Should::ptr Query::should() {
        return this->qbool()->should();
    }
    MustNot::ptr Query::must_not() {
        return this->qbool()->must_not();
    }


    Request::Request(const std::string &index, const std::string &type, const std::string &op, const std::string &id)
        : _index(index), _type(type), _op(op), _id(id) {}
    void Request::set_index(const std::string &index) { _index = index; }
    void Request::set_type(const std::string &type) { _type = type; }
    void Request::set_op(const std::string &op) { _op = op; }
    void Request::set_id(const std::string &id) { _id = id; }
    const std::string& Request::index() const { return _index; }
    const std::string& Request::type() const { return _type; }
    const std::string& Request::op() const { return _op; }
    const std::string& Request::id() const { return _id; }

    Indexer::Indexer(const std::string &index): Object(""), Request(index, "_doc", "index", index) {}
    Settings::ptr Indexer::settings() {
        Settings::ptr settings;
        settings = this->getElement<Settings>("settings");
        if (settings) {
            return settings;
        }
        settings = std::make_shared<Settings>();
        this->addElement(settings);
        return settings;
    }
    Tokenizer::ptr Indexer::tokenizer(const std::string &tokenizer_name) {
        return this->settings()->analysis()->analyzer()->tokenizer(tokenizer_name);
    }
    Mappings::ptr Indexer::mappings() {
        Mappings::ptr mappings;
        mappings = this->getElement<Mappings>("mapping");
        if (mappings) {
            return mappings;
        }
        mappings = std::make_shared<Mappings>();
        this->addElement(mappings);
        return mappings;
    }
    Field::ptr Indexer::field(const std::string &field_name) {
        return this->mappings()->properties()->field(field_name);
    }

    Inserter::Inserter(const std::string &index, const std::string &id): Object(""), Request(index, "_doc", "_insert", id) {}
    Updater::Updater(const std::string &index, const std::string &id): Object(""), Request(index, "_doc", "_update", id) {}
    Object::ptr Updater::doc() {
        Object::ptr doc;
        doc = this->getElement<Object>("doc");
        if (doc) {
            return doc;
        }
        doc = std::make_shared<Object>("doc");
        this->addElement(doc);
        return doc;
    }
    Deleter::Deleter(const std::string &index, const std::string &id): Object(""), Request(index, "_doc", "_delete", id) {}
    Searcher::Searcher(const std::string &index): Object(""), Request(index, "_doc", "_search", "") {}
    Query::ptr Searcher::query() {
        Query::ptr query;
        query = this->getElement<Query>("query");
        if (query) {
            return query;
        }
        query = std::make_shared<Query>();
        this->addElement(query);
        return query;
    }
    void Searcher::size(size_t count) {
        this->add("size", count);
    }
    void Searcher::from(size_t offset) {
        this->add("from", offset);
    }



    ESClient::ESClient(const std::vector<std::string> &hosts)
        : _client(std::make_shared<elasticlient::Client>(hosts)) {}
    bool ESClient::create(const Indexer &idx) {
        //1. 获取关键信息
        std::string index_name = idx.index();
        std::string index_type = idx.type();
        std::string index_id = idx.id();
        std::string index_body = idx.to_string();
        //2. 发起请求
        auto resp = _client->index(index_name, index_type, index_id, index_body);
        //3. 收到响应，判断是否成功
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("创建索引失败: {}/{}/{}/{} - {}", index_name, index_type, index_id, index_body, resp.text);
            return false;
        }
        return true;
    }
    bool ESClient::insert(const Inserter &ins) {
        //1. 获取关键信息
        std::string index_name = ins.index();
        std::string index_type = ins.type();
        std::string doc_id = ins.id();
        std::string index_body = ins.to_string();
        auto resp = _client->index(index_name, index_type, doc_id, index_body);
        //3. 收到响应，判断是否成功
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("新增数据失败: {}/{}/{}/{} - {}", index_name, index_type, doc_id, index_body, resp.text);
            return false;
        }
        return true;
    }
    bool ESClient::update(const Updater &upd) {
        //1. 获取关键信息
        std::string index_name = upd.index();
        std::string index_type = upd.type();
        std::string doc_id = upd.id();
        std::string index_body = upd.to_string();
        std::string url = index_name + "/_update/"  + doc_id;
        auto resp = _client->performRequest(elasticlient::Client::HTTPMethod::POST, url, index_body);
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("更新数据失败: {}/{}/{} - {}", index_name, url, index_body, resp.text);
            return false;
        }
        return true;
    }
    bool ESClient::remove(const Deleter &del) {
        std::string index_name = del.index();
        std::string index_type = del.type();
        std::string doc_id = del.id();
        auto resp = _client->remove(index_name, index_type, doc_id);
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("删除数据失败: {}/{}/{} - {}", index_name, index_type, doc_id, resp.text);
            return false;
        }
        return true;
    }   
    bool ESClient::remove(const std::string &index_name) {
        auto resp = _client->performRequest(elasticlient::Client::HTTPMethod::DELETE, index_name, "");
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("删除索引失败: {} - {}", index_name, resp.text);
            return false;
        }
        return true;
    }
    std::optional<Json::Value> ESClient::search(const Searcher &sea) {
        //1. 获取关键信息
        std::string index_name = sea.index();
        std::string index_type = sea.type();
        std::string index_body = sea.to_string();
        auto resp = _client->search(index_name, index_type, index_body);
        if (resp.status_code < 200 || resp.status_code >= 300) {
            ERR("搜索数据失败: {}/{}/{} - {}", index_name, index_type, index_body, resp.text);
            return std::optional<Json::Value>();
        }
        auto json_resp = Biliutil::JSON::unserialize(resp.text);
        if (!json_resp) {
            ERR("搜索数据,对响应进行反序列化失败: {}/{}/{} - {}", index_name, index_type, index_body, resp.text);
            return std::optional<Json::Value>();
        }
        if ((*json_resp).isNull() || (*json_resp)["hits"].isNull() || (*json_resp)["hits"]["hits"].isNull()) {
            ERR("搜索数据,响应格式错误: {}/{}/{} - {}", index_name, index_type, index_body, resp.text);
            return std::optional<Json::Value>();
        }
        Json::Value result;
        int sz = (*json_resp)["hits"]["hits"].size();
        for (int i = 0; i < sz; ++i) {
            result.append((*json_resp)["hits"]["hits"][i]["_source"]);
        }
        return result;
    }

}