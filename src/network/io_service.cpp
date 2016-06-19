#include "io_service.h"

#include <vector>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace network {

IoService::IoService() : m_signal_set(m_io_service)
{
    m_signal_set.add(SIGINT);
    m_signal_set.add(SIGTERM);

#if defined(SIGQUIT)
    m_signal_set.add(SIGQUIT);
#endif // defined(SIGQUIT)

    m_signal_set.async_wait(boost::bind(&IoService::handleStop, this));
}

void IoService::run(std::size_t thread_pool_size)
{
    typedef boost::shared_ptr<boost::thread> ThreadPtr;

    // create thread and run
    std::vector<ThreadPtr> thread_vec;
    for (std::size_t i = 0; i < thread_pool_size; ++i)
    {
        ThreadPtr thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &m_io_service)));

        thread_vec.push_back(thread);
    }

    // wait for all threads to exit
    for (std::size_t i = 0; i < thread_vec.size(); ++i)
    {
        thread_vec[i]->join();
    }
}

asio::io_service& IoService::getIoService()
{
    return m_io_service;
}

void IoService::handleStop()
{
    m_io_service.stop();
}

} // namespace network
} // namespace rtsp_server
