#include "file.h"

namespace vp_data {
    FileData::FileData(odb::transaction& mtx): _db(mtx.database()){}
    void FileData::insert(File& file) { _db.presist(file); }
    void FileData::update(File& file) { _db.update(file); }
    void FileData::remove(const std::string& file_id) {
        typedef odb::query<File> Query;
        _db.erase_query<File>(Query::file_id == file_id);
    }
    File::ptr FileData::selectById(const std::string& file_id) {
        typedef odb::query<File> Query;
        File::ptr res(_db.query_one<File>(Query::file_id == file_id));
        return res;
    }
}
