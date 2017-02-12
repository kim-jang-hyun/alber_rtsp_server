#include "rtsp/message/request/message_item.h"

#include "util/util.h"

#include "rtsp/message/entity_header.h"
#include "rtsp/message/general_header.h"
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

void MessageItem::setRequestLine(const std::wstring& method,
                                 const std::wstring& uri,
                                 const std::wstring& version)
{
    request_line.method = method;
    request_line.uri = uri;
    request_line.version = version;
}

void MessageItem::addHeader(const std::wstring& name,
                            const std::wstring& value)
{
    if (isGeneralHeader(name))
    {
        general_header_map[name] = value;
    }
    else if (isRequestHeader(name))
    {
        request_header_map[name] = value;
    }
    else if (isEntityHeader(name))
    {
        entity_header_map[name] = value;
    }
    else // extension header
    {
        entity_header_map[name] = value;
    }
}

bool MessageItem::getHeaderValue(const std::wstring& name,
                                 std::wstring& value) const
{
    if (isGeneralHeader(name))
    {
        return util::getMapValue(general_header_map, name, value);
    }
    else if (isRequestHeader(name))
    {
        return util::getMapValue(request_header_map, name, value);
    }
    else if (isEntityHeader(name))
    {
        return util::getMapValue(entity_header_map, name, value);
    }
    else // extension header
    {
        return util::getMapValue(entity_header_map, name, value);
    }
}

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
