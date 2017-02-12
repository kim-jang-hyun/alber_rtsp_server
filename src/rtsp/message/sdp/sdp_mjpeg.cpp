#include "rtsp/message/sdp/sdp_mjpeg.h"

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

SdpMjpeg::SdpMjpeg()
{

}

SdpMjpeg::~SdpMjpeg()
{

}

void SdpMjpeg::generateImpl()
{
    m_media_description.media = "video";
    m_media_description.port = "0";
    m_media_description.proto = "RTP/AVP";
    m_media_description.fmt = "26";

    m_sdp_stream << "m=" << m_media_description.media << " "
                           << m_media_description.port << " "
                           << m_media_description.proto << " "
                           << m_media_description.fmt << "\r\n";
}

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
