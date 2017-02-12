#include "rtsp/message/sdp/sdp.h"

#include "rtp/payload/frame_loader.h"
#include "rtsp/message/sdp/sdp_h264.h"
#include "rtsp/message/sdp/sdp_mjpeg.h"
#include "rtp/payload/payload_common.h"
#include "util/path_util.h"
#include "util/time_util.h"
#include "util/utf8_converter.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

namespace sdp = rtsp_server::rtsp::message::sdp;
namespace payload = rtsp_server::rtp::payload;
namespace util = rtsp_server::util;

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

payload::PayloadType getPayloadType(std::string uri)
{
    payload::FrameLoaderPtr frame_loader;
    {
        std::string exe_path = util::path_util::getExeDirectory();

        std::string stream_file_name;
        bool split_file_name_ret = util::splitFileName(uri, stream_file_name);
        BOOST_VERIFY(split_file_name_ret == true);

        std::string stream_file_path = exe_path + "\\" + stream_file_name;

        frame_loader.reset(new payload::FrameLoader(stream_file_path));
    }
    return frame_loader->getPayloadType();
}

} // namespace

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace sdp {

Sdp::Sdp()
{

}

Sdp::~Sdp()
{

}

void Sdp::generate(const std::string& local_address)
{
    util::TimeUtil time;
    boost::uint64_t now_ntp_time_stamp = time.getCurrentTimeByNtpTimeStamp();

    {
        m_protocol_version = "0";
        m_sdp_stream << "v=" << m_protocol_version << "\r\n";
    }
    {
        m_origin.user_name = "-";
        m_origin.session_id = util::toString<std::string>(now_ntp_time_stamp);
        m_origin.session_version = getSessionVersion();
        m_origin.nettype = "IN";
        m_origin.addrtype = "IP4";
        m_origin.unicast_address = local_address;
        m_sdp_stream << "o=" << m_origin.user_name << " "
                             << m_origin.session_id << " "
                             << m_origin.session_version << " "
                             << m_origin.nettype << " "
                             << m_origin.addrtype << " "
                             << m_origin.unicast_address << "\r\n";
    }
    {
        m_session_name = " ";
        m_sdp_stream << "s=" << m_session_name << "\r\n";
    }
    {
        m_connection_data.nettype = "IN";
        m_connection_data.addrtype = "IP4";
        m_connection_data.connection_address = "0.0.0.0";
        m_sdp_stream << "c=" << m_connection_data.nettype << " "
                             << m_connection_data.addrtype << " "
                             << m_connection_data.connection_address << "\r\n";
    }
    {
        m_timing.start_time = "0";
        m_timing.stop_time = "0";
        m_sdp_stream << "t=" << m_timing.start_time << " "
                             << m_timing.stop_time << "\r\n";
    }

    generateImpl();
}

std::string Sdp::getContent() const
{
    return m_sdp_stream.str();
}

int Sdp::getContentLength() const
{
    return m_sdp_stream.str().length();
}

SdpPtr createSdp(std::wstring request_uri)
{
    SdpPtr sdp_instance;

    std::string request_uri_utf8;
    util::convertUtf16ToUtf8(request_uri, request_uri_utf8);

    payload::PayloadType request_payload_type = getPayloadType(request_uri_utf8);
    switch (request_payload_type)
    {
    case payload::MJPEG_PAYLOAD_TYPE: sdp_instance.reset(new sdp::SdpMjpeg); break;
    case payload::H264_PAYLOAD_TYPE:  sdp_instance.reset(new sdp::SdpH264);  break;
    default:
        {
            BOOST_ASSERT(false);
            return NULL_SHARED_PTR(SdpPtr);
        }
    }

    return sdp_instance;
}

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
