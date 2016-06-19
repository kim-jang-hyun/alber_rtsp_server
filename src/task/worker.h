#ifndef __RTSP_SERVER_WORKER_H__
#define __RTSP_SERVER_WORKER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "task.h"

namespace rtsp_server {
namespace task {

namespace asio = boost::asio;
using boost::uuids::uuid;

class Worker
{
    // type declare - begin
    typedef boost::shared_ptr<asio::io_service::work> WorkPtr;
    typedef std::map<uuid, TaskPtr> TaskPtrMap;

    class StrandImpl;
    typedef boost::shared_ptr<StrandImpl> StrandImplPtr;

    class TimerImpl;
    typedef boost::shared_ptr<TimerImpl> TimerImplPtr;
    // declare - end

public:
    Worker();
    ~Worker();

    void start();
    void start(int thread_count);
    void stop();

    bool isStarted() { return m_is_started; }
    asio::io_service& getService() { return m_io_service; }

public:
    bool postTask(const TaskPtr& task);
    bool postTask(const TaskPtr& task, const unsigned long expiry_time_millisec);
    bool postStrandTask(const TaskPtr& task, const uuid& strand_id);

    bool cancelTask(const uuid& task_id);

    bool addStrand(const uuid& strand_id);
    bool removeStrand(const uuid& strand_id);

private:
    void execute(const TaskPtr& task);

private:
    asio::io_service m_io_service;
    WorkPtr m_work;
    
    boost::thread_group m_thread_pool;

    StrandImplPtr m_strand_impl;
    TimerImplPtr m_timer_impl;

    TaskPtrMap m_task_map;

    bool m_is_started;
};

typedef boost::shared_ptr<Worker> WorkerPtr;

} // namespace task
} // namespace rtsp_server

#endif // __RTSP_SERVER_WORKER_H__
