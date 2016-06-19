#ifndef __RTSP_SERVER_PARTICIPANT_MANAGER_H__
#define __RTSP_SERVER_PARTICIPANT_MANAGER_H__

#include "participant_observer.h"

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "participant.h"
#include "rtp/streaming_service.h"
#include "rtsp/parser/parser_manager.h"
#include "rtsp/session.h"
#include "util/unique_id.h"

namespace rtsp_server {

class ParticipantManager : public ParticipantObserver
{
public:
    ParticipantManager(task::WorkerPtr worker,
                       rtsp::parser::ParserManagerPtr rtsp_parser_manager,
                       rtp::StreamingService& streaming_service);

    // derived from ParticipantObserver - begin
    virtual void onAddStreaming(const util::unique_id participant_id);
    virtual void onRemoveStreaming(const util::unique_id participant_id);
    virtual void onPlayStreaming(const util::unique_id participant_id);
    virtual void onStopStreaming(const util::unique_id participant_id);
    // derived from ParticipantObserver - end

    ParticipantPtr addParticipant(const util::unique_id participant_id,
                                  rtsp::SessionPtr session);

private:
    ParticipantPtr findParticipant(const util::unique_id participant_id) const;

private:
    typedef std::map<util::unique_id, ParticipantPtr> ParticipantPtrMap;

    task::WorkerPtr m_worker;
    rtsp::parser::ParserManagerPtr m_rtsp_parser_manager;
    rtp::StreamingService& m_streaming_service;

    ParticipantPtrMap m_participant_map;
    mutable boost::mutex m_participant_map_mutex;
};
typedef boost::shared_ptr<ParticipantManager> ParticipantManagerPtr;

} // namespace rtsp_server

#endif // __RTSP_SERVER_PARTICIPANT_MANAGER_H__
