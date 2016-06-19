#ifndef __RTSP_SERVER_RTSP_MESSAGE_REQUEST_HEADER_H__
#define __RTSP_SERVER_RTSP_MESSAGE_REQUEST_HEADER_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

const std::wstring W_ACCEPT = L"Accept";
const std::wstring W_ACCEPT_ENCODING = L"Accept-Encoding";
const std::wstring W_ACCEPT_LANGUAGE = L"Accept-Language";
const std::wstring W_AUTHORIZATION = L"Authorization";
const std::wstring W_FROM = L"From";
const std::wstring W_IF_MODIFIED_SINCE = L"If-Modified-Since";
const std::wstring W_RANGE = L"Range";
const std::wstring W_REFERER = L"Referer";
const std::wstring W_SESSION = L"Session";
const std::wstring W_TRANSPORT = L"Transport";
const std::wstring W_USER_AGENT = L"User-Agent";

bool isRequestHeader(const std::wstring header_name);

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_REQUEST_HEADER_H__
