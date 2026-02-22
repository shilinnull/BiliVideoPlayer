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

#pragma db object table("tbl_classes")
class Classes {
public:
    Classes(){}
    Classes(const std::string& name) :_name(name) {}
    size_t id() const { return _id; }
    void set_id(size_t id) { _id = id; }
    std::string name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }
private:
    friend class odb::access;
#pragma db id auto
    size_t _id;
#pragma db unique type("VARCHAR(32)")
    std::string _name;
};

// 过滤条件： Query::Classes::name == "一年级一班"
#pragma db view object(Classes) \
    object(Student : Student::_classes_id == Classes::_id)\
    query((?))
struct ClassesStudent {
    std::shared_ptr<Student> student;
    std::shared_ptr<Classes> classes;
};

// 过滤条件： Query::Student::name == "zhangsan"
#pragma db view object(Student) \
    object(Classes : Student::_classes_id == Classes::_id)\
    query((?))
struct DetailStudent {
    std::shared_ptr<Student> student;
    std::shared_ptr<Classes> classes;
};

#endif //VIDEOPLAYER_STUDENT_H