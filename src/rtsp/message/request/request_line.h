#ifndef __RTSP_SERVER_RTSP_MESSAGE_REQUEST_REQUEST_LINE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_REQUEST_REQUEST_LINE_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace request {

const std::wstring W_ANNOUNCE      = L"ANNOUNCE";
const std::wstring W_DESCRIBE      = L"DESCRIBE";
const std::wstring W_GET_PARAMETER = L"GET_PARAMETER";
const std::wstring W_OPTIONS       = L"OPTIONS";
const std::wstring W_PAUSE         = L"PAUSE";
const std::wstring W_PLAY          = L"PLAY";
const std::wstring W_RECORD        = L"RECORD";
const std::wstring W_REDIRECT      = L"REDIRECT";
const std::wstring W_SETUP         = L"SETUP";
const std::wstring W_SET_PARAMETER = L"SET_PARAMETER";
const std::wstring W_TEARDOWN      = L"TEARDOWN";

struct RequestLine
{
    std::wstring method;
    std::wstring uri;
    std::wstring version;
};

} // namespace request
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_REQUEST_REQUEST_LINE_H__
