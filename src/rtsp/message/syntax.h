#ifndef __RTSP_SERVER_RTSP_MESSAGE_SYNTAX_H__
#define __RTSP_SERVER_RTSP_MESSAGE_SYNTAX_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace syntax {

const std::wstring W_RTSP_VERSION = L"RTSP/1.0";
const std::wstring W_CRLF = L"\r\n"; 
const std::wstring W_END_OF_HEADER = W_CRLF + W_CRLF;
const std::wstring W_SP = L" ";
const std::wstring W_HT = L"\t";
const std::wstring W_COLON = L":";
const std::wstring W_SEMI_COLON = L";";
const std::wstring W_SLASH = L"/";

} // namespace syntax
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_SYNTAX_H__
