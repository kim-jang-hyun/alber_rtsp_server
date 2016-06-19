#include "connector.h"

#include <boost/bind.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace network {

Connector::Connector(asio::io_service& io_service) : m_io_service(io_service)
{

}

void Connector::connect(const std::string& ip, unsigned short port)
{
    using boost::asio::ip::tcp;
    using boost::asio::ip::address;

    std::string temp_ip = ip;
    port = port;

//     m_service_handler.reset(new TcpServiceHandler(m_io_service));
//     tcp::socket& socket = m_service_handler->getSocket();
// 
//     tcp::endpoint endpoint(address::from_string(ip), port);
//     socket.async_connect(endpoint, boost::bind(&Connector::handleConnect,
//                                                 this,
//                                                 asio::placeholders::error));
}

void Connector::handleConnect(const boost::system::error_code& e)
{
    if (! e)
    {
        m_service_handler->readAsync();
    }
}

} // namespace network
} // namespace rtsp_server
