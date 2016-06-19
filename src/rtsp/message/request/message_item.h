#ifndef __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_ITEM_H__
#define __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_ITEM_H__

#include <map>
#include "rtsp/message/request/request_line.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

struct MessageItem
{
    typedef std::map<std::wstring/*name*/,
                     std::wstring/*value*/> MessageHeaderMap;

    void setRequestLine(const std::wstring& method, const std::wstring& uri,
                        const std::wstring& version);
    void addHeader(const std::wstring& name, const std::wstring& value);

    bool getHeaderValue(const std::wstring& name, std::wstring& value) const;

    RequestLine request_line;
    MessageHeaderMap general_header_map;
    MessageHeaderMap request_header_map;
    MessageHeaderMap entity_header_map;
};

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_ITEM_H__
