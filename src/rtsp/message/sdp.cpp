#include "rtsp/message/sdp.h"

#include "util/time_util.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

using namespace rtsp_server;

namespace {

std::string getSessionVersion()
{
    static boost::uint64_t generated_only_once = UINT64_C(0);

    if (generated_only_once == UINT64_C(0))
    {
        util::TimeUtil time;
        generated_only_once = time.getCurrentTimeByNtpTimeStamp();
    }

    return util::toString<std::string>(generated_only_once);
}

} // namespace

namespace rtsp_server {
namespace rtsp {
namespace message {

Sdp::Sdp(const std::string& local_address)
{
    init(local_address);
}

std::string Sdp::getContent() const
{
    return sdp_stream.str();
}

int Sdp::getContentLength() const
{
    return sdp_stream.str().length();
}

void Sdp::init(const std::string& local_address)
{
    util::TimeUtil time;
    boost::uint64_t now_ntp_time_stamp = time.getCurrentTimeByNtpTimeStamp();

    {
        m_protocol_version = "0";
        sdp_stream << "v=" << m_protocol_version << "\r\n";
    }
    {
        m_origin.user_name = "-";
        m_origin.session_id = util::toString<std::string>(now_ntp_time_stamp);
        m_origin.session_version = getSessionVersion();
        m_origin.nettype = "IN";
        m_origin.addrtype = "IP4";
        m_origin.unicast_address = local_address;
        sdp_stream << "o=" << m_origin.user_name << " "
                           << m_origin.session_id << " "
                           << m_origin.session_version << " "
                           << m_origin.nettype << " "
                           << m_origin.addrtype << " "
                           << m_origin.unicast_address << "\r\n";
    }
    {
        m_session_name = " ";
        sdp_stream << "s=" << m_session_name << "\r\n";
    }
    {
        m_connection_data.nettype = "IN";
        m_connection_data.addrtype = "IP4";
        m_connection_data.connection_address = "0.0.0.0";
        sdp_stream << "c=" << m_connection_data.nettype << " "
                           << m_connection_data.addrtype << " "
                           << m_connection_data.connection_address << "\r\n";
    }
    {
        m_timing.start_time = "0";
        m_timing.stop_time = "0";
        sdp_stream << "t=" << m_timing.start_time << " "
                           << m_timing.stop_time << "\r\n";
    }
    {
        m_media_description.media = "video";
        m_media_description.port = "0";
        m_media_description.proto = "RTP/AVP";
        m_media_description.fmt = "26";
        sdp_stream << "m=" << m_media_description.media << " "
                           << m_media_description.port << " "
                           << m_media_description.proto << " "
                           << m_media_description.fmt << "\r\n";
    }
}

} // namespace message
} // namespace rtsp
} // namespace rtsp_server
