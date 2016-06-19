#ifndef __RTSP_SERVER_RTP_PAYLOAD_JPEG_FRAME_LOADER_H__
#define __RTSP_SERVER_RTP_PAYLOAD_JPEG_FRAME_LOADER_H__

#include <string>

// ffmpeg specific - begin
#pragma warning (disable : 4244)
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};
// ffmpeg specific - end

namespace rtsp_server {
namespace rtp {
namespace payload {

class JpegFrameLoader
{
public:
    explicit JpegFrameLoader(std::string file_path);
    ~JpegFrameLoader();

    bool getNextFrame(unsigned char*& data, std::size_t& size);

private:
    void init(std::string file_path);
    void finish();

private:
    AVFormatContext* m_format_context;
    AVCodecContext* m_codec_context;

    std::string m_file_path;
};

} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_SOURCE_HANDLER_H__
