#include "rtsp/message/response/status_line.h"
#include "rtsp/message/response/status_code_and_reason_phrase_table.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

bool getStatusCodeFromTable(int key, std::wstring& status_code_str)
{
    StatusCodeAndReasonPhraseMap::iterator it = 
        STATUS_CODE_AND_REASON_PHARASE_TABLA.find(key);
    if (it == STATUS_CODE_AND_REASON_PHARASE_TABLA.end())
    {
        return false;
    }

    const StatusCodeAndReasonPhrase&
        status_code_and_reason_phrase = it->second;
        status_code_str = status_code_and_reason_phrase.status_code;

    return true;
}

bool getReasonPhraseFromTable(int key, std::wstring& reason_phrase)
{
    StatusCodeAndReasonPhraseMap::iterator it = 
        STATUS_CODE_AND_REASON_PHARASE_TABLA.find(key);
    if (it == STATUS_CODE_AND_REASON_PHARASE_TABLA.end())
    {
        return false;
    }

    const StatusCodeAndReasonPhrase&
        status_code_and_reason_phrase = it->second;
    reason_phrase = status_code_and_reason_phrase.reason_phrase;

    return true;
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
