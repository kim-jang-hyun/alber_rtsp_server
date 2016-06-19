#ifndef __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_CODE_AND_REASON_PHRASE_TABLE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_CODE_AND_REASON_PHRASE_TABLE_H__

#include <string>
#include <map>
#include <boost/assign/list_of.hpp>

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

using namespace boost::assign;

struct StatusCodeAndReasonPhrase
{
    StatusCodeAndReasonPhrase(const std::wstring _status_code,
                              const std::wstring _reason_phrase)
        : status_code(_status_code), reason_phrase(_reason_phrase)
    {
    }

    std::wstring status_code;
    std::wstring reason_phrase;
};

typedef std::map<int/*steatus_code*/,
                StatusCodeAndReasonPhrase> StatusCodeAndReasonPhraseMap;

StatusCodeAndReasonPhraseMap STATUS_CODE_AND_REASON_PHARASE_TABLA = map_list_of
    (100, StatusCodeAndReasonPhrase(L"100", L"Continue"))
    (200, StatusCodeAndReasonPhrase(L"200", L"OK"))
    (201, StatusCodeAndReasonPhrase(L"201", L"Created"))
    (250, StatusCodeAndReasonPhrase(L"250", L"Low on Storage Space"))
    (300, StatusCodeAndReasonPhrase(L"300", L"Multiple Choices"))
    (301, StatusCodeAndReasonPhrase(L"301", L"Moved Permanently"))
    (302, StatusCodeAndReasonPhrase(L"302", L"Moved Temporarily"))
    (303, StatusCodeAndReasonPhrase(L"303", L"See Other"))
    (304, StatusCodeAndReasonPhrase(L"304", L"Not Modified"))
    (305, StatusCodeAndReasonPhrase(L"305", L"Use Proxy"))
    (400, StatusCodeAndReasonPhrase(L"400", L"Bad Request"))
    (401, StatusCodeAndReasonPhrase(L"401", L"Unauthorized"))
    (402, StatusCodeAndReasonPhrase(L"402", L"Payment Required"))
    (403, StatusCodeAndReasonPhrase(L"403", L"Forbidden"))
    (404, StatusCodeAndReasonPhrase(L"404", L"Not Found"))
    (405, StatusCodeAndReasonPhrase(L"405", L"Method Not Allowed"))
    (406, StatusCodeAndReasonPhrase(L"406", L"Not Acceptable"))
    (407, StatusCodeAndReasonPhrase(L"407", L"Proxy Authentication Required"))
    (408, StatusCodeAndReasonPhrase(L"408", L"Request Time-out"))
    (410, StatusCodeAndReasonPhrase(L"410", L"Gone"))
    (411, StatusCodeAndReasonPhrase(L"411", L"Length Required"))
    (412, StatusCodeAndReasonPhrase(L"412", L"Precondition Failed"))
    (413, StatusCodeAndReasonPhrase(L"413", L"Request Entity Too Large"))
    (414, StatusCodeAndReasonPhrase(L"414", L"Request-URI Too Large"))
    (415, StatusCodeAndReasonPhrase(L"415", L"Unsupported Media Type"))
    (451, StatusCodeAndReasonPhrase(L"451", L"Parameter Not Understood"))
    (452, StatusCodeAndReasonPhrase(L"452", L"Conference Not Found"))
    (453, StatusCodeAndReasonPhrase(L"453", L"Not Enough Bandwidth"))
    (454, StatusCodeAndReasonPhrase(L"454", L"Session Not Found"))
    (455, StatusCodeAndReasonPhrase(L"455", L"Method Not Valid in This State"))
    (456, StatusCodeAndReasonPhrase(L"456", L"Header Field Not Valid for Resource"))
    (457, StatusCodeAndReasonPhrase(L"457", L"Invalid Range"))
    (458, StatusCodeAndReasonPhrase(L"458", L"Parameter Is Read-Only"))
    (459, StatusCodeAndReasonPhrase(L"459", L"Aggregate operation not allowed"))
    (460, StatusCodeAndReasonPhrase(L"460", L"Only aggregate operation allowed"))
    (461, StatusCodeAndReasonPhrase(L"461", L"Unsupported transport"))
    (462, StatusCodeAndReasonPhrase(L"462", L"Destination unreachable"))
    (500, StatusCodeAndReasonPhrase(L"500", L"Internal Server Error"))
    (501, StatusCodeAndReasonPhrase(L"501", L"Not Implemented"))
    (502, StatusCodeAndReasonPhrase(L"502", L"Bad Gateway"))
    (503, StatusCodeAndReasonPhrase(L"503", L"Service Unavailable"))
    (504, StatusCodeAndReasonPhrase(L"504", L"Gateway Time-out"))
    (505, StatusCodeAndReasonPhrase(L"505", L"RTSP Version not supported"))
    (551, StatusCodeAndReasonPhrase(L"551", L"Option not supported"));

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_RESPONSE_STATUS_CODE_AND_REASON_PHRASE_TABLE_H__
