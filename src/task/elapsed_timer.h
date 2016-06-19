#ifndef __RTSP_SERVER_ELAPSED_TIMER_H__
#define __RTSP_SERVER_ELAPSED_TIMER_H__

namespace boost
{
namespace posix_time
{
class ptime;
}
}

namespace rtsp_server {
namespace task {

class ElapsedTimer
{
public:
    ElapsedTimer();
    ~ElapsedTimer();

    void reset();
    double elapsed() const;

private:
    boost::posix_time::ptime* m_start;
};

} // namespace task
} // namespace rtsp_server

#endif // __RTSP_SERVER_ELAPSED_TIMER_H__
