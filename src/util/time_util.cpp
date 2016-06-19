#include "time_util.h"

#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>

// #ifdef _DEBUG
// #ifndef DBG_NEW
// #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// #define new DBG_NEW
// #endif
// #endif  // _DEBUG

namespace {

const std::string GMT = "GMT";

} // namespace

namespace rtsp_server {
namespace util {

using namespace boost::posix_time;
using namespace boost::gregorian;

TimeUtil::TimeUtil()
{
    m_now = second_clock::universal_time();
}

long TimeUtil::years() const
{
    return m_now.date().year();
}

long TimeUtil::months() const
{
    return m_now.date().month();
}

long TimeUtil::days() const
{
    return m_now.date().day();
}

long TimeUtil::hours() const
{
    return m_now.time_of_day().hours();
}

long TimeUtil::minutes() const
{
    return m_now.time_of_day().minutes();
}

long TimeUtil::seconds() const
{
    return m_now.time_of_day().seconds();
}

boost::uint64_t TimeUtil::getCurrentTimeByNtpTimeStamp() const
{
    ptime now_microsec = microsec_clock::universal_time();
    ptime ntp_epoch(date(1900, boost::gregorian::Jan, 1));

    time_duration diff = now_microsec - ntp_epoch;

    boost::uint32_t elapsed_seconds = diff.total_seconds();
    boost::uint32_t fractional_in_elapsed_seconds =
        (boost::uint32_t)diff.fractional_seconds();

    boost::uint32_t ntp_frational =
        (boost::uint32_t)(fractional_in_elapsed_seconds *
                          pow(2.0, 32.0) / pow(10.0, 6.0));

    boost::uint64_t ntp_timestamp = 0;
    ntp_timestamp |= elapsed_seconds;
    ntp_timestamp <<= 32;
    ntp_timestamp |= ntp_frational;

    return ntp_timestamp;
}

std::string TimeUtil::getCurrentTimeByHttpDateFormat() const
{
    std::stringstream time_stream;
    time_stream.imbue(std::locale(std::locale::classic(),
                                  new time_facet("%a, %d %b %Y %H:%M:%S")));

    ptime now_sec = second_clock::universal_time();
    time_stream << now_sec << " " << GMT;

    return time_stream.str();
}

} // namespace util
} // namespace rtsp_server
