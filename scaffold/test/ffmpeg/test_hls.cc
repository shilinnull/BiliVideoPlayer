#include "../../source/hls.h"
#include "../../source/log.h"

void hls_test() {
    Bilihls::hls_settings settings = {
        .hls_time = 5,
        .playlist_type = "vod",
        .base_url = "/video/"
    };
    Bilihls::HLSTranscoder transcoder(settings);
    bool ret = transcoder.transcode("./111.mp4", "./dest.m3u8");
    if (ret == false) {
        ERR("转码失败!");
        abort();
    }
}
void m3u8_test() {
    Bilihls::M3U8Info info("./dest.m3u8");
    bool ret = info.parse();
    if (ret == false) {
        ERR("m3u8文件解析失败!");
        return;
    }
    auto &headers = info.headers();
    for (auto &h : headers) {
        std::cout << "[" << h << "]" << std::endl;
    }
    std::cout << "==========================\n";
    auto &pieces = info.pieces();
    for (int i = 0; i < pieces.size(); i++) {
        std::cout << "[" << pieces[i].first << "]" << std::endl;
        std::cout << "[" << pieces[i].second << "]" << std::endl;
        pieces[i].second = "/hello" + pieces[i].second;
    }
    ret = info.write();
    if (ret == false) {
        ERR("m3u8文件重写失败!");
        return;
    }
}

int main()
{
    Bililog::Bililog_init();
    hls_test();
    m3u8_test();
    return 0;
}
