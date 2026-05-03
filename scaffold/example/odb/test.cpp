#include <string>
#include <memory> // std::auto_ptr
#include <cstdlib> // std::exit
#include <iostream>

#include <odb/mysql/database.hxx>

#include "person.hxx"
#include "person-odb.hxx"

using namespace std;

int main()
{
    std::unique_ptr<odb::core::database> db(new odb::mysql::database("root", "123456", "test", "192.168.80.128", 0, 0, "utf8"));
    if (!db) {
        std::cerr << "Cannot open database" << std::endl;
        return -1;
    }
    ptime p = boost::posix_time::second_clock::local_time();
    Person zhang("小张", 18, p);
    Person wang("小王", 20, p);

    typedef odb::query<Person> query;
    typedef odb::result<Person> result;
    {
        odb::core::transaction t(db->begin());
        size_t zid = db->persist(zhang);
        size_t wid = db->persist(wang);
        t.commit();
    }
    {
        ptime p = boost::posix_time::time_from_string("2000-01-01 00:00:00");
        ptime e = boost::posix_time::time_from_string("2028-01-01 00:00:00");
        odb::core::transaction t(db->begin());
        result r(db->query<Person>(query::update < e && query::update > p));
        for (result::iterator it = r.begin(); it != r.end(); ++it) {
            cout << "姓名：" << it->name() << endl;
            cout << it->age() << it->update() << endl;
        }
        t.commit();
    }

    return 0;
}


