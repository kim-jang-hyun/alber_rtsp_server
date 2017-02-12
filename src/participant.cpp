#include "participant.h"

#include <iostream>
#include <boost/assert.hpp>

#include "network/io_service.h"
#include "task/request_method_describe_task.h"
#include "task/request_method_options_task.h"
#include "task/request_method_play_task.h"
#include "task/request_method_setup_task.h"
#include "task/request_method_teardown_task.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {

Participant::Participant(util::unique_id participant_id,
                         ParticipantObserver* observer,
                         rtsp::SessionPtr rtsp_session,
                         task::WorkerPtr worker,
                         rtsp::parser::ParserManagerPtr rtsp_parser_manager)
    : m_participant_id(participant_id),
      m_observer(observer),
      m_rtsp_session(rtsp_session),
      m_rtsp_parser_manager(rtsp_parser_manager),
      m_worker(worker)
{
    BOOST_ASSERT(m_observer != NULL);
    m_rtsp_session->init(m_rtsp_parser_manager, m_worker);
}

void Participant::onReceive(rtsp::message::MessageInterfacePtr msg)
{
    task::TaskPtr task = createTask(msg);
    if (task)
    {
        m_worker->postTask(task);
    }
}

void Participant::createRtpSession()
{
    m_rtp_session.reset(new rtp::Session);

    network::UdpServiceHandlerPtr rtp_service_handler;
    {
        network::IoService& io_service =
            network::IoService::get_mutable_instance();

        rtp_service_handler.reset(new network::UdpServiceHandler(
                                      util::generateId(),
                                      io_service.getIoService(),
                                      m_rtp_session.get()));
    }

    m_rtp_session->setServiceHandler(rtp_service_handler);
}

void Participant::createRtcpSession()
{
    m_rtcp_session.reset(new rtp::Session);

    network::UdpServiceHandlerPtr rtcp_service_handler;
    {
        network::IoService& io_service =
            network::IoService::get_mutable_instance();

        rtcp_service_handler.reset(new network::UdpServiceHandler(
                                      util::generateId(),
                                      io_service.getIoService(),
                                      m_rtcp_session.get()));
    }

    m_rtcp_session->setServiceHandler(rtcp_service_handler);
}

void Participant::addStreaming()
{
    m_observer->onAddStreaming(m_participant_id);
}

void Participant::removeStreaming()
{
    m_observer->onRemoveStreaming(m_participant_id);
}

void Participant::playStreaming()
{
    m_observer->onPlayStreaming(m_participant_id);
}

void Participant::stopStreaming()
{
    m_observer->onStopStreaming(m_participant_id);
}

void Participant::setRtspSessionId(const util::unique_id rtsp_session_id)
{
    m_rtsp_session->setSessionId(rtsp_session_id);
}

util::unique_id Participant::getRtspSessionId() const
{
    return m_rtsp_session->getSessonId();
}

void Participant::setRtpRemoteAddress(const std::string address)
{
    m_rtp_session->setRemoteRtpAddress(address);
}

void Participant::setRtpRemotePort(const unsigned short port)
{
    m_rtp_session->setRemoteRtpPort(port);
}

void Participant::setRtcpRemoteAddress(const std::string address)
{
    m_rtcp_session->setRemoteRtcpAddress(address);
}

void Participant::setRtcpRemotePort(const unsigned short port)
{
    m_rtcp_session->setRemoteRtcpPort(port);
}

void Participant::setRequestUri(const std::string request_uri)
{
    m_request_uri = request_uri;
}

task::TaskPtr Participant::createTask(
    const rtsp::message::MessageInterfacePtr msg)
{
    namespace rtsp_request = rtsp::message::request;
    using rtsp::message::MessageInterface;

    if (msg->isRequestMessage())
    {
        rtsp_request::MessagePtr request_msg =
            boost::dynamic_pointer_cast<rtsp_request::Message>(msg);

        rtsp_request::MessageItem msg_item = request_msg->getMsgItem();

        switch (request_msg->getType())
        {
        case MessageInterface::REQUEST_METHOD_OPTIONS :
            {
                return task::TaskPtr(
                    new task::RequestMethodOptionsTask(msg_item,
                                                       shared_from_this()));
            }
        case MessageInterface::REQUEST_METHOD_DESCRIBE :
            {
                return task::TaskPtr(
                    new task::RequestMethodDescribeTask(msg_item,
                                                        shared_from_this()));
            }
        case MessageInterface::REQUEST_METHOD_SETUP :
            {
                return task::TaskPtr(
                    new task::RequestMethodSetupTask(msg_item,
                                                     shared_from_this()));
            }
        case MessageInterface::REQUEST_METHOD_PLAY :
            {
                return task::TaskPtr(
                    new task::RequestMethodPlayTask(msg_item,
                                                    shared_from_this()));
            }
        case MessageInterface::REQUEST_METHOD_TEARDOWN :
            {
                return task::TaskPtr(
                    new task::RequestMethodTeardownTask(msg_item,
                                                        shared_from_this()));
            }
        }
    }

    return NULL_SHARED_PTR(task::TaskPtr);
}

} // namespace rtsp_server
