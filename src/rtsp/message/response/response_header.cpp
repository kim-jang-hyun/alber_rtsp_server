#include "rtsp/message/response/response_header.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

bool isResponseHeader(const std::wstring header_name)
{
    if (header_name == W_LOCATION ||
        header_name == W_PROXY_AUTHENTICATE ||
        header_name == W_PUBLIC ||
        header_name == W_RETRY_AFTER ||
        header_name == W_SERVER ||
        header_name == W_VARY ||
        header_name == W_WWW_AUTHENTICATE)
    {
        return true;
    }
    return false;
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
