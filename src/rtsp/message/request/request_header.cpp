#include "rtsp/message/request/request_header.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

bool isRequestHeader(const std::wstring header_name)
{
    if (header_name == W_ACCEPT ||
        header_name == W_ACCEPT_ENCODING ||
        header_name == W_ACCEPT_LANGUAGE ||
        header_name == W_AUTHORIZATION ||
        header_name == W_FROM ||
        header_name == W_IF_MODIFIED_SINCE ||
        header_name == W_RANGE ||
        header_name == W_REFERER ||
        header_name == W_USER_AGENT)
    {
        return true;
    }
    return false;
}

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
