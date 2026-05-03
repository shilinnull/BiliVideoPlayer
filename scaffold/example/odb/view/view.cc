#include <odb/database.hxx>
#include <odb/mysql/transaction.hxx>
#include <odb/mysql/database.hxx>

#include "student.h"
#include "student-odb.hxx"

using namespace std;

const std::string HOST = "192.168.80.128";
const std::string USER = "root";
const std::string PASS = "123456";
const std::string DBNAME = "mytest";
const unsigned int PORT = 3306;
const std::string CSET = "utf8";

void get_classes_students(std::unique_ptr<odb::mysql::database> &handler, int class_id) {
    try {
        odb::transaction tx(handler->begin());
        auto& db = tx.database();
        typedef odb::query<ClassesStudent> Query;
        typedef odb::result<ClassesStudent> Result;

        Result res(db.query<ClassesStudent>(Query::Classes::name == "一年级一班"));
        for (auto it = res.begin(); it != res.end(); ++it) {
            if (it->classes) {
                std::cout << "班级:" << it->classes->name() << "\t";
            }
            if (it->student) {
                std::cout << "姓名:" << it->student->name() << std::endl;
            }
        }
        tx.commit();
    }catch (odb::exception &e) {
        std::cerr << "odb error: " << e.what() << std::endl;
    }
}

void get_detail_student(std::unique_ptr<odb::mysql::database> &handler, int sn) {
    try {
        odb::transaction tx(handler->begin());
        auto &db = tx.database();
        typedef odb::query<DetailStudent> Query;
        typedef odb::result<DetailStudent> Result;

        Result res(db.query<DetailStudent>(Query::Student::name == "赵六"));
        for (auto it = res.begin(); it != res.end(); ++it) {
            if (it->classes) {
                std::cout << "班级:" << it->classes->name() << "\t";
            }
            if (it->student) {
                std::cout << "姓名:" << it->student->name() << std::endl;
            }
        }
        tx.commit();
    }catch (odb::exception &e) {
        std::cout << "odb error: " << e.what() << std::endl;
    }
}

int main() {
    // 构造连接池
    std::unique_ptr<odb::mysql::connection_factory> pool(new odb::mysql::connection_pool_factory(5));
    // 构造数据库操作句柄
    auto handler = std::make_unique<odb::mysql::database>(
        USER, PASS, DBNAME, HOST, PORT, nullptr, CSET, 0, std::move(pool));
    get_classes_students(handler, 1);
    get_detail_student(handler, 2);
    return 0;
}
