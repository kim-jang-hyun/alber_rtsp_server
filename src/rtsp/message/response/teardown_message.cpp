#include "rtsp/message/response/teardown_message.h"
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

TeardownMessage::TeardownMessage()
    : Message(MessageInterface::RESPONSE_METHOD_TEARDOWN)
{

}

TeardownMessage::~TeardownMessage()
{

}

bool TeardownMessage::interprete(const request::MessageItem& request_msg_item)
{
    if (! Message::interprete(request_msg_item))
    {
        return false;
    }

    return true;
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
