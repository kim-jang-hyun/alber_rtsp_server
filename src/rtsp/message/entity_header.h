#ifndef __RTSP_SERVER_RTSP_MESSAGE_ENTITY_HEADER_H__
#define __RTSP_SERVER_RTSP_MESSAGE_ENTITY_HEADER_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {

const std::wstring W_ALLOW = L"Allow";
const std::wstring W_CONTENT_BASE = L"Content-Base";
const std::wstring W_CONTENT_ENCODING = L"Content-Encoding";
const std::wstring W_CONTENT_LANGUAGE = L"Content-Language";
const std::wstring W_CONTENT_LENGTH = L"Content-Length";
const std::wstring W_CONTENT_LOCATION = L"Content-Location";
const std::wstring W_CONTENT_TYPE = L"Content-Type";
const std::wstring W_EXPIRES = L"Expires";
const std::wstring W_LAST_MODIFIED = L"Last-Modified";

bool isEntityHeader(const std::wstring header_name);

} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_ENTITY_HEADER_H__
