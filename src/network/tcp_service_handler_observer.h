#ifndef __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_OBSERVER_H__
#define __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_OBSERVER_H__

#include <cstddef>
#include <string>

namespace rtsp_server {
namespace network {

class TcpServiceHandlerObserver
{
public:
    virtual void onRead(unsigned char* data, std::size_t size) = 0;
    virtual void onWrite(unsigned char* data, std::size_t size) = 0;
    virtual void onClose() = 0;
};

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_OBSERVER_H__
