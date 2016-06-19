#include "rtsp/message/request/message_factory.h"

#include <string>
#include "rtsp/message/request/message.h"
#include "rtsp/message/request/request_line.h"
#include "util/util.h"

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

MessageInterfacePtr MessageFactory::createMsg(const MessageItem& msg_item)
{
    MessagePtr message;
    MessageInterface::Type message_type = MessageInterface::NONE;

    std::wstring method = msg_item.request_line.method;
    if (method == W_OPTIONS)
    {
        message_type = MessageInterface::REQUEST_METHOD_OPTIONS;
    }
    else if (method == W_DESCRIBE)
    {
        message_type = MessageInterface::REQUEST_METHOD_DESCRIBE;
    }
    else if (method == W_SETUP)
    {
        message_type = MessageInterface::REQUEST_METHOD_SETUP;
    }
    else if (method == W_PLAY)
    {
        message_type = MessageInterface::REQUEST_METHOD_PLAY;
    }
    else if (method == W_TEARDOWN)
    {
        message_type = MessageInterface::REQUEST_METHOD_TEARDOWN;
    }
    else
    {
        return NULL_SHARED_PTR(MessageInterfacePtr);
    }

    message.reset(new Message(message_type, msg_item));

    return boost::static_pointer_cast<MessageInterface>(message);
}

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
