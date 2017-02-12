#ifndef __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H264_H__
#define __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H264_H__

#include "rtsp/message/sdp/sdp.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace sdp {

class SdpH264 : public Sdp
{
public:
    SdpH264();
    virtual ~SdpH264();

private:
    //derived from Sdp - begin
    virtual void generateImpl();
    //derived from Sdp - end

private:
    struct
    {
        std::string payload_type;
        std::string encoding_name;
        std::string clock_rate;
    } m_media_attribute_rtpmap;

    struct
    {
        std::string format;
        std::string packetization_mode;
    } m_media_attribute_fmtp;
};

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H264_H__
