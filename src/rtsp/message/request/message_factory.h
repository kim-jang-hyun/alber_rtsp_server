#ifndef __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_FACTORY_H__
#define __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_FACTORY_H__

#include "rtsp/message/message_interface.h"
#include "rtsp/message/request/message_item.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

class MessageFactory
{
public:
    static MessageInterfacePtr createMsg(const MessageItem& msg_item);
};

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_REQUEST_MESSAGE_FACTORY_H__
