#ifndef VIDEOPLAYER_STUDENT_H
#define VIDEOPLAYER_STUDENT_H

#include <string>
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
    size_t classes_id() const { return _classes_id; }
    void set_classes_id(size_t classes_id) { _classes_id = classes_id; }
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
#pragma db index
    size_t _classes_id;
    std::string _name;
    odb::nullable<int> _age;
    odb::nullable<double> _score;
    odb::nullable<boost::posix_time::ptime> _birthday;
};

//  select * from tbl where xxx limit n offset n
#pragma db view query("select name, classes_id, score from tbl_student")
struct NativeStudent {
    std::string name;
    size_t classes_id;
    odb::nullable<double> score;
};

#endif //VIDEOPLAYER_STUDENT_H