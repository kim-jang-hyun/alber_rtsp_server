#ifndef __RTSP_SERVER_UTIL_UNIQUE_ID_H__
#define __RTSP_SERVER_UTIL_UNIQUE_ID_H__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace rtsp_server {
namespace util {

typedef boost::uuids::uuid unique_id;

inline unique_id generateId()
{
    return boost::uuids::random_generator()();
}

inline unique_id noneId()
{
    return boost::uuids::nil_uuid();
}

inline std::string to_string(const unique_id& id)
{
    return boost::uuids::to_string(id);
}

inline std::wstring to_wstring(const unique_id& id)
{
    return boost::uuids::to_wstring(id);
}

} // namespace util
} // namespace rtsp_server

#endif // __RTSP_SERVER_UTIL_UNIQUE_ID_H__
