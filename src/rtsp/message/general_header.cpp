#include "rtsp/message/general_header.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {

bool isGeneralHeader(const std::wstring header_name)
{
    if (header_name == W_CSEQ ||
        header_name == W_CACHE_CONTROL ||
        header_name == W_CONNECTION ||
        header_name == W_DATE ||
        header_name == W_VIA)
    {
        return true;
    }
    return false;
}

} // namespace message
} // namespace rtsp
} // namespace rtsp_server
