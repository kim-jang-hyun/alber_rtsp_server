#ifndef __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_H__

#include "rtsp/message/message_interface.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include "rtsp/message/request/message_item.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

class Message : public MessageInterface
{
public:
    Message(MessageInterface::Type type, const MessageItem& msg_item)
        : MessageInterface(type), m_msg_item(msg_item) {}
    virtual ~Message() {}


    MessageItem getMsgItem() const { return m_msg_item; }

private:
    MessageItem m_msg_item;
};
typedef boost::shared_ptr<Message> MessagePtr;

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_H__
