#pragma warning (disable : 4005)

#include "rtp/streaming_service.h"

#include <fstream>
#include <boost/foreach.hpp>

#include "rtp/header.h"
#include "rtp/payload/jpeg/jpeg_payload.h"
#include "rtp/payload/h264/h264_payload.h"
#include "rtp/payload/payload_common.h"
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
    std::string jpg_file_name = file_name + ".h264";

    std::fstream out(jpg_file_name.c_str(), std::ios::binary | std::ios::out);
    out.write(data, data_size);
}

const unsigned char* fragmentScanData(unsigned char* scan_data,
                                      const std::size_t scan_data_size,
                                      const std::size_t fragment_offset,
                                      std::size_t& fragment_size,
                                      bool& first_fragment,
                                      bool& last_fragment)
{
    namespace payload = rtsp_server::rtp::payload;

    if (fragment_offset + payload::FRAGMENTAION_SIZE < scan_data_size)
    {
        fragment_size = payload::FRAGMENTAION_SIZE;
    }
    else
    {
        fragment_size = scan_data_size - fragment_offset;
    }

    first_fragment = fragment_offset == 0 ? true : false;
    last_fragment =  (fragment_offset + fragment_size == scan_data_size) ?
                      true : false;

    return scan_data + fragment_offset;
}

void sendRtpPacket(const rtsp_server::rtp::SessionPtr& session,
                   const rtsp_server::rtp::RtpPacket& rtp_packet)
{
    std::string remote_address = session->getRemoteRtpAddress();
    unsigned short remote_port =  session->getRemoteRtpPort();

    session->sendRtpPacket(rtp_packet.rtp_header,
                           rtp_packet.rtp_header_size,
                           rtp_packet.rtp_data,
                           rtp_packet.rtp_data_size,
                           remote_address,
                           remote_port);
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

void StreamingService::add(const util::unique_id& id,
                           const std::string& request_uri, SessionPtr session)
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    SessionDataPtr session_data(new SessionData);
    session_data->request_uri = request_uri;
    session_data->session = session;
    session_data->play_state = false;
    session_data->next_presentation_time = init_time_point();
    session_data->frame_rate = payload::DEFAULT_FRAME_RATE;

    payload::FrameLoaderPtr frame_loader;
    {
        std::string exe_path = util::path_util::getExeDirectory();
        
        std::string stream_file_name;
        {
            // uri : ex)rtsp://127.0.0.1/mjpeg_test.avi/
            std::string uri(request_uri);

            // truncate last '/'
            uri.erase(uri.end()-1);

            bool split_file_name_ret = util::splitFileName(uri, stream_file_name);
            BOOST_VERIFY(split_file_name_ret == true);
        }

        std::string stream_file_path = exe_path + "\\" + stream_file_name;

        frame_loader.reset(new payload::FrameLoader(stream_file_path));
    }
    session_data->frame_loader = frame_loader;
    session_data->payload_type = frame_loader->getPayloadType();

    PacketCreatorPtr packet_creator;
    {
        packet_creator.reset(new PacketCreator);
        packet_creator->setFrameRate(payload::DEFAULT_FRAME_RATE);
    }
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
                                               SessionDataPtrVec& session_data_vec)
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

void StreamingService::sendSingleFrame(payload::FrameParserPtr frame_parser,
                                       const SessionDataPtr& session_data)
{
    using namespace payload::h264;
    using namespace payload::jpeg;

    if (session_data->payload_type == payload::H264_PAYLOAD_TYPE)
    {
        H264FrameParserPtr h264_frame_parser = 
            boost::dynamic_pointer_cast<H264FrameParser>(frame_parser);
        BOOST_ASSERT(h264_frame_parser != NULL);

        sendH264SingleFrame(h264_frame_parser, session_data);
    }
    else if (session_data->payload_type == payload::MJPEG_PAYLOAD_TYPE)
    {
        JpegFrameParserPtr jpeg_frame_parser = 
            boost::dynamic_pointer_cast<JpegFrameParser>(frame_parser);

        BOOST_ASSERT(jpeg_frame_parser != NULL);

        sendJpegSingleFrame(jpeg_frame_parser, session_data);
    }
    else
    {
        BOOST_ASSERT(false);
    }
}

