#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_H__

#include "rtsp/message/message_interface.h"

#include <string>
#include "rtsp/message/request/message_item.h"
#include "rtsp/message/response/message_item.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

class Message : public MessageInterface
{
public:
    explicit Message(MessageInterface::Type type) : MessageInterface(type) {}
    virtual ~Message() {}

    virtual bool interprete(const request::MessageItem& request_msg_item);
    virtual std::wstring serialize();

protected:
    MessageItem m_response_msg_item;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_MESSAGE_H__
