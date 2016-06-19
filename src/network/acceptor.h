#ifndef __RTSP_SERVER_NETWORK_ACCEPTOR_H__
#define __RTSP_SERVER_NETWORK_ACCEPTOR_H__

#include <boost/noncopyable.hpp>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "network/acceptor_observer.h"
#include "network/io_service.h"
#include "network/tcp_service_handler.h"

namespace rtsp_server {
namespace network {

namespace asio = boost::asio;

class Acceptor : private boost::noncopyable
{
public:
    Acceptor(IoService& io_service, unsigned short port,
             AcceptorObserver* observer);
    virtual ~Acceptor();

    void accept();

private:
    void handleAccept(const boost::system::error_code& e,
                      TcpServiceHandlerPtr service_handler);

private:
    asio::io_service& m_io_service;
    asio::ip::tcp::acceptor m_acceptor;
    AcceptorObserver* m_acceptor_observer;
};
typedef boost::shared_ptr<Acceptor> AcceptorPtr;

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_ACCEPTOR_H__
