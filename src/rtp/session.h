#ifndef __RTSP_SERVER_RTP_SESSION_H__
#define __RTSP_SERVER_RTP_SESSION_H__

#include "network/udp_service_handler_observer.h"

#include <string>
#include <boost/shared_ptr.hpp>

#include "network/udp_service_handler.h"
#include "rtp/header.h"

namespace rtsp_server {
namespace rtp {

class Session : public network::UdpServiceHandlerObserver
{
public:
    Session();

    bool bind(unsigned short port);
    void close();

    void sendRtpPacket(unsigned char* header, std::size_t header_size,
        unsigned char* data, std::size_t data_size,
        std::string ip, unsigned short port);

    // derived from network::UdpServiceHandlerObserver - begin
    virtual void onRead(unsigned char* /*data*/, std::size_t /*size*/,
                        std::string /*sender_address*/,
                        unsigned short /*sender_port*/) {}
    virtual void onWrite(unsigned char* /*data*/, std::size_t /*size*/,
                         std::string /*destination_address*/,
                         unsigned short /*destination_port*/) {}
    virtual void onClose() {}
    // derived from network::UdpServiceHandlerObserver - end

    // property function - begin
    void setServiceHandler(network::UdpServiceHandlerPtr service_handler);

    void setRemoteRtpAddress(std::string address) { m_remote_rtp_address = address; }
    void setRemoteRtcpAddress(std::string address) { m_remote_rtcp_address = address; }
    std::string getRemoteRtpAddress() const { return m_remote_rtp_address; }
    std::string getRemoteRtcpAddress() const { return m_remote_rtcp_address; }

    void setRemoteRtpPort(unsigned short port) { m_remote_rtp_port = port; }
    void setRemoteRtcpPort(unsigned short port) { m_remote_rtcp_port = port; }
    unsigned short getRemoteRtpPort() const { return m_remote_rtp_port; }
    unsigned short getRemoteRtcpPort() const { return m_remote_rtcp_port; }

    unsigned short getLocalPort() const;
    // property function - end

private:
    network::UdpServiceHandlerPtr m_service_handler;

    std::string m_remote_rtp_address;
    unsigned short m_remote_rtp_port;

    std::string m_remote_rtcp_address;
    unsigned short m_remote_rtcp_port;
};
typedef boost::shared_ptr<Session> SessionPtr;

} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_SESSION_H__
