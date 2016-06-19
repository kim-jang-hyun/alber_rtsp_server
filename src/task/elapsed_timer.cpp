#include "elapsed_timer.h"
#include <boost/date_time.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG


namespace rtsp_server {
namespace task {

ElapsedTimer::ElapsedTimer()
    :m_start(new boost::posix_time::ptime)
{
    reset();
}

ElapsedTimer::~ElapsedTimer()
{
    delete m_start;
}

void ElapsedTimer::reset()
{
    *m_start = boost::posix_time::microsec_clock::universal_time();
}

double ElapsedTimer::elapsed() const
{
    boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::universal_time() - *m_start;
    return (double)diff.total_milliseconds();
}

} // namespace task
} // namespace rtsp_server
