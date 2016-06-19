#ifndef __RTSP_UTIL_TIME_UTIL_H__
#define __RTSP_UTIL_TIME_UTIL_H__

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace rtsp_server {
namespace util {

class TimeUtil
{
public:
    TimeUtil();

    long years() const;
    long months() const;
    long days() const;
    long hours() const;
    long minutes() const;
    long seconds() const;

    boost::uint64_t getCurrentTimeByNtpTimeStamp() const;
    std::string getCurrentTimeByHttpDateFormat() const;

private:
    boost::posix_time::ptime m_now;
};

} // namespace util
} // namespace rtsp_server

#endif // __RTSP_UTIL_TIME_UTIL_H__
