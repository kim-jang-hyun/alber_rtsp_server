#include "worker.h"

#include <boost/date_Time/posix_time/posix_time.hpp>
#include <boost/noncopyable.hpp>

#include "elapsed_timer.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG


namespace rtsp_server {
namespace task {
//
// class Worker::StrandImpl
//
class Worker::StrandImpl : private boost::noncopyable
{
    typedef boost::shared_ptr<asio::strand> StrandPtr;
    typedef std::map<uuid, StrandPtr> StrandMap;

public:
    explicit StrandImpl(Worker* worker)
        : m_worker(worker), m_io_service(worker->getService()) {}
    ~StrandImpl() { m_strand_map.clear(); }

    bool add(const uuid& id)
    {
        if (! util::isExisted(m_strand_map, id))
        {
            return false;
        }

        StrandPtr strand(new asio::strand(m_io_service));
        m_strand_map[id] = strand;

        return true;
    }
    bool remove(const uuid& id)
    {
        const StrandMap::const_iterator it = m_strand_map.find(id);
        if (it == m_strand_map.end())
        {
            return false;
        }

        m_strand_map.erase(it);

        return true;
    }
    bool post(const TaskPtr& task, const uuid& id)
    {
        if (! util::isExisted(m_strand_map, id))
        {
            return false;
        }

        m_strand_map[id]->post(boost::bind(&Worker::execute, m_worker, task));

        return true;
    }

private:
    Worker* m_worker;
    asio::io_service& m_io_service;
    StrandMap m_strand_map;
};


//
// struct Worker::TimerImpl
//
class Worker::TimerImpl : private boost::noncopyable
{
    struct Timer
    {
        typedef boost::shared_ptr<asio::deadline_timer> DeadlineTimerPtr;

        DeadlineTimerPtr timerImpl;

        unsigned long trigger_time_millisec;
        ElapsedTimer check_elapsed_timer;  // http://think-async.com/Asio/boost_asio_1_5_3/doc/html/boost_asio/history.html
                                           // 위 링크의 Ticket #4745 확인.
                                           // Window OS에서 timer 시간이 5분이상 이면 안 됨.
                                           // 따라서, 4분을 넘는 DelayTask를 실행할 경우 4분주기로 시간을 체크해야함.
        const static int MaximumTimeMilliSec = 4 * 60 * 1000;
    };

    typedef boost::shared_ptr<Timer> TimerPtr; 
    typedef std::map<uuid, TimerPtr> TimerMap;

public:
    explicit TimerImpl(Worker* worker)
        : m_worker(worker), m_io_service(worker->getService()) {}
    ~TimerImpl() { m_timer_map.clear(); }

    void add(const uuid& id, const unsigned long expiry_time_millisec)
    {
        TimerPtr timer(new Timer);
        timer->trigger_time_millisec = expiry_time_millisec;
        timer->timerImpl.reset(new asio::deadline_timer(m_io_service));

        m_timer_map[id] = timer;
    }

    void post(const uuid& id, const TaskPtr& task)
    {
        typedef boost::posix_time::milliseconds milliseconds;

        TimerPtr timer = m_timer_map[id];

        bool exceedMaximumTime = false;
        if (timer->trigger_time_millisec > (double)Timer::MaximumTimeMilliSec)
        {
            exceedMaximumTime = true;
        }

        timer->timerImpl->expires_from_now(
            milliseconds(exceedMaximumTime ?
                        Timer::MaximumTimeMilliSec : timer->trigger_time_millisec));
        timer->timerImpl->async_wait(
            boost::bind(&TimerImpl::onTimer,
                        this,
                        boost::asio::placeholders::error,
                        task,
                        id));
        timer->check_elapsed_timer.reset();
    }

