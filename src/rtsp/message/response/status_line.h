#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_LINE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_LINE_H__

#include <string>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

bool getStatusCodeFromTable(int key, std::wstring& status_code_str);
bool getReasonPhraseFromTable(int key, std::wstring& reason_phrase);

struct StatusLine
{
    std::wstring version;
    std::wstring status_code;
    std::wstring reason_phrase;
};

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_LINE_H__
