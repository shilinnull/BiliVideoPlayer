#include "../../source/odb.h"
#include <iostream>

#include "student.h"
#include "student-odb.hxx"

int main()
{
    // 构造服务器配置
    Biliodb::mysql_settings settings = {
        .host = "192.168.80.128",
        .passwd = "123456",
        .db = "mytest",
        .connection_pool_size = 5
    };
    // 创建操作句柄
    auto handler = Biliodb::DBFactory::mysql(settings);
    // 数据操作
    {
        try {
            odb::transaction tx(handler->begin());
            auto &db = tx.database();
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
            std::cout << "odb error: " << e.what() << std::endl;
        }
    }
    return 0;
}