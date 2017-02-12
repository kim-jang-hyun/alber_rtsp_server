#include "rtp/payload/frame_loader.h"

// ffmpeg specific - begin
#pragma warning (disable : 4244)
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};
// ffmpeg specific - end

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

const int INVALID_VIDEO_STREAM_INDEX = -1;

}; // namespace

namespace rtsp_server {
namespace rtp {
namespace payload {

FrameLoader::FrameLoader(std::string file_path)
    : m_format_context(NULL), m_codec_context(NULL),
      m_payload_type(UNKNOWN_PAYLOAD_TYPE),
      m_video_stream_index(INVALID_VIDEO_STREAM_INDEX)
{
    init(file_path);
}

FrameLoader::~FrameLoader()
{
    finish();
}

bool FrameLoader::getNextFrame(unsigned char*& data, std::size_t& size) const
{
    bool get_frame_success = false;

    AVPacket packet;
    int read_frame_ret = av_read_frame(m_format_context, &packet);
    if (read_frame_ret == 0)
    {
        if (packet.stream_index == m_video_stream_index)
        {
            unsigned char* buf = new unsigned char[packet.size];
            std::memcpy(buf, packet.data, packet.size);

            data = buf;
            size = packet.size;

            get_frame_success = true;
        }
    }

    av_packet_unref(&packet);

    return get_frame_success;
}

void FrameLoader::init(std::string file_path)
{
    avcodec_register_all();
    av_register_all();

    AVFormatContext* format_context = NULL;
    int format_ret = avformat_open_input(&format_context,
                                         file_path.c_str(), NULL, NULL);
    if (format_ret != 0)
    {
        return;
    }

    int find_stream_info_ret = avformat_find_stream_info(format_context, NULL);
    if (find_stream_info_ret < 0)
    {
        return;
    }

    int video_stream_index = INVALID_VIDEO_STREAM_INDEX;
    for (unsigned int i = 0; i < format_context->nb_streams; ++i)
    {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == INVALID_VIDEO_STREAM_INDEX)
    {
        return;
    }

    AVCodec* codec = avcodec_find_decoder(
        format_context->streams[video_stream_index]->codecpar->codec_id);
    if (! codec)
    {
        return;
    }

    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    if (! codec_context)
    {
        return;
    }
    int codec_ret = avcodec_open2(codec_context, codec, NULL);
    if (codec_ret != 0)
    {
        return;
    }

    m_format_context = format_context;
    m_codec_context = codec_context;
    m_video_stream_index = video_stream_index;

    switch (codec->id)
    {
    case AV_CODEC_ID_MJPEG: m_payload_type = MJPEG_PAYLOAD_TYPE;    break;
    case AV_CODEC_ID_H264:  m_payload_type = H264_PAYLOAD_TYPE;     break;
    default:                m_payload_type = UNKNOWN_PAYLOAD_TYPE;  break;
    }
}

void FrameLoader::finish()
{
    if (m_codec_context)
    {
        avcodec_close(m_codec_context);
        avcodec_free_context(&m_codec_context);
    }
    if (m_format_context)
    {
        avformat_close_input(&m_format_context);
    }
}

} // namespace payload
} // namespace rtp
} // namespace rtsp_server
