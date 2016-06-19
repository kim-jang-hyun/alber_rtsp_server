#include "task/request_method_setup_task.h"

#include <iostream>
#include "rtsp/message/response/setup_message.h"
#include "util/utf8_converter.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

namespace rtp = rtsp_server::rtp;

const int RTP_PORT_BEGIN = 5004;
const int RTP_PORT_END = 65535;

bool bindAvailablePort(rtp::SessionPtr rtp_session, rtp::SessionPtr rtcp_session)
{
    bool bind_ret = false;

    for (unsigned short port = RTP_PORT_BEGIN; port < RTP_PORT_END; port+=2)
    {
        bind_ret = rtp_session->bind(port);
        if (! bind_ret)
        {
            continue;
        }

        bind_ret = rtcp_session->bind(port+1);
        if (! bind_ret)
        {
            continue;
        }

        return true;
    }

    return false;
}

bool getAvaiablePort(rtsp_server::ParticipantPtr participant,
                     unsigned short& rtp_port,
                     unsigned short& rtcp_port)
{
    rtp::SessionPtr rtp_session = participant->getRtpSession();
    rtp::SessionPtr rtcp_session = participant->getRtcpSession();

    if (! bindAvailablePort(rtp_session, rtcp_session))
    {
        return false;
    }

    rtp_port = rtp_session->getLocalPort();
    rtcp_port = rtcp_session->getLocalPort();

    std::cout << "bind local port : " << "rtp(" << rtp_port << "), "
                                      << "rtcp(" << rtcp_port << ")" << std::endl;

    return true;
}

} // namespace

namespace rtsp_server {
namespace task {

RequestMethodSetupTask::RequestMethodSetupTask(
    const rtsp::message::request::MessageItem& msg_item,
    ParticipantPtr participant)
    : Task(),
    m_request_message_item(msg_item),
    m_participant(participant)
{
    m_participant->createRtpSession();
    m_participant->createRtcpSession();
}

void RequestMethodSetupTask::execute()
{
    unsigned short rtp_port = 0;
    unsigned short rtcp_port = 0;
    if (! getAvaiablePort(m_participant, rtp_port, rtcp_port))
    {
        return;
    }

    rtsp::message::response::SetupMessage response_setup_msg(rtp_port,
                                                             rtcp_port);
    bool valid_msg = response_setup_msg.interprete(m_request_message_item);
    if (! valid_msg)
    {
        return;
    }

    std::string serialized_msg_utf8;
    std::wstring serialized_msg_utf16 = response_setup_msg.serialize();

    bool convert_ret = util::convertUtf16ToUtf8(serialized_msg_utf16,
                                                serialized_msg_utf8);
    if (! convert_ret)
    {
        return;
    }

    rtsp::SessionPtr rtsp_session = m_participant->getRtspSession();
    rtsp_session->sendRtspPacket(((unsigned char*)serialized_msg_utf8.c_str()),
                                 serialized_msg_utf8.size());

    // set session info
    std::string rtp_remote_ip, rtcp_remote_ip;
    unsigned short rtp_remote_port, rtcp_remote_port;
    {
        rtp_remote_ip = rtsp_session->getRemoteAddress();
        rtp_remote_port = response_setup_msg.getRemoteRtpPort();

        rtcp_remote_ip = rtsp_session->getRemoteAddress();
        rtcp_remote_port = response_setup_msg.getRemoteRtcpPort();

        util::unique_id rtsp_session_id = response_setup_msg.getSessionId();
        m_participant->setRtspSessionId(rtsp_session_id);

        m_participant->setRtpRemoteAddress(rtp_remote_ip);
        m_participant->setRtpRemotePort(rtp_remote_port);

        m_participant->setRtcpRemoteAddress(rtcp_remote_ip);
        m_participant->setRtcpRemotePort(rtcp_remote_port);
    }

    // log
    {
        std::cout << "rtp address : " << "ip(" << rtp_remote_ip
                                      << "), port(" << rtp_remote_port
                                      << std::endl;
        std::cout << "rtcp address : " << "ip(" << rtcp_remote_ip
                                       << "), port(" << rtcp_remote_port
                                       << std::endl;
    }

    m_participant->addStreaming();
}

} // namespace task
} // namespace rtsp_server
