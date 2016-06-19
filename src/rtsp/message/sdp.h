#ifndef __RTSP_SERVER_RTSP_MESSAGE_SDP_H__
#define __RTSP_SERVER_RTSP_MESSAGE_SDP_H__

#include <string>
#include <sstream>

namespace rtsp_server {
namespace rtsp {
namespace message {

const std::wstring W_SDP_MEDIA_TYPE = L"application/sdp";

class Sdp
{
public:
    explicit Sdp(const std::string& local_address);

    std::string getContent() const;
    int getContentLength() const;

private:
    void init(const std::string& local_address);

private:
    std::string m_protocol_version;
    struct
    {
        std::string user_name;
        std::string session_id;
        std::string session_version;
        std::string nettype;
        std::string addrtype;
        std::string unicast_address;
    } m_origin;
    std::string m_session_name;
    struct
    {
        std::string nettype;
        std::string addrtype;
        std::string connection_address;
    } m_connection_data;
    struct
    {
        std::string start_time;
        std::string stop_time;
    } m_timing;
    struct
    {
        std::string media;
        std::string port;
        std::string proto;
        std::string fmt;
    } m_media_description;

    std::stringstream sdp_stream;
};

} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_SDP_H__
