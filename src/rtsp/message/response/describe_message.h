#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_DESCRIBE_MESSAGE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_DESCRIBE_MESSAGE_H__

#include "rtsp/message/response/message.h"

#include <string>
#include "rtsp/message/request/message_item.h"
#include "rtsp/message/sdp.h"

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

class DescribeMessage : public Message
{
public:
    explicit DescribeMessage(const std::string& local_ip);
    virtual ~DescribeMessage();

    // derived from Message - begin
    virtual bool interprete(const request::MessageItem& request_msg_item);
    // derived from Message - end

private:
    int errorCheck(const request::MessageItem& request_msg_item);

    void addDateHeader();
    void addContentType();
    void addContentLength();

    void addSdp();

private:
    Sdp m_sdp;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_DESCRIBE_MESSAGE_H__
