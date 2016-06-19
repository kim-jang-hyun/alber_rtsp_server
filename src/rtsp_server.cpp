#include "rtsp_server.h"

#include <boost/thread/thread.hpp>

#include "network/io_service.h"
#include "rtsp/parser/request_parser.h"
#include "rtsp/session.h"
#include "util/unique_id.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {

RtspServer::RtspServer(unsigned short port)
    : m_worker(new task::Worker),
      m_rtsp_parser_manager(new rtsp::parser::ParserManager),
      m_streaming_service(),
      m_participant_manager(new ParticipantManager(m_worker,
                                                   m_rtsp_parser_manager,
                                                   m_streaming_service))
{
    network::IoService& io_service = network::IoService::get_mutable_instance();
    m_acceptor.reset(new network::Acceptor(io_service, port, this));

    m_rtsp_parser_manager->addParser(new rtsp::parser::RequestParser);
}

RtspServer::~RtspServer()
{

}

void RtspServer::onAccept(network::TcpServiceHandlerPtr service_handler)
{
    rtsp::SessionPtr rtsp_session(new rtsp::Session);
    {
        service_handler->setObserver(rtsp_session.get());
        rtsp_session->setServiceHandler(service_handler);
    }

    ParticipantPtr participant;
    {
        util::unique_id participant_id = util::generateId();
        participant = m_participant_manager->addParticipant(participant_id,
                                                            rtsp_session);
    }
    rtsp_session->setObserver(participant.get());

    doAccept();
}

void RtspServer::run()
{
    doAccept();

    m_streaming_service.start();

    {
        static const int SINGLE_THREAD_WORKER = 1;
        m_worker->start(SINGLE_THREAD_WORKER);
    }

    {
        unsigned int core_number = boost::thread::hardware_concurrency();
        std::size_t thread_number = (core_number * 2) + 1;

        network::IoService& io_service =
            network::IoService::get_mutable_instance();
        io_service.run(thread_number);
    }
}

void RtspServer::doAccept()
{
    m_acceptor->accept();
}

} // namespace rtsp_server
