#ifndef __RTSP_SERVER_RTSP_MESSAGE_GENERAL_HEADER_H__
#define __RTSP_SERVER_RTSP_MESSAGE_GENERAL_HEADER_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {

const std::wstring W_CSEQ = L"CSeq";
const std::wstring W_CACHE_CONTROL = L"Cache-Control";
const std::wstring W_CONNECTION = L"Connection";
const std::wstring W_DATE = L"Date";
const std::wstring W_VIA = L"Via";

bool isGeneralHeader(const std::wstring header_name);

} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_GENERAL_HEADER_H__
