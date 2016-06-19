#ifndef __RTSP_SERVER_RTSP_SESSION_OBSERVER_H__
#define __RTSP_SERVER_RTSP_SESSION_OBSERVER_H__

#include "rtsp/message/request/message.h"

namespace rtsp_server {
namespace rtsp {

class SessionObserver
{
public:
    virtual void onReceive(message::MessageInterfacePtr msg) = 0;
};

} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_SESSION_OBSERVER_H__
