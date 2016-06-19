#ifndef __RTSP_SERVER_TASK_H__
#define __RTSP_SERVER_TASK_H__

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "util/unique_id.h"

namespace rtsp_server {
namespace task {

using util::unique_id;

class Task
{
public:
    Task();
    virtual ~Task();

    virtual void execute() = 0;

    void cancel();
    bool isCanceled();

    unique_id getId();

private:
    bool m_cancel_task;
    boost::mutex m_cancel_task_mutex;

    unique_id m_task_id;
};
typedef boost::shared_ptr<Task> TaskPtr;

} // namespace task
} // namespace rtsp_server

#endif // __RTSP_SERVER_TASK_H__
