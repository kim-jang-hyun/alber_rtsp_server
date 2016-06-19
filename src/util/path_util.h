#ifndef __RTSP_SERVER_UTIL_PATH_UTIL_H__
#define __RTSP_SERVER_UTIL_PATH_UTIL_H__

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

namespace rtsp_server {
namespace util {
namespace path_util {

std::string getExeFullPath()
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);

    std::wstring path_w_str(path);
    std::string path_str;
    path_str.assign(path_w_str.begin(), path_w_str.end());

    return path_str;
}

std::string getExeFileName()
{	
    boost::filesystem::path path(getExeFullPath());
    std::string tmp = path.filename().string();
    return path.filename().string();
}

std::string getExeDirectory()
{	
    boost::filesystem::path path(getExeFullPath());
    std::string tmp = path.parent_path().string();
    return path.parent_path().string();
}

void changeCurrentDirectoryToExeDirectory()
{
    std::string path = getExeDirectory();

    std::wstring path_w_str;
    path_w_str.assign(path.begin(), path.end());

    SetCurrentDirectory(path_w_str.c_str());
}

} // namespace path_util
} // namespace util
} // namespace rtsp_server

#endif // __RTSP_SERVER_UTIL_PATH_UTIL_H__
