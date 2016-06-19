#ifndef __RTSP_SERVER_RTP_STREAMING_SERVICE_H__
#define __RTSP_SERVER_RTP_STREAMING_SERVICE_H__

#include <vector>
#include <map>

#include <boost/chrono.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "rtp/packet_creator.h"
#include "rtp/session.h"
#include "rtp/streaming_service_helper.h"
#include "util/unique_id.h"

namespace rtsp_server {
namespace rtp {

class StreamingService
{
public:
    // type declare - begin
    typedef boost::chrono::steady_clock::time_point time_point;

    struct SessionData
    {
        SessionPtr session;
        bool play_state;
        time_point next_presentation_time;
        unsigned short frame_rate;
        PacketCreatorPtr packet_creator;
    };
    typedef boost::shared_ptr<SessionData> SessionDataPtr;

    typedef std::map<util::unique_id, SessionDataPtr> SessionDataMap;
    typedef std::vector<SessionDataPtr> SessionDataVec;
    // type declare - end

    void start();
    void stop();

    void add(const util::unique_id& id, SessionPtr session);
    void remove(const util::unique_id& id);

    bool playStreaming(const util::unique_id& id);
    bool stopStreaming(const util::unique_id& id);

    void waitIfActivatedSessionIsEmpty();

private:
    void getNextPresentationTimeSessionData(time_point now,
                                            SessionDataVec& session_data_vec);
    void sendSingleFrame(
        streaming_service_helper::JpegParserOutput& jpeg_parser_output,
        SessionDataVec& session_data_vec);

    void thread_func();

private:
    boost::thread m_thread;

    SessionDataMap m_session_data_map;
    boost::condition_variable m_condition_variable;

    mutable boost::mutex m_mutex;
};

} //namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_STREAMING_SERVICE_H__
