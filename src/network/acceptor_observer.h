#ifndef __RTSP_SERVER_NETWORK_ACCEPTOR_OBSERVER_H__
#define __RTSP_SERVER_NETWORK_ACCEPTOR_OBSERVER_H__

#include "network/tcp_service_handler.h"

namespace rtsp_server {
namespace network {

class AcceptorObserver
{
public:
    virtual void onAccept(TcpServiceHandlerPtr service_handler) = 0;
};

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_ACCEPTOR_OBSERVER_H__
