#include "rtsp/message/response/message.h"

#include <sstream>
#include <boost/foreach.hpp>

#include "rtsp/message/general_header.h"
#include "rtsp/message/response/status_line.h"
#include "rtsp/message/syntax.h"

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

bool Message::interprete(const request::MessageItem& request_msg_item)
{
    if (syntax::W_RTSP_VERSION != request_msg_item.request_line.version)
    {
        return false;
    }

    std::wstring cseq;
    if (! request_msg_item.getHeaderValue(W_CSEQ, cseq))
    {
        return false;
    }

    m_response_msg_item.setVersion(request_msg_item.request_line.version);
    m_response_msg_item.setStatus(200);

    m_response_msg_item.addHeader(W_CSEQ, cseq);

    return true;
}

std::wstring Message::serialize()
{
    using namespace syntax;
    typedef MessageItem::MessageHeaderMap MessageHeaderMap;

    std::wstringstream string_stream;
    string_stream << m_response_msg_item.status_line.version << W_SP
                  << m_response_msg_item.status_line.status_code << W_SP
                  << m_response_msg_item.status_line.reason_phrase << W_CRLF;

    BOOST_FOREACH(const MessageHeaderMap::value_type& header,
                  m_response_msg_item.general_header_map)
    {
        std::wstring name = header.first;
        std::wstring value = header.second;

        string_stream << name << W_COLON << W_SP << value << W_CRLF; 
    }
    BOOST_FOREACH(const MessageHeaderMap::value_type& header,
        m_response_msg_item.response_header_map)
    {
        std::wstring name = header.first;
        std::wstring value = header.second;

        string_stream << name << W_COLON << W_SP << value << W_CRLF; 
    }
    BOOST_FOREACH(const MessageHeaderMap::value_type& header,
        m_response_msg_item.entity_header_map)
    {
        std::wstring name = header.first;
        std::wstring value = header.second;

        string_stream << name << W_COLON << W_SP << value << W_CRLF; 
    }
    string_stream << W_CRLF;

    string_stream << m_response_msg_item.message_body;

    return string_stream.str();
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
