#include "task/request_method_describe_task.h"

#include "rtsp/message/response/describe_message.h"
#include "util/utf8_converter.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace task {

RequestMethodDescribeTask::RequestMethodDescribeTask(
    const rtsp::message::request::MessageItem& msg_item,
    ParticipantPtr participant)
    : Task(),
    m_request_message_item(msg_item),
    m_participant(participant)
{

}

void RequestMethodDescribeTask::execute()
{
    rtsp::SessionPtr rtsp_session = m_participant->getRtspSession();

    std::string local_ip = rtsp_session->getLocalAddress();
    rtsp::message::response::DescribeMessage response_describe_msg(local_ip);

    bool valid_msg = response_describe_msg.interprete(m_request_message_item);
    if (! valid_msg)
    {
        std::cout << "invalid describe message" << std::endl;
    }

    std::wstring serialized_msg_utf16 = response_describe_msg.serialize();

    std::string serialized_msg_utf8;
    bool convert_ret = util::convertUtf16ToUtf8(serialized_msg_utf16,
                                                serialized_msg_utf8);
    if (! convert_ret)
    {
        return;
    }

    rtsp_session->sendRtspPacket(((unsigned char*)serialized_msg_utf8.c_str()),
                                  serialized_msg_utf8.size());
}

} // namespace task
} // namespace rtsp_server
