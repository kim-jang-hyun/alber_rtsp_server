#ifndef __RTSP_SERVER_TASK_REQUEST_METHOD_SETUP_TASK_H__
#define __RTSP_SERVER_TASK_REQUEST_METHOD_SETUP_TASK_H__

#include "task/task.h"

#include "participant.h"
#include "rtsp/message/request/message_item.h"

namespace rtsp_server {
namespace task {

class RequestMethodSetupTask : public Task
{
public:
    RequestMethodSetupTask(
        const rtsp::message::request::MessageItem& msg_item,
        ParticipantPtr participant);

    virtual void execute();

private:
    rtsp::message::request::MessageItem m_request_message_item;
    ParticipantPtr m_participant;
};

} // namespace task
} // namespace rtsp_server

#endif // __RTSP_SERVER_TASK_REQUEST_METHOD_SETUP_TASK_H__
