#ifndef __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H__
#define __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H__

#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace sdp {

const std::wstring W_SDP_MEDIA_TYPE = L"application/sdp";

class Sdp
{
public:
    Sdp();
    virtual ~Sdp();

    void generate(const std::string& local_address);

    std::string getContent() const;
    int getContentLength() const;

private:
    virtual void generateImpl() = 0;

protected:
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

    std::stringstream m_sdp_stream;
};
typedef boost::shared_ptr<Sdp> SdpPtr;

SdpPtr createSdp(std::wstring request_uri);

} // namespace sdp
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_SDP_SDP_H__
