#include "rtsp/message/entity_header.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {

bool isEntityHeader(const std::wstring header_name)
{
    if (header_name == W_ALLOW ||
        header_name == W_CONTENT_BASE ||
        header_name == W_CONTENT_ENCODING ||
        header_name == W_CONTENT_LANGUAGE ||
        header_name == W_CONTENT_LENGTH ||
        header_name == W_CONTENT_LOCATION ||
        header_name == W_CONTENT_TYPE ||
        header_name == W_EXPIRES ||
        header_name == W_LAST_MODIFIED)
    {
        return true;
    }
    return false;
}

} // namespace message
} // namespace rtsp
} // namespace rtsp_server
