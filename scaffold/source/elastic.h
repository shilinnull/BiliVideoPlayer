#pragma once
#include "util.h"
#include <cpr/response.h>
#include <elasticlient/client.h>

namespace Biliics {
    class Base {
        public:
            using ptr = std::shared_ptr<Base>;
            Base(const std::string &key);
            //向val对象中添加数据：基础类型数据，数组类型数据
            template<typename T>
            void add(const std::string& key, const T &val) {
                _val[key] = val;
            }
            template<typename T>
            void append(const std::string& key, const T &val) {
                _val[key].append(val);
            }
            const std::string& key() const;
            virtual const Json::Value val() const;
            virtual std::string to_string() const;
        protected:
            std::string _key;
            Json::Value _val;
    };
    class Array;
    class Object: public Base {
        public:
            using ptr = std::shared_ptr<Object>;
            Object(const std::string &key);
            template<typename R>
            std::shared_ptr<R> getElement(const std::string &key) {
                auto it = _subs.find(key);
                if (it == _subs.end()) {
                    return nullptr;
                }
                return std::dynamic_pointer_cast<R>(it->second);
            }
            std::shared_ptr<Object> newObject(const std::string &key);
            std::shared_ptr<Array> newArray(const std::string &key);
            void addElement(const Base::ptr &sub);
            virtual const Json::Value val() const;
            virtual std::string to_string() const;
        private:
            std::unordered_map<std::string, Base::ptr> _subs;
            //Json::Value _val;
    };
    
    class Array: public Base {
        public:
            using ptr = std::shared_ptr<Array>;
            Array(const std::string &key);
            std::shared_ptr<Object> newObject(const std::string &key);
            std::shared_ptr<Array> newArray(const std::string &key);
            void addElement(const Base::ptr &sub);
            virtual const Json::Value val() const;
            virtual std::string to_string() const;
        private:
            std::vector<Base::ptr> _subs;
    };

    class Tokenizer : public Object{
        public:
            using ptr = std::shared_ptr<Tokenizer>;
            Tokenizer(const std::string &str);
            void tokenizer(const std::string &t);
            void type(const std::string &type);
    };
    class Analyzer : public Object {
        public:
            using ptr = std::shared_ptr<Analyzer>;
            Analyzer();
            Tokenizer::ptr tokenizer(const std::string &tokenizer_name);
    };
    class Analysis : public Object {
        public:
            using ptr = std::shared_ptr<Analysis>;
            Analysis();
            Analyzer::ptr analyzer();
    };

    class Settings : public Object {
        public:
            using ptr = std::shared_ptr<Settings>;
            Settings();
            Analysis::ptr analysis();
    };

    class Field : public Object {
        public:
            using ptr = std::shared_ptr<Field>;
            Field(const std::string &key);
            void type(const std::string &type);
            void boost(double boost);
            void index(bool flag);
            void analyzer(const std::string &analizer_name);
    };

    class Properties : public Object {
        public:
            using ptr = std::shared_ptr<Properties>;
            Properties();
            Field::ptr field(const std::string &field_name);
    };

    class Mappings : public Object {
        public:
            using ptr = std::shared_ptr<Mappings>;
            Mappings();
            Properties::ptr properties();
            void dynamic(bool flag);
    };


    class Term : public Object {
        public:
            using ptr = std::shared_ptr<Term>;
            Term(const std::string &field);
            template<typename T>
            void setValue(const T &value) {
                this->add(_field, value);
            }
        private:
            std::string _field;
    };
    class Terms : public Object {
        public:
            using ptr = std::shared_ptr<Terms>;
            Terms(const std::string &field);
            template<typename T>
            void setValue(const T &value) {
                this->append(_field, value);
            }
        private:
            std::string _field;
    };
    class Match : public Object {
        public:
            using ptr = std::shared_ptr<Match>;
            Match(const std::string &field);
            template<typename T>
            void setValue(const T &value) {
                this->add(_field, value);
            }
        private:
            std::string _field;
    };
    class MultiMatch : public Object {
        public:
            using ptr = std::shared_ptr<MultiMatch>;
            MultiMatch();
            void appendField(const std::string &field);
            template<typename T>
            void setQuery(const T &query) {
                this->add("query", query);
            }
    };
    class Range : public Object {
        public:
            using ptr = std::shared_ptr<Range>;
            Range(const std::string &field);
            template<typename T>
            void setRange(const T &gt, const T& lt) {
                _sub->add("gt", gt);
                _sub->add("lt", lt);
            }
        private:
            Object::ptr _sub;
    };

