#pragma warning (disable : 4005)

#include "rtp/streaming_service.h"

#include <fstream>
#include <boost/foreach.hpp>

#include "rtp/header.h"
#include "rtp/payload/jpeg_frame_loader.h"
#include "util/path_util.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

using namespace rtsp_server::rtp;
namespace chrono = boost::chrono;

const unsigned short DEFAULT_FRAME_RATE = 30;

//const size_t FRAME_FRAGMENTAION_SIZE = 512;
const size_t FRAME_FRAGMENTAION_SIZE = 8096;

chrono::steady_clock::time_point now()
{
    return chrono::steady_clock::now();
}

StreamingService::time_point init_time_point()
{
    typedef StreamingService::time_point time_point;

    static time_point init_time_point = time_point::min();
    return init_time_point;
}

chrono::nanoseconds toDuration(unsigned short fps)
{
    chrono::duration<double> fps_duration(1./fps);

    chrono::nanoseconds duration_nanosec =
        chrono::duration_cast<chrono::nanoseconds>(fps_duration);

    if (duration_nanosec < fps_duration)
    {
        ++duration_nanosec;
    }

    return duration_nanosec;
}

template <class Clock, class Duration1, class Duration2>
inline
typename boost::common_type<chrono::time_point<Clock, Duration1>,
                            chrono::time_point<Clock, Duration2> >::type
min BOOST_PREVENT_MACRO_SUBSTITUTION (chrono::time_point<Clock, Duration1> t1,
                                      chrono::time_point<Clock, Duration2> t2)
{
    return t2 < t1 ? t2 : t1;
}

void saveToFile(const std::string& file_name,
                const char* data, std::size_t data_size)
{
    std::string jpg_file_name = file_name + ".jpg";

    std::fstream out(jpg_file_name.c_str(), std::ios::binary | std::ios::out);
    out.write(data, data_size);
}
} // namespace

namespace rtsp_server {
namespace rtp {

void StreamingService::start()
{
    m_thread = boost::thread(&StreamingService::thread_func, this);
}

void StreamingService::stop()
{
    m_thread.interrupt();
    m_thread.join();
}

void StreamingService::add(const util::unique_id& id, SessionPtr session)
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    SessionDataPtr session_data(new SessionData);
    session_data->session = session;
    session_data->play_state = false;
    session_data->next_presentation_time = init_time_point();
    session_data->frame_rate = DEFAULT_FRAME_RATE;

    PacketCreatorPtr packet_creator(new PacketCreator);
    packet_creator->setFrameRate(DEFAULT_FRAME_RATE);
    session_data->packet_creator = packet_creator;

    m_session_data_map[id] = session_data;
}

void StreamingService::remove(const util::unique_id& id)
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    m_session_data_map.erase(id);
}

bool StreamingService::playStreaming(const util::unique_id& id)
{
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);

        SessionDataMap::iterator it = m_session_data_map.find(id);
        if (it == m_session_data_map.end())
        {
            return false;
        }

        m_session_data_map[id]->play_state = true;
    }

    m_condition_variable.notify_one();

    return true;
}

bool StreamingService::stopStreaming(const util::unique_id& id)
{
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);

        SessionDataMap::iterator it = m_session_data_map.find(id);
        if (it == m_session_data_map.end())
        {
            return false;
        }

        m_session_data_map[id]->play_state = false;
    }

    return true;
}

void StreamingService::waitIfActivatedSessionIsEmpty()
{
    struct SessionStateChecker
    {
        static bool activeteSessionIsExisted(
            const SessionDataMap& session_data_map)
        {
            BOOST_FOREACH(const SessionDataMap::value_type& v, session_data_map)
            {
                const SessionDataPtr& session_data = v.second;
                if (session_data->play_state == true)
                {
                    return true;
                }
            }
            return false;
        }
    };

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (! SessionStateChecker::activeteSessionIsExisted(m_session_data_map))
    {
        m_condition_variable.wait(lock);
    }
}

void StreamingService::getNextPresentationTimeSessionData(time_point now,
                                               SessionDataVec& session_data_vec)
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    BOOST_FOREACH(SessionDataMap::value_type& v, m_session_data_map)
    {
        SessionDataPtr session_data = v.second;

        if (! session_data->play_state)
        {
            continue;
        }

        chrono::nanoseconds fps_duration = toDuration(session_data->frame_rate);

        if (session_data->next_presentation_time == init_time_point())
        {
            session_data->next_presentation_time = now + fps_duration;
            session_data_vec.push_back(session_data);
        }
        else if (session_data->next_presentation_time <= now)
        {
            session_data->next_presentation_time += fps_duration;
            session_data_vec.push_back(session_data);
        }
        else
        {
            // do nothing.
        }
    }
}

