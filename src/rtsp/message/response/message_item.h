#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_ITEM_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_ITEM_H__

#include <map>
#include "rtsp/message/response/status_line.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

struct MessageItem
{
    typedef std::map<std::wstring/*name*/,
                     std::wstring/*value*/> MessageHeaderMap;

    void setVersion(const std::wstring& version);
    void setStatus(int status_code);
    void addHeader(const std::wstring& name, const std::wstring& value);
    void addBody(const std::wstring& body) { message_body = body; }

    StatusLine status_line;
    MessageHeaderMap general_header_map;
    MessageHeaderMap response_header_map;
    MessageHeaderMap entity_header_map;

    std::wstring message_body;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_ITEM_H__
