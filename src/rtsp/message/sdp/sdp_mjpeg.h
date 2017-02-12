#ifndef __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_MJPEG_H__
#define __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_MJPEG_H__

#include "rtsp/message/sdp/sdp.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace sdp {

class SdpMjpeg : public Sdp
{
public:
    SdpMjpeg();
    virtual ~SdpMjpeg();

private:
    //derived from Sdp - begin
    virtual void generateImpl();
    //derived from Sdp - end
};

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_MJPEG_H__
