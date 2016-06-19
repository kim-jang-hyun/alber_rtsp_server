#ifndef __RTSP_SERVER_NETWORK_CONNECTOR_H__
#define __RTSP_SERVER_NETWORK_CONNECTOR_H__

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "tcp_service_handler.h"

namespace rtsp_server {
namespace network {

namespace asio = boost::asio;

class Connector : private boost::noncopyable
{
public:
    explicit Connector(asio::io_service& io_service);

    void connect(const std::string& ip, unsigned short port);

private:
    void handleConnect(const boost::system::error_code& e);

private:
    asio::io_service& m_io_service;
    TcpServiceHandlerPtr m_service_handler;
};

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_CONNECTOR_H__