    class QObject : public Object {
        public:
            using ptr = std::shared_ptr<QObject>;
            QObject(const std::string &key);
            Term::ptr term(const std::string &field); //创建一个term子对象
            Terms::ptr terms(const std::string &field); //创建一个terms子对象
            Match::ptr match(const std::string &field); //创建一个match子对象
            MultiMatch::ptr multi_match(); //创建一个multi_match子对象
            Range::ptr range(const std::string &field); //创建一个range子对象
    };

    class QArray : public Array {
        public:
            using ptr = std::shared_ptr<QArray>;
            QArray(const std::string &key);
            Term::ptr term(const std::string &field);
            Terms::ptr terms(const std::string &field);
            Match::ptr match(const std::string &field);
            MultiMatch::ptr multi_match(); 
            Range::ptr range(const std::string &field);
    };

    class Must : public QArray {
        public:
            using ptr = std::shared_ptr<Must>;
            Must();
    };
    class Should : public QArray {
        public:
            using ptr = std::shared_ptr<Should>;
            Should();
    };
    class MustNot : public QArray {
        public:
            using ptr = std::shared_ptr<MustNot>;
            MustNot();
    };

    class QBool : public QObject {
        public:
            using ptr = std::shared_ptr<QBool>;
            QBool();
            Must::ptr must();
            Should::ptr should();
            MustNot::ptr must_not();
            void minimum_should_match(size_t count);
    };

    class Query : public QObject {
        public:
            using ptr = std::shared_ptr<Query>;
            Query();
            void match_all();
            QBool::ptr qbool();
            Must::ptr must();
            Should::ptr should();
            MustNot::ptr must_not();
    };

    class Request {
        public:
            Request(const std::string &index, const std::string &type, const std::string &op, const std::string &id);
            void set_index(const std::string &index);
            void set_type(const std::string &type);
            void set_op(const std::string &op);
            void set_id(const std::string &id);
            const std::string& index() const;
            const std::string& type() const;
            const std::string& op() const;
            const std::string& id() const;
        protected:
            std::string _index;
            std::string _type;
            std::string _op;
            std::string _id;
    };

    class Indexer : public Object, public Request {
        public:
            using ptr = std::shared_ptr<Indexer>;
            Indexer(const std::string &index);
            Settings::ptr settings();
            Tokenizer::ptr tokenizer(const std::string &tokenizer_name);
            Mappings::ptr mappings();
            Field::ptr field(const std::string &field_name);
    };

    class Inserter : public Object, public Request {
        public:
            using ptr = std::shared_ptr<Inserter>;
            Inserter(const std::string &index, const std::string &id);
    };

    class Updater : public Object, public Request {
        public:
            using ptr = std::shared_ptr<Updater>;
            Updater(const std::string &index, const std::string &id);
            Object::ptr doc();
            template<typename T>
            void add(const std::string &key, const T &val) {
                this->doc()->add(key, val);
            }
            template<typename T>
            void append(const std::string &key, const T &val) {
                this->doc()->append(key, val);
            }
    };
    class Deleter : public Object, public Request {
        public:
            using ptr = std::shared_ptr<Deleter>;
            Deleter(const std::string &index, const std::string &id);
    };
    class Searcher : public Object, public Request  {
        public:
            Searcher(const std::string &index);
            Query::ptr query();
            void size(size_t count);
            void from(size_t offset);
    };

    class BaseClient {
        public:
            using ptr = std::shared_ptr<BaseClient>;
            BaseClient() = default;
            virtual ~BaseClient() = default;
            virtual bool create(const Indexer &idx) = 0; //创建索引
            virtual bool insert(const Inserter &ins) = 0; //插入数据
            virtual bool update(const Updater &upd) = 0; //更新数据
            virtual bool remove(const Deleter &del) = 0; //删除数据
            virtual bool remove(const std::string &index) = 0; //删除索引
            virtual std::optional<Json::Value> search(const Searcher &sea) = 0; //查询数据
    };

    class ESClient : public BaseClient {
        public:
            using ptr = std::shared_ptr<ESClient>;
            ESClient(const std::vector<std::string> &hosts);
            bool create(const Indexer &idx) override;
            bool insert(const Inserter &ins) override;
            bool update(const Updater &upd) override;
            bool remove(const Deleter &del) override;
            bool remove(const std::string &index) override;
            std::optional<Json::Value> search(const Searcher &sea) override;
        private:
            std::shared_ptr<elasticlient::Client> _client;
    };
}