#ifndef __RTSP_SERVER_RTP_PAYLOAD_PAYLOAD_COMMON_H__
#define __RTSP_SERVER_RTP_PAYLOAD_PAYLOAD_COMMON_H__

namespace rtsp_server {
namespace rtp {
namespace payload {

enum PayloadType
{
    H264_PAYLOAD_TYPE,
    MJPEG_PAYLOAD_TYPE,
    UNKNOWN_PAYLOAD_TYPE,
};

const unsigned short DEFAULT_FRAME_RATE = 30;
const std::size_t FRAGMENTAION_SIZE = 1440;

} // namespace payload
} // namespace rtp
} // namespace rtsp_server


#endif // __RTSP_SERVER_RTP_PAYLOAD_PAYLOAD_COMMON_H__
