#ifndef BILIVIDEOPLAYER_PERSON_H
#define BILIVIDEOPLAYER_PERSON_H

#include <string>
#include <cstddef>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/core.hxx>

using ptime = boost::posix_time::ptime;

#pragma db object
class Person {
public:
    Person(std::string &name, unsigned long age, const ptime &update):
        _name(name), _age(age), _update(update) {}

    void age(int val) {}
    int age() const { return _age; }
    void name(std::string &name) { _name = name; }
    std::string name() const { return _name; }
    void update(const ptime &update) { _update = update; }
    std::string update() const { return boost::posix_time::to_simple_string(_update); }

private:
    friend class odb::access;
    Person() {}
    #pragma db id auto
    unsigned long id;
    unsigned long _age;
    std::string _name;
    #pragma db type("TIMESTAMP") not_null
    ptime _update;
};



#endif //BILIVIDEOPLAYER_PERSON_H