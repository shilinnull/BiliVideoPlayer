#include <string>
#include <memory> // std::auto_ptr
#include <cstdlib> // std::exit
#include <iostream>

#incldue <odb/database.hxx>
#include <odb/mysql/database.hxx>

#include "person.hxx"
#include "person-odb.hxx"

int main()
{
    std::unique_ptr<odb::core::database> db(new odb::mysql::database("root", "shilin", "mytest", "127.0.0.1", 0, 0, "utf8"));
    if (!db)
    {
        std::cerr << "Cannot open database" << std::endl;
        return -1;
    }
    return 0;
}


