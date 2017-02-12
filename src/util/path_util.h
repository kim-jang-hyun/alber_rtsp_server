#ifndef __RTSP_SERVER_UTIL_PATH_UTIL_H__
#define __RTSP_SERVER_UTIL_PATH_UTIL_H__

#include <string>

namespace rtsp_server {
namespace util {
namespace path_util {

std::string getExeFullPath();
std::string getExeFileName();
std::string getExeDirectory();

void changeCurrentDirectoryToExeDirectory();

} // namespace path_util
} // namespace util
} // namespace rtsp_server

#endif // __RTSP_SERVER_UTIL_PATH_UTIL_H__
