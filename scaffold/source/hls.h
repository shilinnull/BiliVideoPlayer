#ifndef VIDEOPLAYER_HLS_H
#define VIDEOPLAYER_HLS_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}

#include <iostream>
#include <string>
#include <vector>
#include <memory>

const std::string HLS_EXTM3U = "#EXTM3U";
const std::string HLS_VERSION = "#EXT-X-VERSION:";
const std::string HLS_TARGETDURATION = "#EXT-X-TARGETDURATION:";
const std::string HLS_SEQUENCE = "EXT-X-MEDIA-SEQUENCE:";
const std::string HLS_PLAYLIST_TYPE = "EXT-X-PLAYLIST-TYPE";
const std::string HLS_INDEPENDENT_SEGMENTS = "#EXT-X-INDEPENDENT-SEGMENTS";
const std::string HLS_ENDLIST = "#EXT-X-ENDLIST";
const std::string HLS_EXTINF = "#EXTINF:";

namespace Bilihls {
    class M3U8Info {
    public:
        using ptr = std::shared_ptr<M3U8Info>;
        using StrPair = std::pair<std::string, std::string>;
        M3U8Info(const std::string& filename);
        bool parse();
        bool write();
        std::vector<std::string>& headers();
        std::vector<StrPair>& pieces();

    private:
        std::string _filename;
        std::vector<std::string> _headers;
        std::vector<StrPair> _pieces;
    };

    struct hls_settings {
        size_t hls_time;
        std::string playlist_type;
        std::string base_url;
    };

    class HLSTranscoder {
    public:
        using ptr = std::shared_ptr<HLSTranscoder>;
        HLSTranscoder(const hls_settings& settings);
        bool transcode(const std::string& input, const std::string& output);
    private:
        const char* avError(int err_code);
        hls_settings _settings;
    };
}

#endif //VIDEOPLAYER_HLS_H
