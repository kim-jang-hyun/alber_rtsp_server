#ifndef __RTSP_SERVER_RTP_PAYLOAD_FRAME_LOADER_H__
#define __RTSP_SERVER_RTP_PAYLOAD_FRAME_LOADER_H__

#include <string>
#include <boost/shared_ptr.hpp>

#include "rtp/payload/payload_common.h"

struct AVFormatContext;
struct AVCodecContext;

namespace rtsp_server {
namespace rtp {
namespace payload {

class FrameLoader
{
public:
    explicit FrameLoader(std::string file_path);
    ~FrameLoader();

    bool getNextFrame(unsigned char*& data, std::size_t& size) const;
    PayloadType getPayloadType() const { return m_payload_type; }

private:
    void init(std::string file_path);
    void finish();

private:
    AVFormatContext* m_format_context;
    AVCodecContext* m_codec_context;
    int m_video_stream_index;

    PayloadType m_payload_type;
};
typedef boost::shared_ptr<FrameLoader> FrameLoaderPtr;

} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_FRAME_LOADER_H__
