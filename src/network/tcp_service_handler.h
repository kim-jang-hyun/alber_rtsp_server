#ifndef __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_H__
#define __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_H__

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <deque>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace rtsp_server {
namespace network {

namespace asio = boost::asio;

class TcpServiceHandlerObserver;

class TcpServiceHandler :
    public boost::enable_shared_from_this<TcpServiceHandler>,
    private boost::noncopyable
{
public:
    explicit TcpServiceHandler(asio::io_service& io_service);
    virtual ~TcpServiceHandler();

    virtual void start();
    virtual void stop();

    virtual void writeAsync(unsigned char* data, unsigned int size);
    void readAsync();

    // property funtion - begin
    void setObserver(TcpServiceHandlerObserver* observer);

    asio::ip::tcp::socket& getSocket();
    virtual std::string getLocalIp() const;
    virtual std::string getRemoteIp() const;
    virtual unsigned short getLocalPort() const;
    virtual unsigned short getRemotePort() const;
    // property funtion - end

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
    asio::strand m_strand;
    asio::ip::tcp::socket m_socket;

    boost::array<unsigned char, READ_BUFFER_SIZE> m_read_buffer;
    std::deque<WriteMessagePtr> m_write_msg_buffer;

    TcpServiceHandlerObserver* m_service_handler_observer;
};

typedef boost::shared_ptr<TcpServiceHandler> TcpServiceHandlerPtr;

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_TCP_SERVICE_HANDLER_H__
