#include "hls.h"
#include "util.h"

namespace Bilihls {
    M3U8Info::M3U8Info(const std::string& filename) :_filename(filename) {}

    bool M3U8Info::parse() {
        std::string body;
        bool ret = Biliutil::FUTIL::read(_filename, body);
        if (ret == false) {
            ERR("M3U8文件读取数据失败: {}", _filename);
            return false;
        }
        std::vector<std::string> str_list;
        int count = Biliutil::STR::split(body, "\n", str_list);
        for (int i = 0; i < count; i++) {
            if (str_list[i].find(HLS_ENDLIST) != std::string::npos) { break; }
            if (str_list[i].find(HLS_EXTINF) != std::string::npos) {
                _pieces.push_back({str_list[i], str_list[i + 1]});
                i++;
                continue;
            }
            _headers.push_back(str_list[i]);
        }
        return true;
    }

    bool M3U8Info::write() {
        std::stringstream ss;
        for (auto& h : _headers) { ss << h << "\n"; }
        for (auto& p : _pieces) { ss << p.first << "\n" << p.second << "\n"; }
        ss << HLS_ENDLIST;
        bool ret = Biliutil::FUTIL::write(_filename, ss.str());
        if (ret == false) {
            ERR("M3U8文件写入数据失败: {}", _filename);
            return false;
        }
        return true;
    }

    std::vector<std::string>& M3U8Info::headers() { return _headers; }
    std::vector<M3U8Info::StrPair>& M3U8Info::pieces() { return _pieces; }

    HLSTranscoder::HLSTranscoder(const hls_settings& settings) : _settings(settings) {}

    bool HLSTranscoder::transcode(const std::string& input, const std::string& output) {
        int ret;
        //1. 打开输入文件，创建输入格式化上下文对象
        AVFormatContext *inputContext = nullptr, *outputContext = nullptr;
        ret = avformat_open_input(&inputContext, input.c_str(), nullptr, nullptr);
        if (ret < 0) {
            std::cout << "打开输入文件失败:" << avError(ret) << std::endl;
            return false;
        }
        //2. 通过输入格式化上下文对象解析视频文件元信息
        ret = avformat_find_stream_info(inputContext, nullptr);
        if (ret < 0) {
            std::cout << "解析媒体元信息失败:" << avError(ret) << std::endl;
            avformat_close_input(&inputContext);
            return false;
        }
        //3. 申请创建输出格式化上下文对象，并且设定输出格式 hls
        ret = avformat_alloc_output_context2(&outputContext, nullptr, "hls", output.c_str());
        if (ret < 0) {
            std::cout << "创建输出上下文对象失败:" << avError(ret) << std::endl;
            avformat_close_input(&inputContext);
            return false;
        }
        //4. 遍历输入格式化上下文中的媒体流信息，为输出格式化上下文对象创建媒体流，并复制编解码器参数
        for (int i = 0; i < inputContext->nb_streams; i++) {
            AVStream* inputStream = inputContext->streams[i];
            AVStream* outputStream = avformat_new_stream(outputContext, nullptr);
            avcodec_parameters_copy(outputStream->codecpar, inputStream->codecpar);
            outputStream->avg_frame_rate = inputStream->avg_frame_rate;
            outputStream->r_frame_rate = inputStream->r_frame_rate;
        }
        //5. 设置HLS转码的各项细节参数：播放类型-点播vod，分片时间， 路径前缀
        AVDictionary* dict = nullptr;
        av_dict_set_int(&dict, "hls_time", _settings.hls_time, 0);
        av_dict_set(&dict, "hls_base_url", _settings.base_url.c_str(), 0);
        av_dict_set(&dict, "hls_playlist_type", _settings.playlist_type.c_str(), 0);
        av_dict_set(&dict, "hls_flags", "independent_segments", 0);
        //6. 通过输出格式化上下文，向输出文件写入头部信息
        ret = avformat_write_header(outputContext, &dict);
        if (ret < 0) {
            std::cout << "输出头部信息失败:" << avError(ret) << std::endl;
            av_dict_free(&dict);
            avformat_free_context(outputContext);
            avformat_close_input(&inputContext);
            return false;
        }
        //7. 遍历输入格式化上下文中的数据帧，
        AVPacket pkt;
        while (av_read_frame(inputContext, &pkt) >= 0) {
            AVStream* inputStream = inputContext->streams[pkt.stream_index];
            AVStream* outputStream = outputContext->streams[pkt.stream_index];
            //1. 将数据包中的时间戳，从输入流的时间基转换为输出流的时间基
            if (pkt.pts == AV_NOPTS_VALUE) {
                // 若当前数据帧显示时间戳无效，则将时间戳设置为从低0s开始的时间戳
                pkt.pts = av_rescale_q(0, AV_TIME_BASE_Q, inputStream->time_base);
                pkt.dts = pkt.pts;
            }
            //应该是输入数据帧时间基转换到输出数据帧时间基
            //av_packet_rescale_ts(数据帧，输入时间基， 输出时间基)；
            av_packet_rescale_ts(&pkt, inputStream->time_base, outputStream->time_base);
            //2. 将数据帧通过输出格式化上下文对象，写入输出文件中
            ret = av_interleaved_write_frame(outputContext, &pkt);
            if (ret < 0) {
                std::cout << "输出数据帧失败:" << avError(ret) << std::endl;
                av_dict_free(&dict);
                avformat_free_context(outputContext);
                avformat_close_input(&inputContext);
                return false;
            }
            //3. 释放数据帧
            av_packet_unref(&pkt);
        }
        //8. 向输出文件写入文件尾部信息
        ret = av_write_trailer(outputContext);
        if (ret < 0) {
            std::cout << "输出尾部信息失败:" << avError(ret) << std::endl;
            av_dict_free(&dict);
            avformat_free_context(outputContext);
            avformat_close_input(&inputContext);
            return false;
        }
        //9. 转码完成，释放资源：参数字典对象，输入格式化上下文对象，输出格式化上下文对象
        av_dict_free(&dict);
        avformat_free_context(outputContext);
        avformat_close_input(&inputContext);
        return true;
    }

    const char* HLSTranscoder::avError(int err_code) {
        static char errmsg[256];
        av_strerror(err_code, errmsg, 255);
        return errmsg;
    }
}
