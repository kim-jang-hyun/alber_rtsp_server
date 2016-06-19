#include "tcp_service_handler.h"

#include <iostream>
#include <boost/bind.hpp>

#include "network/tcp_service_handler_observer.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace network {

// struct TcpServiceHandler::WriteMessage - begin
struct TcpServiceHandler::WriteMessage
{
    WriteMessage(unsigned char* _data, std::size_t _size)
        : data(NULL), size(_size)
    {
        data = new unsigned char[_size];
        memcpy(data, _data, size);
    }
    ~WriteMessage()
    {
        rtsp_server::util::safe_delete_array(data);
    }

    unsigned char* data;
    std::size_t size;
};
// struct TcpServiceHandler::WriteMessage - end

TcpServiceHandler::TcpServiceHandler(asio::io_service& io_service)
  : m_strand(io_service),
    m_socket(io_service),
    m_service_handler_observer(NULL)
{

}

TcpServiceHandler::~TcpServiceHandler()
{

}

void TcpServiceHandler::start()
{
    readAsync();
}

void TcpServiceHandler::stop()
{
    m_strand.post(boost::bind(&TcpServiceHandler::doClose, shared_from_this()));
}

void TcpServiceHandler::writeAsync(unsigned char* data, unsigned int size)
{
    WriteMessagePtr write_msg(new WriteMessage(data, size));

    m_strand.post(boost::bind(&TcpServiceHandler::doWrite,
                              shared_from_this(),
                              write_msg));
}

void TcpServiceHandler::readAsync()
{
    m_strand.post(boost::bind(&TcpServiceHandler::doRead, shared_from_this()));
}

void TcpServiceHandler::setObserver(TcpServiceHandlerObserver* observer)
{
    m_service_handler_observer = observer;
}

asio::ip::tcp::socket& TcpServiceHandler::getSocket()
{
    return m_socket;
}

std::string TcpServiceHandler::getLocalIp() const
{
    return m_socket.local_endpoint().address().to_string();
}

std::string TcpServiceHandler::getRemoteIp() const
{
    return m_socket.remote_endpoint().address().to_string();
}

unsigned short TcpServiceHandler::getLocalPort() const
{
    return m_socket.local_endpoint().port();
}

unsigned short TcpServiceHandler::getRemotePort() const
{
    return m_socket.remote_endpoint().port();
}

void TcpServiceHandler::doRead()
{
    m_socket.async_read_some(
        asio::buffer(m_read_buffer),
        m_strand.wrap(boost::bind(&TcpServiceHandler::handleRead,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred)));
}

void TcpServiceHandler::handleRead(const boost::system::error_code& e, 
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
                                           bytes_transferred);
    }

    doRead();
}

void TcpServiceHandler::doWrite(WriteMessagePtr msg)
{
    bool write_in_progress = ! m_write_msg_buffer.empty();

    if (! write_in_progress)
    {
        boost::asio::async_write(m_socket,
                                 asio::buffer(msg->data, msg->size),
                                 m_strand.wrap(
                                     boost::bind(&TcpServiceHandler::handleWrite,
                                     shared_from_this(),
                                     boost::asio::placeholders::error)));
    }

    m_write_msg_buffer.push_back(msg);
}

void TcpServiceHandler::handleWrite(const boost::system::error_code& e)
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
        written_msg = m_write_msg_buffer.front();
        m_write_msg_buffer.pop_front();
    }

    bool continue_to_write = ! m_write_msg_buffer.empty();
    if (continue_to_write)
    {
        WriteMessagePtr write_msg = m_write_msg_buffer.front();

        boost::asio::async_write(m_socket,
                                 asio::buffer(write_msg->data, write_msg->size),
                                 m_strand.wrap(
                                     boost::bind(&TcpServiceHandler::handleWrite,
                                     shared_from_this(),
                                     boost::asio::placeholders::error)));
    }

    if (m_service_handler_observer)
    {
        m_service_handler_observer->onWrite(written_msg->data,
                                            written_msg->size);
    }
}

void TcpServiceHandler::doClose()
{
    boost::system::error_code ignored_ec;

    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    m_socket.close(ignored_ec);

    if (m_service_handler_observer)
    {
        m_service_handler_observer->onClose();
    }
}

} // namespace network
} // namespace rtsp_server
