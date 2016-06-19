#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_TEARDOWN_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_TEARDOWN_MESSAGE_H__

#include "rtsp/message/response/message.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

class TeardownMessage : public Message
{
public:
    TeardownMessage();
    virtual ~TeardownMessage();

    // derived from Message - begin
    virtual bool interprete(const request::MessageItem& request_msg_item);
    // derived from Message - end
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_TEARDOWN_MESSAGE_H__
