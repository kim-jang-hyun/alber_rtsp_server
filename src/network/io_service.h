#ifndef __RTSP_SERVER_NETWORK_IO_SERVICE_H__
#define __RTSP_SERVER_NETWORK_IO_SERVICE_H__

// singleton
#include <boost/serialization/singleton.hpp>
#define Singleton boost::serialization::singleton

#include <boost/asio.hpp>

namespace rtsp_server {
namespace network {

namespace asio = boost::asio;

class IoService : public Singleton<IoService>
{
public:
    void run(std::size_t thread_pool_size);

    asio::io_service& getIoService();

protected:
    IoService();

private:
    void handleStop();

private:
    asio::io_service m_io_service;
    asio::signal_set m_signal_set;
};

} // namespace network
} // namespace rtsp_server

#endif // __RTSP_SERVER_NETWORK_IO_SERVICE_H__
