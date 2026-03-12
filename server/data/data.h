#ifndef BILIVIDEOPLAYER_DATA_H
#define BILIVIDEOPLAYER_DATA_H

#include <odb/nullable.hxx>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <optional>

namespace vp_data {

#pragma db object table("tbl_session_meta")
class Session {
public:
    using ptr = std::shared_ptr<Session>;
    Session();
    Session(const std::string &session_id);
    Session(const std::string &session_id, const std::string &user_id);

    size_t id() const;
    const std::string &session_id() const;
    odb::nullable<std::string> user_id() const;

    void set_id(size_t id);
    void set_session_id(const std::string &session_id);
    void set_user_id(const std::string &user_id);
private:
    friend class odb::access;
#pragma db id auto
    size_t _id;
#pragma db unique type("VARCHAR(64)")
    std::string _session_id;
#pragma db index type("VARCHAR(64)")
    odb::nullable<std::string> _user_id;
};
#pragma db view object(Session) \
        query((?))

struct SessionPtr {
    using ptr = std::shared_ptr<Session>;
    std::shared_ptr<Session> session;
};

#pragma db object table("tbl_file_meta")
class File {
public:
    using ptr = std::shared_ptr<File>;
    File();
    File(const std::string &file_id,
        const std::string &uploader_uid,
        const std::string &path,
        size_t size,
        const std::string &mime);
    size_t id() const;
    const std::string &file_id() const;
    const std::string &uploader_uid() const;
    const std::string &path() const;
    size_t size() const;
    const std::string &mime() const;

    void set_id(size_t id);
    void set_file_id(const std::string &file_id);
    void set_uploader_uid(const std::string &uploader_uid);
    void set_path(const std::string &path);
    void set_size(size_t size);
    void set_mime(const std::string &mime);
private:
    friend class odb::access;
#pragma db id auto
    size_t _id;
#pragma db unique type("VARCHAR(64)")
    std::string _file_id;               // 文件id
#pragma db index type("VARCHAR(64)")
    std::string _uploader_uid;          // 上传用户id
    std::string _path;                  // 存储路径
    size_t _size;                       // 文件大小
#pragma db type("VARCHAR(32)")
    std::string _mime;                  // 文件mime
};



}


#endif //BILIVIDEOPLAYER_DATA_H