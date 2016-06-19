#ifndef __RTSP_UTIL_UTF8_CONVERTER_H__
#define __RTSP_UTIL_UTF8_CONVERTER_H__

#include <string>

namespace rtsp_server {
namespace util {

std::string toUtf8(const std::string& str);
bool fromUtf8(const std::string& utf8_str, std::string& cur_locale_str);

std::wstring toUtf16(const std::string& str);

bool convertUtf8ToUtf16(const std::string& utf8_str, std::wstring& utf16_str);
bool convertUtf16ToUtf8(const std::wstring& utf16_str, std::string& utf8_str);

int isUtf8(unsigned char* str, std::size_t len, std::string error_msg);

} // namespace util
} // namespace rtsp_server

#endif // __RTSP_UTIL_UTF8_CONVERTER_H__
