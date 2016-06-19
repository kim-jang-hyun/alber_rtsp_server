#ifndef __RTSP_SERVER_RTSP_SERVER_H__
#define __RTSP_SERVER_RTSP_SERVER_H__

#include "network/acceptor_observer.h"

#include "network/acceptor.h"
#include "network/tcp_service_handler.h"
#include "participant_manager.h"
#include "rtp/streaming_service.h"
#include "rtsp/parser/parser_manager.h"
#include "task/worker.h"

namespace rtsp_server {

class RtspServer : public network::AcceptorObserver
{
public:
    static const int DEFAULT_RTSP_SESSION_PORT = 554;

    explicit RtspServer(unsigned short port);
    ~RtspServer();

    // derived from network::AcceptorObserver - begin
    virtual void onAccept(network::TcpServiceHandlerPtr service_handler);
    // derived from network::AcceptorObserver - end

    void run();

private:
    void doAccept();

private:
    network::AcceptorPtr m_acceptor;
    task::WorkerPtr m_worker;

    rtsp::parser::ParserManagerPtr m_rtsp_parser_manager;

    rtp::StreamingService m_streaming_service;
    ParticipantManagerPtr m_participant_manager;
};

} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_SERVER_H__