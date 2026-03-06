
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}

#include <iostream>

const char* mavError(int err_code) {
    static char errmsg[256];
    av_strerror(err_code, errmsg, sizeof(errmsg));
    return errmsg;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./hls input_file output_file\n";
        return -1;
    }
    // 1. 打开输入文件，创建输入格式化上下文对象
    int ret = 0;
    AVFormatContext *inputContext = nullptr, *outputContext = nullptr;
    ret = avformat_open_input(&inputContext, argv[1], nullptr, nullptr);
    if (ret < 0) {
        std::cout << "打开输入文件失败：" << mavError(ret) << std::endl;
        return -1;
    }
    // 2. 通过输入格式化上下文对象解析视频文件元信息
    ret = avformat_find_stream_info(inputContext, nullptr);
    if (ret < 0) {
        std::cout << "解析媒体元信息失败：" << mavError(ret) << std::endl;
        return -1;
    }
    // 3. 申请创建输出格式化上下文对象，并且设置输出格式hls
    ret = avformat_alloc_output_context2(&outputContext, nullptr, "hls", argv[2]);
    if (ret < 0) {
        std::cout << "创建输出上下文对象失败：" << mavError(ret) << std::endl;
        return -1;
    }
    // 4. 遍历输入格式化上下文中的媒体流信息，为输出格式化上下文对象创建媒体流，并复制解码器参数
    for (int i = 0; i < inputContext->nb_streams; i++) {
        AVStream *inputStream = inputContext->streams[i];
        AVStream* outputStream = avformat_new_stream(outputContext, nullptr);
        avcodec_parameters_copy(outputStream->codecpar, inputStream->codecpar);
        outputStream->avg_frame_rate = inputStream->avg_frame_rate;
        outputStream->r_frame_rate = inputStream->r_frame_rate;
    }
    // 5. 设置HLS转码的各项细节参数：播放类型-点播vod，分片时间，路径前缀：/video/
    AVDictionary *dict = nullptr;
    av_dict_set_int(&dict, "hls_time", 5, 0);
    av_dict_set(&dict, "hls_base_url", "/video/", 0);
    av_dict_set(&dict, "hls_playlist_type", "vod", 0);
    av_dict_set(&dict, "hls_flags", "independent_segments", 0);
    // 6. 通过输出格式上下文，向输出文件写入头部信息
    ret = avformat_write_header(outputContext, &dict);
    if (ret < 0) {
        std::cout << "输出头部信息失败：" << mavError(ret) << std::endl;
        return -1;
    }
    //7. 遍历输入格式化上下文中的数据帧
    AVPacket pkt;
    while (av_read_frame(inputContext, &pkt) >= 0) {
        AVStream *inputStream = inputContext->streams[pkt.stream_index];
        AVStream *outputStream = outputContext->streams[pkt.stream_index];
        //1. 将数据包中的时间戳，从输入流的时间基转换为输出流的时间基
        if (pkt.pts == AV_NOPTS_VALUE) {
            // 若当前数据帧显示时间戳无效，则将时间戳设置为从低0s开始的时间戳
            pkt.pts = av_rescale_q(0, AV_TIME_BASE_Q, inputStream->time_base);
            pkt.dts = pkt.pts;
        }
        av_packet_rescale_ts(&pkt, outputStream->time_base, inputStream->time_base);
        //2. 将数据帧通过输出格式化上下文对象，写入输出文件中
        ret = av_interleaved_write_frame(outputContext, &pkt);
        if (ret < 0) {
            std::cout << "输出数据帧失败:" << mavError(ret) << std::endl;
            return -1;
        }
        //3. 释放数据帧
        av_packet_unref(&pkt);
    }
    //8. 向输出文件写入文件尾部信息
    ret = av_write_trailer(outputContext);
    if (ret < 0) {
        std::cout << "输出尾部信息失败:" << mavError(ret) << std::endl;
        return -1;
    }
    //9. 转码完成，释放资源：参数字典对象，输入格式化上下文对象，输出格式化上下文对象
    av_dict_free(&dict);
    avformat_free_context(outputContext);
    avformat_close_input(&inputContext);
    return 0;
}
