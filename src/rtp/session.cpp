#include "rtp/session.h"

#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtp {

Session::Session() : m_remote_rtp_port(0), m_remote_rtcp_port(0)
{

}

bool Session::bind(unsigned short port)
{
    return m_service_handler->bind(port);
}

void Session::close()
{
    m_service_handler->close();
}

void Session::sendRtpPacket(unsigned char* header, std::size_t header_size,
                            unsigned char* data, std::size_t data_size,
                            std::string ip, unsigned short port)
{
    size_t send_buf_len = header_size + data_size;
    unsigned char* send_buf = new unsigned char[send_buf_len];
    memcpy(send_buf, header, header_size);
    memcpy(send_buf + header_size, data, data_size);

    m_service_handler->writeAsync(send_buf, send_buf_len, ip, port);

    util::safe_delete_array(send_buf);
}

void Session::setServiceHandler(network::UdpServiceHandlerPtr service_handler)
{
    m_service_handler = service_handler;
}

unsigned short Session::getLocalPort() const
{
    return m_service_handler->getLocalPort();
}

} // namespace rtp
} // namespace rtsp_server
