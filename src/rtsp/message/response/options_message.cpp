#include "rtsp/message/response/options_message.h"
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

OptionsMessage::OptionsMessage()
    : Message(MessageInterface::RESPONSE_METHOHD_OPTIONS)
{

}

OptionsMessage::~OptionsMessage()
{

}

bool OptionsMessage::interprete(const request::MessageItem& request_msg_item)
{
    if (! Message::interprete(request_msg_item))
    {
        return false;
    }

    addPublicHeader();

    return true;
}

void OptionsMessage::addPublicHeader()
{
    m_response_msg_item.addHeader(W_PUBLIC, L"DESCRIBE, SETUP, TEARDOWN, PLAY");
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
