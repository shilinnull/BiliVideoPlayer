#include "../../source/log.h"
#include "../../source/util.h"
#include "../../source/fdfs.h" //一定要最后被包含，内部有些操作与其他C++头文件中高级特性冲突了

std::string file_test() {
    //基于文件的上传下载测试
    // 1. 将文件上传到fastdfs服务器
    auto id = Bilifdfs::FDFSClient::upload_from_file("./test.cc");
    if (!id) {
        ERR("上传文件失败!");
        abort();
    }
    // 2. 从fastdfs服务器下载到文件
    bool ret = Bilifdfs::FDFSClient::download_to_file(*id, "./test.cc.download1");
    if (!ret) {
        ERR("下载文件失败!");
        abort();
    }
    // 3. 通过MD5sum命令比较文件是否一致 -- 由我们外部执行命令完成
    DBG("上传文件成功! file_id: {}", *id);
    return *id;
}
std::string buff_test() {
    //基于内存的上传下载测试
    // 1. 从文件中读取数据到内存
    std::string body;
    Biliutil::FUTIL::read("./test.cc", body);
    // 2. 从内存上传到fastdfs服务器
    auto id = Bilifdfs::FDFSClient::upload_from_buff(body);
    if (!id) {
        ERR("上传文件失败!");
        abort();
    }
    // 3. 从fastdfs服务器下载到内存
    std::string buff;
    bool ret = Bilifdfs::FDFSClient::download_to_buff(*id, buff);
    // 4. 将内存中的数据写入到文件
    Biliutil::FUTIL::write("./test.cc.download2", buff);
    // 5. 通过MD5sum命令比较文件是否一致 -- 由我们外部执行命令完成
    DBG("上传文件成功! file_id: {}", *id);
    return *id;
}
void remove_test(std::vector<std::string> &arr) {
    //删除测试
    for (auto &id : arr) {
        bool ret = Bilifdfs::FDFSClient::remove(id);
        if (!ret) {
            ERR("删除文件失败!");
            abort();
        }
        DBG("删除文件成功! file_id: {}", id); 
    }
}

int main()
{
    Bililog::Bililog_init();
    Bilifdfs::fdfs_settings settings = {
        .trackers = {"192.168.80.128:22122"}
    };
    Bilifdfs::FDFSClient::init(settings);
    std::mutex mtx;
    auto id1 = file_test();
    auto id2 = buff_test();
    std::vector<std::string> arr = {id1, id2};
    remove_test(arr);
    Bilifdfs::FDFSClient::destroy();
    return 0;
}
