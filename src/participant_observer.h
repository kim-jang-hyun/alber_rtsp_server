#ifndef __RTSP_SERVER_PARTICIPANT_OBSERVER_H__
#define __RTSP_SERVER_PARTICIPANT_OBSERVER_H__

#include "util/unique_id.h"

namespace rtsp_server {

class ParticipantObserver
{
public:
    virtual void onAddStreaming(const util::unique_id participant_id) = 0;
    virtual void onRemoveStreaming(const util::unique_id participant_id) = 0;
    virtual void onPlayStreaming(const util::unique_id participant_id) = 0;
    virtual void onStopStreaming(const util::unique_id participant_id) = 0;
};

} // namespace rtsp_server

#endif // __RTSP_SERVER_PARTICIPANT_OBSERVER_H__
