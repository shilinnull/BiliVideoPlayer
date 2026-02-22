#ifndef VIDEOPLAYER_ODB_H
#define VIDEOPLAYER_ODB_H

#include <odb/database.hxx>
#include <odb/mysql/transaction.hxx>
#include <odb/mysql/database.hxx>

namespace Biliodb {
    struct mysql_settings {
        std::string host;
        std::string user = "root";
        std::string passwd;
        std::string db;
        std::string cset = "utf8";
        unsigned int port = 3306;
        unsigned int connection_pool_size = 3;
    };

    class DBFactory {
    public:
        static std::shared_ptr<odb::database> mysql(const mysql_settings &settings);
    };
}


#endif //VIDEOPLAYER_ODB_H