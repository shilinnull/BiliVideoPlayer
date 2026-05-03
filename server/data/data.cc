#include "data.h"

namespace vp_data {
    /*------------------------Session start------------------------*/
    Session::Session() {}
    Session::Session(const std::string& session_id) :
        _session_id(session_id){}
    Session::Session(const std::string& session_id, const std::string& user_id) :
        _session_id(session_id), _user_id(user_id){}
    size_t Session::id() const { return _id; }
    const std::string& Session::session_id() const { return _session_id; }
    odb::nullable<std::string> Session::user_id() const { return _user_id; }
    void Session::set_id(size_t id) { _id = id; }
    void Session::set_session_id(const std::string& session_id) { _session_id = session_id; }
    void Session::set_user_id(const std::string& user_id) { _user_id = user_id; }
    /*------------------------Session end------------------------*/

    /*------------------------File start------------------------*/
    File::File() {}
    File::File(const std::string& file_id,
        const std::string& uploader_uid,
        const std::string& path,
        size_t size,
        const std::string& mime):
    _file_id(file_id), _uploader_uid(uploader_uid),
    _path(path), _size(size), _mime(mime)
    {}
    size_t File::id() const { return _id; }
    const std::string& File::file_id() const { return _file_id; }
    const std::string& File::uploader_uid() const { return _uploader_uid; }
    const std::string& File::path() const { return _path; }
    size_t File::size() const { return _size; }
    const std::string& File::mime() const { return _mime; }
    void File::set_id(size_t id) { _id = id; }
    void File::set_file_id(const std::string& file_id) { _file_id = file_id; }
    void File::set_uploader_uid(const std::string& uploader_uid) { _uploader_uid = uploader_uid; }
    void File::set_path(const std::string& path) { _path = path; }
    void File::set_size(size_t size) { _size = size; }
    void File::set_mime(const std::string& mime) { _mime = mime; }
    /*------------------------File end------------------------*/


}
