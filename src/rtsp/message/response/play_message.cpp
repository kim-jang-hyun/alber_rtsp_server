#include "rtsp/message/response/play_message.h"

#include "rtsp/message/general_header.h"
#include "rtsp/message/request/request_header.h"
#include "util/time_util.h"
#include "util/utf8_converter.h"

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

PlayMessage::PlayMessage(const util::unique_id& session_id)
    : Message(MessageInterface::RESPONSE_METHOD_PLAY),
      m_session_id(session_id)
{

}

PlayMessage::~PlayMessage()
{

}

bool PlayMessage::interprete(const request::MessageItem& request_msg_item)
{
    if (! Message::interprete(request_msg_item))
    {
        return false;
    }

    std::wstring session_value;
    if (request_msg_item.getHeaderValue(request::W_SESSION, session_value))
    {
        if (session_value != util::to_wstring(m_session_id))
        {
            // should send a 454 (Session Not Found) response.
            BOOST_ASSERT(false);
            return false;
        }
    }

    addDateHeader();
    // range

    return true;
}

void PlayMessage::addDateHeader()
{
    util::TimeUtil time;
    std::string date_header_value = time.getCurrentTimeByHttpDateFormat();
    std::wstring date_header_value_utf16 = util::toUtf16(date_header_value);

    m_response_msg_item.addHeader(W_DATE, date_header_value_utf16);
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
