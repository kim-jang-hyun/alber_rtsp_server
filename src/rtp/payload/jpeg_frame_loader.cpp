#include "rtp/payload/jpeg_frame_loader.h"

#include <iostream>
#include <boost/assert.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtp {
namespace payload {

JpegFrameLoader::JpegFrameLoader(std::string file_path)
    : m_format_context(NULL), m_codec_context(NULL)
{
    init(file_path);
}

JpegFrameLoader::~JpegFrameLoader()
{
    finish();
}

bool JpegFrameLoader::getNextFrame(unsigned char*& data, std::size_t& size)
{
    AVPacket packet;
    int read_frame_ret = av_read_frame(m_format_context, &packet);
    if (read_frame_ret == 0)
    {
        unsigned char* buf = new unsigned char[packet.size];
        std::memcpy(buf, packet.data, packet.size);

        data = buf;
        size = packet.size;
    }

    av_packet_unref(&packet);
    return read_frame_ret == 0 ? true : false;
}

void JpegFrameLoader::init(std::string file_path)
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

    AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
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

    m_file_path = file_path;
}

void JpegFrameLoader::finish()
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
