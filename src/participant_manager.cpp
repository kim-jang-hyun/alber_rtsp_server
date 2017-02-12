#include "participant_manager.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {

ParticipantManager::ParticipantManager(
    task::WorkerPtr worker, rtsp::parser::ParserManagerPtr rtsp_parser_manager,
    rtp::StreamingService& streaming_service)
    : m_worker(worker),
      m_rtsp_parser_manager(rtsp_parser_manager),
      m_streaming_service(streaming_service)
{

}

void ParticipantManager::onAddStreaming(const util::unique_id participant_id)
{
    ParticipantPtr participant = findParticipant(participant_id);
    if (! participant)
    {
        return;
    }

    util::unique_id rtsp_session_id = participant->getRtspSessionId();
    rtp::SessionPtr rtp_session = participant->getRtpSession();
    std::string request_uri = participant->getRequestUri();

    m_streaming_service.add(rtsp_session_id, request_uri, rtp_session);
}

void ParticipantManager::onRemoveStreaming(const util::unique_id participant_id)
{
    ParticipantPtr participant = findParticipant(participant_id);
    if (! participant)
    {
        return;
    }

    util::unique_id rtsp_session_id = participant->getRtspSessionId();
    m_streaming_service.remove(rtsp_session_id);
}

void ParticipantManager::onPlayStreaming(const util::unique_id participant_id)
{
    ParticipantPtr participant = findParticipant(participant_id);
    if (! participant)
    {
        return;
    }

    util::unique_id rtsp_session_id = participant->getRtspSessionId();
    m_streaming_service.playStreaming(rtsp_session_id);
}

void ParticipantManager::onStopStreaming(const util::unique_id participant_id)
{
    ParticipantPtr participant = findParticipant(participant_id);
    if (! participant)
    {
        return;
    }

    util::unique_id rtsp_session_id = participant->getRtspSessionId();
    m_streaming_service.stopStreaming(rtsp_session_id);
}

ParticipantPtr ParticipantManager::addParticipant(
    const util::unique_id participant_id, rtsp::SessionPtr session)
{
    boost::unique_lock<boost::mutex> lock(m_participant_map_mutex);

    ParticipantPtr participant(new Participant(participant_id,
                                               this,
                                               session,
                                               m_worker,
                                               m_rtsp_parser_manager));
    m_participant_map[participant_id] = participant;

    return participant;
}

ParticipantPtr ParticipantManager::findParticipant(
                                   const util::unique_id participant_id) const
{
    boost::unique_lock<boost::mutex> lock(m_participant_map_mutex);

    ParticipantPtrMap::const_iterator it = m_participant_map.find(participant_id);
    if (it == m_participant_map.end())
    {
        return ParticipantPtr();
    }

    ParticipantPtr participant = it->second;
    return participant;
}

} // namespace rtsp_server
