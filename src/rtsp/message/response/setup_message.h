#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_SETUP_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_SETUP_MESSAGE_H__

#include "rtsp/message/response/message.h"

#include "rtsp/message/request/message_item.h"
#include "util/unique_id.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

class SetupMessage : public Message
{
public:
    SetupMessage(unsigned short rtp_port, unsigned short rtcp_port);
    virtual ~SetupMessage();

    // derived from Message - begin
    virtual bool interprete(const request::MessageItem& request_msg_item);
    // derived from Message - end

    util::unique_id getSessionId() const { return m_session_id; }
    unsigned short getRemoteRtpPort() const { return m_client_rtp_port; }
    unsigned short getRemoteRtcpPort() const { return m_client_rtcp_port; }

private:
    void addSession();
    void addTransport(const request::MessageItem& request_msg_item);

private:
    unsigned short m_client_rtp_port;
    unsigned short m_client_rtcp_port;

    unsigned short m_server_rtp_port;
    unsigned short m_server_rtcp_port;

    util::unique_id m_session_id;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_SETUP_MESSAGE_H__
