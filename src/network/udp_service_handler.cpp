#include "udp_service_handler.h"

#include <iostream>
#include <boost/bind.hpp>

#include "network/io_service.h"
#include "network/udp_service_handler_observer.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace network {

// struct UdpServiceHandler::WriteMessage - begin
struct UdpServiceHandler::WriteMessage
{
    WriteMessage(unsigned char* _data, std::size_t _size,
                 asio::ip::udp::endpoint _destination_endpoint)
        : data(NULL), size(_size), destination_endpoint(_destination_endpoint)
    {
        data = new unsigned char[_size];
        memcpy(data, _data, size);
    }
    ~WriteMessage()
    {
        util::safe_delete_array(data);
    }

    unsigned char* data;
    std::size_t size;
    asio::ip::udp::endpoint destination_endpoint;
};
// struct UdpServiceHandler::WriteMessage - end

UdpServiceHandler::UdpServiceHandler(util::unique_id id,
                                     asio::io_service& io_service,
                                     UdpServiceHandlerObserver* observer)
    : m_id(id),
      m_strand(io_service),
      m_socket(io_service),
      m_service_handler_observer(observer)
{

}

UdpServiceHandler::~UdpServiceHandler()
{

}

void UdpServiceHandler::start()
{
    readAsync();
}

void UdpServiceHandler::stop()
{
    close();
}

bool UdpServiceHandler::bind(unsigned short port)
{
    asio::ip::udp ip4 = boost::asio::ip::udp::v4();

    boost::system::error_code ec;
    m_socket.open(ip4, ec);
    if (ec)
    {
        doClose();
        return false;
    }

    m_socket.bind(boost::asio::ip::udp::endpoint(ip4, port), ec);
    if (ec)
    {
        doClose();
        return false;
    }

    return true;
}

void UdpServiceHandler::close()
{
    m_strand.post(boost::bind(&UdpServiceHandler::doClose, shared_from_this()));
}

asio::ip::udp::socket& UdpServiceHandler::getSocket()
{
    return m_socket;
}

util::unique_id UdpServiceHandler::getId() const
{
    return m_id;
}

std::string UdpServiceHandler::getLocalIp() const
{
    try
    {
        return m_socket.local_endpoint().address().to_string();
    }
    catch (boost::system::error_code& e)
    {
        util::printLog(e);
        assert(false);

        return std::string();
    }
}

std::string UdpServiceHandler::getRemoteIp() const
{
    try
    {
        return m_socket.remote_endpoint().address().to_string();
    }
    catch (boost::system::error_code& e)
    {
        util::printLog(e);
        assert(false);

        return std::string();
    }
}

unsigned short UdpServiceHandler::getLocalPort() const
{
    try
    {
        return m_socket.local_endpoint().port();
    }
    catch (boost::system::error_code& e)
    {
        util::printLog(e);
        assert(false);

        return 0;
    }
}

unsigned short UdpServiceHandler::getRemotePort() const
{
    try
    {
        return m_socket.remote_endpoint().port();
    }
    catch (boost::system::error_code& e)
    {
        util::printLog(e);
        assert(false);

        return 0;
    }
}

void UdpServiceHandler::writeAsync(unsigned char* data, unsigned int size,
                                   std::string ip, unsigned short port)
{
    asio::ip::udp::endpoint destination_endpoint(
        asio::ip::address::from_string(ip),
        port);

    WriteMessagePtr write_msg(new WriteMessage(data,
                                               size,
                                               destination_endpoint));

    m_strand.post(boost::bind(&UdpServiceHandler::doWrite,
                              shared_from_this(),
                              write_msg));
}

void UdpServiceHandler::readAsync()
{
    m_strand.post(boost::bind(&UdpServiceHandler::doRead, shared_from_this()));
}

void UdpServiceHandler::doRead()
{
    m_socket.async_receive_from(asio::buffer(m_read_buffer),
        m_sender_endpoint,
        m_strand.wrap(boost::bind(&UdpServiceHandler::handleRead,
                      shared_from_this(),
                      asio::placeholders::error,
                      asio::placeholders::bytes_transferred)));
}

void UdpServiceHandler::handleRead(const boost::system::error_code& e, 
    std::size_t bytes_transferred)
{
    if (e)
    {
        util::printLog(e);

        doClose();
        return;
    }

    if (m_service_handler_observer)
    {
        m_service_handler_observer->onRead(m_read_buffer.data(),
                                           bytes_transferred,
                                           m_sender_endpoint.address().to_string(),
                                           m_sender_endpoint.port());
    }

    doRead();
}

void UdpServiceHandler::doWrite(WriteMessagePtr msg)
{
    bool write_in_progress = ! m_write_msg_deque.empty();

    if (! write_in_progress)
    {
        m_socket.async_send_to(asio::buffer(msg->data, msg->size),
                               msg->destination_endpoint,
                               m_strand.wrap(
                                   boost::bind(&UdpServiceHandler::handleWrite,
                                   shared_from_this(),
                                   boost::asio::placeholders::error)));
    }

    m_write_msg_deque.push_back(msg);
}

void UdpServiceHandler::handleWrite(const boost::system::error_code& e)
{
    if (e)
    {
        util::printLog(e);

        doClose();
        return;
    }

    // get written msg
    WriteMessagePtr written_msg;
    {
        written_msg = m_write_msg_deque.front();
        m_write_msg_deque.pop_front();
    }

    bool continue_to_write = ! m_write_msg_deque.empty();
    if (continue_to_write)
    {
        WriteMessagePtr write_msg = m_write_msg_deque.front();

        m_socket.async_send_to(asio::buffer(write_msg->data, write_msg->size),
                               write_msg->destination_endpoint,
                               m_strand.wrap(
                                   boost::bind(&UdpServiceHandler::handleWrite,
                                   shared_from_this(),
                                   boost::asio::placeholders::error)));
    }

    if (m_service_handler_observer)
    {
        m_service_handler_observer->onWrite(
            written_msg->data,
            written_msg->size,
            written_msg->destination_endpoint.address().to_string(),
            written_msg->destination_endpoint.port());
    }
}

void UdpServiceHandler::doClose()
{
    boost::system::error_code ignored_ec;

    m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both, ignored_ec);
    m_socket.close(ignored_ec);

    if (m_service_handler_observer)
    {
        m_service_handler_observer->onClose();
    }
}

} // namespace network
} // namespace rtsp_server