    bool cacnel(const uuid& id)
    {
        TimerMap::iterator it = m_timer_map.find(id);
        if (it == m_timer_map.end())
        {
            return false;
        }

        TimerPtr timer = it->second;

        boost::system::error_code ec;
        timer->timerImpl->cancel(ec);
        if (ec)
        {
            return false;
        }

        m_timer_map.erase(id);
        return true;
    }

private:
    void onTimer(const boost::system::error_code& e,
                 const TaskPtr& task,
                 const uuid& id)
    {
        if (e == boost::asio::error::operation_aborted)
        {
            return;
        }

        TimerPtr timer = m_timer_map[id];

        unsigned long elapsed_time_millisec = (unsigned long)timer->check_elapsed_timer.elapsed();
        unsigned long remain_time_millisec = timer->trigger_time_millisec - elapsed_time_millisec;

        if (remain_time_millisec > 0)
        {
            timer->timerImpl->expires_from_now(boost::posix_time::milliseconds(remain_time_millisec));
            timer->timerImpl->async_wait(
                boost::bind(&TimerImpl::onTimer,
                            this,
                            boost::asio::placeholders::error, task, id));
        }
        else
        {
            m_worker->execute(task);

            m_timer_map.erase(id);
        }
    }

private:
    Worker* m_worker;
    asio::io_service& m_io_service;
    TimerMap m_timer_map;
};

//
// class EdWorker
//
Worker::Worker() : m_is_started(false)
{

}

Worker::~Worker()
{
    stop();
}

void Worker::start()
{
    start(boost::thread::hardware_concurrency() + 1);
}

void Worker::start(int thread_count)
{
    if (! isStarted())
    {
        m_work.reset(new asio::io_service::work(m_io_service));
        m_strand_impl.reset(new StrandImpl(this));
        m_timer_impl.reset(new TimerImpl(this));

        for (int i = 0; i < thread_count; ++i)
        {
            m_thread_pool.create_thread(
                boost::bind(&boost::asio::io_service::run, &m_io_service));
        }

        m_is_started = true;
    }
}

void Worker::stop()
{
    if (isStarted())
    {
        m_work.reset();

        m_io_service.stop();
        m_thread_pool.join_all();

        m_io_service.reset();
        
        m_timer_impl.reset();
        m_strand_impl.reset();

        m_is_started = false;
    }
}

bool Worker::postTask(const TaskPtr& task)
{
    if (! isStarted())
    {
        return false;
    }

    uuid task_id = task->getId();
    m_task_map[task_id] = task;

    m_io_service.post(boost::bind(&Worker::execute, this, task));

    return true;
}

bool Worker::postTask(const TaskPtr& task,
                      const unsigned long expiry_time_millisec)
{
    if (!isStarted())
    {
        return false;
    }

    uuid task_id = task->getId();

    m_timer_impl->add(task_id, expiry_time_millisec);
    m_task_map[task_id] = task;

    m_timer_impl->post(task_id, task);

    return true;
}

bool Worker::cancelTask(const uuid& task_id)
{
    TaskPtrMap::iterator it = m_task_map.find(task_id);
    if (it == m_task_map.end())
    {
        return false;
    }

    TaskPtr task = it->second;
    task->cancel();
    m_task_map.erase(it);

    return m_timer_impl->cacnel(task_id);
}

bool Worker::addStrand(const uuid& strand_id)
{
    return m_strand_impl->add(strand_id);
}

bool Worker::removeStrand(const uuid& strand_id)
{
    return m_strand_impl->remove(strand_id);
}

bool Worker::postStrandTask(const TaskPtr& task, const uuid& strand_id)
{
    if (! isStarted())
    {
        return false;
    }

    bool post_ret = m_strand_impl->post(task, strand_id);
    if (! post_ret)
    {
        return false;
    }

    uuid task_id = task->getId();
    m_task_map[task_id] = task;

    return true;
}

void Worker::execute(const TaskPtr& task)
{
    uuid task_id = task->getId();
    if (! util::isExisted(m_task_map, task_id))
    {
        return;
    }

    m_task_map.erase(task_id);
    if (task->isCanceled())
    {
        return;
    }

    task->execute();
}

} // namespace task
} // namespace rtsp_server
