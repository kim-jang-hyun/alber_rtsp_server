#ifndef __RTSP_SERVER_RTSP_PARSER_REQUEST_PARSER_H__
#define __RTSP_SERVER_RTSP_PARSER_REQUEST_PARSER_H__

#include "rtsp/parser/parser_interface.h"

#include <string>
#include "rtsp/message/request/message_item.h"

namespace rtsp_server {
namespace rtsp {
namespace parser {

class RequestParser : public ParserInterface
{
public:
    virtual ~RequestParser();

    // derived from ParserInterface - begin
    virtual message::MessageInterfacePtr parse(const std::string& msg,
                                               int& end_of_message_index);
    // derived from ParserInterface - end

private:
    int parseRequestLine(const std::wstring& msg, int next_parse_index,
                         message::request::MessageItem& msg_item);
    int parseRequestHeader(const std::wstring& msg, int next_parse_index,
                           message::request::MessageItem& msg_item);
};

} // namespace parser
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_PARSER_REQUEST_PARSER_H__
