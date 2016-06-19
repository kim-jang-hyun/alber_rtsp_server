#include "task/request_method_play_task.h"

#include "rtsp/message/response/play_message.h"
#include "util/utf8_converter.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace task {

RequestMethodPlayTask::RequestMethodPlayTask(
    const rtsp::message::request::MessageItem& msg_item,
    ParticipantPtr participant)
    : Task(),
    m_request_message_item(msg_item),
    m_participant(participant)
{

}

void RequestMethodPlayTask::execute()
{
    util::unique_id session_id = m_participant->getRtspSessionId();
    rtsp::message::response::PlayMessage response_play_msg(session_id);

    bool valid_msg = response_play_msg.interprete(m_request_message_item);
    if (! valid_msg)
    {
        return;
    }

    std::string serialized_msg_utf8;
    std::wstring serialized_msg_utf16 = response_play_msg.serialize();

    bool convert_ret = util::convertUtf16ToUtf8(serialized_msg_utf16,
                                                serialized_msg_utf8);
    if (! convert_ret)
    {
        return;
    }

    rtsp::SessionPtr rtsp_session = m_participant->getRtspSession();
    rtsp_session->sendRtspPacket(((unsigned char*)serialized_msg_utf8.c_str()),
                                  serialized_msg_utf8.size());

    m_participant->playStreaming();
}

} // namespace task
} // namespace rtsp_server
