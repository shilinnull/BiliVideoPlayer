#ifndef VIDEOPLAYER_STUDENT_H
#define VIDEOPLAYER_STUDENT_H

#include <string>
#include <cstddef> // std::size_t
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

#pragma db object table("tbl_student")
class Student {
public:
    Student() {}
    Student(const std::string &name): _name(name) {}
    size_t sn() const { return _sn; }
    void set_sn(size_t sn) { _sn = sn; }
    const std::string &name() const { return _name; }
    void set_name(const std::string &name) { _name = name; }
    odb::nullable<int> age() const { return _age; }
    void set_age(odb::nullable<int> age) { _age = age; }
    odb::nullable<double> score() const { return _score; }
    void set_score(odb::nullable<double> score) { _score = score; }
    odb::nullable<boost::posix_time::ptime> birthday() const { return _birthday; }
    void set_birthday(const boost::posix_time::ptime &birthday) { _birthday = birthday; }
private:
    friend class odb::access;
#pragma db id auto
    size_t _sn;
    std::string _name;
    odb::nullable<int> _age;
    odb::nullable<double> _score;
    odb::nullable<boost::posix_time::ptime> _birthday;
};



#endif //VIDEOPLAYER_STUDENT_H