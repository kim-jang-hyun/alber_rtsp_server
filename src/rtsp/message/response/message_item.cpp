#include "rtsp/message/response/message_item.h"

#include <assert.h>
#include "rtsp/message/entity_header.h"
#include "rtsp/message/general_header.h"
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


void MessageItem::setVersion(const std::wstring& version)
{
    status_line.version = version;
}

void MessageItem::setStatus(int status_code)
{
    std::wstring status_code_str, reason_phrase;
    if (! getStatusCodeFromTable(status_code, status_code_str))
    {
        assert(false);
    }
    if (! getReasonPhraseFromTable(status_code, reason_phrase))
    {
        assert(false);
    }

    status_line.status_code = status_code_str;
    status_line.reason_phrase = reason_phrase;
}

void MessageItem::addHeader(const std::wstring& name, const std::wstring& value)
{
    if (isGeneralHeader(name))
    {
        general_header_map[name] = value;
    }
    else if (isResponseHeader(name))
    {
        response_header_map[name] = value;
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

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