void StreamingService::sendJpegSingleFrame(
                                payload::jpeg::JpegFrameParserPtr frame_parser,
                                const SessionDataPtr& session_data)
{
    using namespace payload::jpeg;

    std::size_t fragment_size = 0;
    std::size_t fragment_offset = 0;

    unsigned char* scan_data = frame_parser->getScanData();
    std::size_t scan_data_size = frame_parser->getScanDataSize();

    while (fragment_offset < scan_data_size)
    {
        bool first_fragment = false;
        bool last_fragment = false;

        const unsigned char* fragment_buf = fragmentScanData(scan_data,
                                                             scan_data_size,
                                                             fragment_offset,
                                                             fragment_size,
                                                             first_fragment,
                                                             last_fragment);

        unsigned char* payload_buf = NULL;
        std::size_t payload_buf_size = 0;
        {
            JpegPayload jpeg_payload;
            jpeg_payload.makePayload(frame_parser,
                                     fragment_buf, fragment_size,
                                     fragment_offset);
            payload_buf = jpeg_payload.marshall(payload_buf_size);
        }

        RtpPacket rtp_packet;
        session_data->packet_creator->makePacket(first_fragment,
                                                 last_fragment,
                                                 Header::JPEG_PAYLOAD,
                                                 payload_buf,
                                                 payload_buf_size,
                                                 rtp_packet);

        sendRtpPacket(session_data->session, rtp_packet);

        util::safe_delete_array(rtp_packet.rtp_data);
        util::safe_delete_array(rtp_packet.rtp_header);

        fragment_offset += payload::FRAGMENTAION_SIZE;
    }
}

void StreamingService::sendH264SingleFrame(
                                payload::h264::H264FrameParserPtr frame_parser,
                                const SessionDataPtr& session_data)
{
    using namespace payload::h264;

    H264FrameParser::NalUnitVec nal_unit_vec = frame_parser->getNalUnitVec();
    BOOST_FOREACH(H264FrameParser::NalUnit nal_unit, nal_unit_vec)
    {
        unsigned char* nal_data = nal_unit.data;
        std::size_t nal_data_size = nal_unit.data_size;

        bool fragmentation_mode = false;
        if (nal_data_size > payload::FRAGMENTAION_SIZE)
        {
            fragmentation_mode = true;
        }

        std::size_t fragment_size = 0;
        std::size_t fragment_offset = 0;

        while (fragment_offset < nal_data_size)
        {
            bool first_fragment = false;
            bool last_fragment = false;

            const unsigned char* fragment_buf = fragmentScanData(nal_data,
                                                                 nal_data_size,
                                                                 fragment_offset,
                                                                 fragment_size,
                                                                 first_fragment,
                                                                 last_fragment);
            unsigned char* payload_buf = NULL;
            std::size_t payload_buf_size = 0;
            {
                H264Payload h264_payload;
                h264_payload.makePayload(nal_unit.header,
                                         first_fragment, last_fragment,
                                         fragment_buf, fragment_size,
                                         fragmentation_mode);
                payload_buf = h264_payload.marshall(payload_buf_size);
            }

            RtpPacket rtp_packet;
            session_data->packet_creator->makePacket(first_fragment,
                                                     last_fragment,
                                                     Header::H264_PAYLOAD,
                                                     payload_buf,
                                                     payload_buf_size,
                                                     rtp_packet);

            sendRtpPacket(session_data->session, rtp_packet);

            util::safe_delete_array(rtp_packet.rtp_data);
            util::safe_delete_array(rtp_packet.rtp_header);

            fragment_offset += payload::FRAGMENTAION_SIZE;
        }
    }
}

void StreamingService::thread_func()
{
    try
    {
        for(;;)
        {
            boost::this_thread::interruption_point();

            // wait & get activate session
            SessionDataPtrVec session_data_vec;
            {
                waitIfActivatedSessionIsEmpty();

                getNextPresentationTimeSessionData(now(), session_data_vec);
            }

            BOOST_FOREACH(SessionDataPtr session_data, session_data_vec)
            {
                // get frame data - begin
                unsigned char* frame_buf = NULL;
                std::size_t frame_buf_size = 0;
                {
                    const payload::FrameLoaderPtr& frame_loader = session_data->frame_loader;
                    bool get_frame_ret = frame_loader->getNextFrame(frame_buf,
                                                                    frame_buf_size);
                    if (! get_frame_ret)
                    {
                        continue;
                    }
                }
                // get frame data - end

                // parse frame - begin
                payload::FrameParserPtr frame_parser;
                {
                    frame_parser = payload::createFrameParser(session_data->payload_type);
                    if (! frame_parser)
                    {
                        continue;
                    }

                    bool parse_ret = frame_parser->parse(frame_buf,
                                                         frame_buf_size);
                    BOOST_VERIFY(parse_ret == true);
                    util::safe_delete_array(frame_buf);
                }
                // parse frame - end

                sendSingleFrame(frame_parser, session_data);
            }

            // sleep
            {
                time_point shortest_next_presentation_time_from_now = time_point::max();
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

} // namespace rtp
} // namespace rtsp_server