void StreamingService::sendSingleFrame(
    streaming_service_helper::JpegParserOutput& jpeg_parser_output,
    SessionDataVec& session_data_vec)
{
    std::size_t fragment_size = 0;
    std::size_t fragment_offset = 0;

    std::size_t scan_data_size = jpeg_parser_output.scandataLength;
    while (fragment_offset < scan_data_size)
    {
        unsigned char* fragment_buf =
            jpeg_parser_output.scandata + fragment_offset;

        if (fragment_offset + FRAME_FRAGMENTAION_SIZE < scan_data_size)
        {
            fragment_size = FRAME_FRAGMENTAION_SIZE;
        }
        else
        {
            fragment_size = scan_data_size - fragment_offset;
        }

        bool first_fragment = fragment_offset == 0 ? true : false;
        bool last_fragment = 
            (fragment_offset + fragment_size == scan_data_size) ?
            true : false;

        // send fragment
        BOOST_FOREACH(const SessionDataPtr session_data, session_data_vec)
        {
            PacketCreatorPtr packet_creator = session_data->packet_creator;

            payload::JpegPayload payload;
            makePayload(jpeg_parser_output, fragment_buf,
                fragment_size, fragment_offset, payload);

            unsigned char* rtp_header = NULL;
            std::size_t rtp_header_size = 0;
            {
                packet_creator->createHeader(first_fragment, last_fragment,
                                             rtp_header, rtp_header_size);
            }

            unsigned char* rtp_data = NULL;
            std::size_t rtp_data_size = 0;
            {
                packet_creator->createData(payload, rtp_data, rtp_data_size);
            }

            // send
            {
                std::string remote_address =
                    session_data->session->getRemoteRtpAddress();
                unsigned short remote_port = 
                    session_data->session->getRemoteRtpPort();

                session_data->session->sendRtpPacket(rtp_header,
                                                     rtp_header_size,
                                                     rtp_data,
                                                     rtp_data_size,
                                                     remote_address,
                                                     remote_port);
            }
            util::safe_delete_array(rtp_data);
            util::safe_delete_array(rtp_header);
        }

        fragment_offset += FRAME_FRAGMENTAION_SIZE;
    }
}

void StreamingService::thread_func()
{
    std::string exe_path = util::path_util::getExeDirectory();
    std::string stream_file_name = "toy_plane_liftoff.avi";

    std::string stream_file_path = exe_path + "\\" + stream_file_name;
    payload::JpegFrameLoader jpeg_frame_loader(stream_file_path);

    try
    {
        for(;;)
        {
            boost::this_thread::interruption_point();

            // wait & get activate session
            SessionDataVec session_data_vec;
            {
                waitIfActivatedSessionIsEmpty();

                getNextPresentationTimeSessionData(now(), session_data_vec);
            }

            // get frame data
            unsigned char* frame_buf = NULL;
            std::size_t frame_buf_size = 0;
            bool get_frame_ret = jpeg_frame_loader.getNextFrame(frame_buf,
                                                                frame_buf_size);
            if (! get_frame_ret)
            {
                continue;
            }

            // parse jpeg frame
            streaming_service_helper::JpegParserOutput jpeg_parser_output;
            {
                payload::JpegFrameParser jpeg_frame_parser;
                bool parse_ret = jpeg_frame_parser.parse(frame_buf,
                                                         frame_buf_size);
                BOOST_VERIFY(parse_ret == true);

                getJpegParserOutput(jpeg_frame_parser, jpeg_parser_output);
            }

            sendSingleFrame(jpeg_parser_output, session_data_vec);

            util::safe_delete_array(frame_buf);

            // sleep
            {
                time_point shortest_next_presentation_time_from_now = 
                    time_point::max();
                BOOST_FOREACH(const SessionDataPtr& session_data, session_data_vec)
                {
                    shortest_next_presentation_time_from_now =
                        min(shortest_next_presentation_time_from_now,
                            session_data->next_presentation_time);
                }

                boost::this_thread::sleep_until(shortest_next_presentation_time_from_now);
            }
        }
    }
    catch (boost::thread_interrupted& /*e*/)
    {
        std::cout << "streaming service is interrupted." << std::endl;
    }
}

} //namespace rtp
} // namespace rtsp_server
