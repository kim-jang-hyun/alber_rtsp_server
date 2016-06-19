#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_HEADER_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_HEADER_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

const std::wstring W_LOCATION = L"Location";
const std::wstring W_PROXY_AUTHENTICATE = L"Proxy-Authenticate";
const std::wstring W_PUBLIC = L"Public";
const std::wstring W_RETRY_AFTER = L"Retry-After";
const std::wstring W_SESSION = L"Session";
const std::wstring W_SERVER = L"Server";
const std::wstring W_TRANSPORT = L"Transport";
const std::wstring W_VARY = L"Vary";
const std::wstring W_WWW_AUTHENTICATE = L"WWW-Authenticate";

bool isResponseHeader(const std::wstring header_name);

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_HEADER_H__
