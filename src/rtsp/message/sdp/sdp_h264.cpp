#include "rtsp/message/sdp/sdp_h264.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace sdp {

SdpH264::SdpH264()
{

}

SdpH264::~SdpH264()
{

}

void SdpH264::generateImpl()
{
    {
        m_media_description.media = "video";
        m_media_description.port = "0";
        m_media_description.proto = "RTP/AVP";
        m_media_description.fmt = "96";

        m_sdp_stream << "m=" << m_media_description.media << " "
                             << m_media_description.port << " "
                             << m_media_description.proto << " "
                             << m_media_description.fmt << "\r\n";
    }
    {
        m_media_attribute_rtpmap.payload_type = "96";
        m_media_attribute_rtpmap.encoding_name = "H264";
        m_media_attribute_rtpmap.clock_rate = "90000";

        m_sdp_stream << "a=rtpmap:"
                     << m_media_attribute_rtpmap.payload_type << " "
                     << m_media_attribute_rtpmap.encoding_name << "/"
                     << m_media_attribute_rtpmap.clock_rate << "\r\n";
    }
    {
        m_media_attribute_fmtp.format = "96";
        m_media_attribute_fmtp.packetization_mode = "packetization-mode=1";

        m_sdp_stream << "a=fmtp:"
                     << m_media_attribute_fmtp.format << " "
                     << m_media_attribute_fmtp.packetization_mode << "\r\n";
    }
}

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
