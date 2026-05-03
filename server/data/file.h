#pragma once

#include "data.h"

namespace vp_data {
class FileData {
public:
    using ptr = std::shared_ptr<FileData>;
    FileData(odb::transaction &mtx);
    void insert(File& file);
    void update(File& file);
    void remove(const std::string& file_id);
    File::ptr selectById(const std::string& file_id);
private:
    odb::database _db;
};
}