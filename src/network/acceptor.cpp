#include "acceptor.h"

#include <iostream>
#include <boost/assert.hpp>
#include <boost/bind.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace network {

Acceptor::Acceptor(IoService& io_service, unsigned short port,
                   AcceptorObserver* observer)
 :  m_io_service(io_service.getIoService()), m_acceptor(m_io_service),
    m_acceptor_observer(observer)
{
    BOOST_ASSERT(m_acceptor_observer != NULL);

    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);

    {
        m_acceptor.open(endpoint.protocol());
        m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    }

    m_acceptor.bind(endpoint);
    m_acceptor.listen();
}

Acceptor::~Acceptor()
{

}

void Acceptor::accept()
{
    TcpServiceHandlerPtr service_handler;
    service_handler.reset(new TcpServiceHandler(m_io_service));

    m_acceptor.async_accept(service_handler->getSocket(),
                            boost::bind(&Acceptor::handleAccept, this,
                                        asio::placeholders::error,
                                        service_handler));
}

void Acceptor::handleAccept(const boost::system::error_code& e,
                            TcpServiceHandlerPtr service_handler)
{
    if (! e)
    {
        std::string remote_ip = service_handler->getRemoteIp();
        unsigned short remote_port = service_handler->getRemotePort();

        // log
        {
            std::cout << "accpet socket : " << "ip(" << remote_ip << "), "
                      << "port(" << remote_port <<")" << std::endl;
        }

        m_acceptor_observer->onAccept(service_handler);

        service_handler->start();
    }
}

} // namespace network
} // namespace rtsp_server
