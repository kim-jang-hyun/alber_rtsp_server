#ifndef __RTSP_SERVER_NETWORK_UDP_SERVICE_HANDLER_H__
#define __RTSP_SERVER_NETWORK_UDP_SERVICE_HANDLER_H__

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <deque>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "util/unique_id.h"
#include "util/util.h"

namespace rtsp_server {
namespace network {

namespace asio = boost::asio;

class UdpServiceHandlerObserver;

class UdpServiceHandler : public boost::enable_shared_from_this<UdpServiceHandler>,
                          private boost::noncopyable
{
public:
    UdpServiceHandler(util::unique_id id, asio::io_service& io_service,
                      UdpServiceHandlerObserver* observer);
    virtual ~UdpServiceHandler();

    virtual void start();
    virtual void stop();

    virtual bool bind(unsigned short port);
    virtual void close();

    virtual void writeAsync(unsigned char* data, unsigned int size,
                            std::string ip, unsigned short port);
    void readAsync();

    // property function - begin
    asio::ip::udp::socket& getSocket();
    virtual util::unique_id getId() const;

    virtual std::string getLocalIp() const;
    virtual std::string getRemoteIp() const;
    virtual unsigned short getLocalPort() const;
    virtual unsigned short getRemotePort() const;
    // property function - end

private:
    // type declare - begin
    static const int READ_BUFFER_SIZE = 8192;

    struct WriteMessage;
    typedef boost::shared_ptr<WriteMessage> WriteMessagePtr;
    // type declare - end

    void doRead();
    void handleRead(const boost::system::error_code& e,
                    std::size_t bytes_transferred);

    void doWrite(WriteMessagePtr msg);
    void handleWrite(const boost::system::error_code& e);

    void doClose();

private:
    util::unique_id m_id;

    asio::strand m_strand;
    asio::ip::udp::socket m_socket;
    asio::ip::udp::endpoint m_sender_endpoint;

    boost::array<unsigned char, READ_BUFFER_SIZE> m_read_buffer;
    std::deque<WriteMessagePtr> m_write_msg_deque;

    UdpServiceHandlerObserver* m_service_handler_observer;
};

typedef boost::shared_ptr<UdpServiceHandler> UdpServiceHandlerPtr;

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_UDP_SERVICE_HANDLER_H__
