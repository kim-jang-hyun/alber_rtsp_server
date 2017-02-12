#ifndef __RTSP_SERVER_PARTICIPANT_H__
#define __RTSP_SERVER_PARTICIPANT_H__

#include "rtsp/session_observer.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "network/tcp_service_handler.h"
#include "participant_observer.h"
#include "rtp/session.h"
#include "rtsp/message/message_interface.h"
#include "rtsp/parser/parser_manager.h"
#include "rtsp/session.h"
#include "task/task.h"
#include "task/worker.h"
#include "util/unique_id.h"

namespace rtsp_server {

class Participant : public rtsp::SessionObserver,
                    public boost::enable_shared_from_this<Participant>
{
public:
    Participant(util::unique_id participant_id,
                ParticipantObserver* observer,
                rtsp::SessionPtr rtsp_session,
                task::WorkerPtr worker,
                rtsp::parser::ParserManagerPtr rtsp_parser_manager);

    // derived from SessionObserver - begin
    virtual void onReceive(rtsp::message::MessageInterfacePtr msg);
    // derived from SessionObserver - end

    void createRtpSession();
    void createRtcpSession();

    void addStreaming();
    void removeStreaming();

    void playStreaming();
    void stopStreaming();

    // property function - begin
    void setRtspSessionId(const util::unique_id rtsp_session_id);
    util::unique_id getRtspSessionId() const;

    void setRtpRemoteAddress(const std::string address);
    void setRtpRemotePort(const unsigned short port);

    void setRtcpRemoteAddress(const std::string address);
    void setRtcpRemotePort(const unsigned short port);

    rtsp::SessionPtr getRtspSession() const  { return m_rtsp_session; }
    rtp::SessionPtr getRtpSession() const { return m_rtp_session; }
    rtp::SessionPtr getRtcpSession() const { return m_rtcp_session; }

    void setRequestUri(const std::string request_uri);
    std::string getRequestUri() const { return m_request_uri; }
    // property function - end

private:
    task::TaskPtr createTask(
        const rtsp::message::MessageInterfacePtr msg);

private:
    util::unique_id m_participant_id;
    std::string m_request_uri;

    ParticipantObserver* m_observer;

    rtsp::SessionPtr m_rtsp_session;
    rtsp::parser::ParserManagerPtr m_rtsp_parser_manager;

    rtp::SessionPtr m_rtp_session;
    rtp::SessionPtr m_rtcp_session;

    task::WorkerPtr m_worker;
};
typedef boost::shared_ptr<Participant> ParticipantPtr;

} // namespace rtsp_server;

#endif // __RTSP_SERVER_PARTICIPANT_H__
