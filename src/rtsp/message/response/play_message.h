#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_PLAY_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_PLAY_MESSAGE_H__

#include "rtsp/message/response/message.h"

#include <string>
#include "rtsp/message/request/message_item.h"
#include "util/unique_id.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

class PlayMessage : public Message
{
public:
    explicit PlayMessage(const util::unique_id& session_id);
    virtual ~PlayMessage();

    // derived from Message - begin
    virtual bool interprete(const request::MessageItem& request_msg_item);
    // derived from Message - end

private:
    void addDateHeader();

private:
    util::unique_id m_session_id;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_PLAY_MESSAGE_H__
