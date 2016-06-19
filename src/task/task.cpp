#include "task.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace task {

Task::Task() : m_cancel_task(false), m_task_id(util::generateId())
{

}

Task::~Task()
{

}

void Task::cancel()
{
    boost::unique_lock<boost::mutex> lock(m_cancel_task_mutex);
    m_cancel_task = true;
}

bool Task::isCanceled()
{
    boost::unique_lock<boost::mutex> lock(m_cancel_task_mutex);
    return m_cancel_task;
}

unique_id Task::getId()
{
    return m_task_id;
}

} // namespace task
} // namespace rtsp_server
