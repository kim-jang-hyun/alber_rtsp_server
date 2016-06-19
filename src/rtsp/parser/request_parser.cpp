#include "rtsp/parser/request_parser.h"

#include <boost/algorithm/string/trim.hpp>
#include "rtsp/message/message_interface.h"
#include "rtsp/message/request/message.h"
#include "rtsp/message/request/message_factory.h"
#include "rtsp/message/syntax.h"
#include "util/utf8_converter.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

using namespace rtsp_server;
const int INVALID_INDEX = -1;

template <typename T>
int subString(const T& msg, const T& key, std::size_t begin_pos, T& sub_str)
{
    std::size_t found = msg.find(key, begin_pos);
    if (found == std::wstring::npos)
    {
        return INVALID_INDEX;
    }

    sub_str.assign(msg.begin() + begin_pos, msg.begin() + found);

    return found + key.length();
}

int findEndOfHeaderIndex(const std::string& msg)
{
    std::string end_of_header_utf8;
    if (! util::convertUtf16ToUtf8(rtsp::message::syntax::W_END_OF_HEADER,
                                   end_of_header_utf8))
    {
        return INVALID_INDEX;
    }

    std::size_t found = msg.find(end_of_header_utf8);
    if (found == std::string::npos)
    {
        return INVALID_INDEX;
    }

    return found + end_of_header_utf8.length();
}

}  // namespace

namespace rtsp_server {
namespace rtsp {
namespace parser {

RequestParser::~RequestParser()
{

}

message::MessageInterfacePtr RequestParser::parse(const std::string& msg,
                                                  int& end_of_message_index)
{
    end_of_message_index = findEndOfHeaderIndex(msg);
    if (end_of_message_index == INVALID_INDEX)
    {
        return NULL_SHARED_PTR(message::MessageInterfacePtr);
    }

    std::wstring request_msg_utf16;
    std::string request_msg(msg.begin(), msg.begin() + end_of_message_index);

    if (! util::convertUtf8ToUtf16(request_msg, request_msg_utf16))
    {
        return NULL_SHARED_PTR(message::MessageInterfacePtr);
    }

    message::request::MessageItem rtsp_request_message_item;
    int next_parse_index = 0;
    next_parse_index = parseRequestLine(request_msg_utf16, next_parse_index,
                                        rtsp_request_message_item);
    if (next_parse_index == INVALID_INDEX)
    {
        return NULL_SHARED_PTR(message::MessageInterfacePtr);
    }

    next_parse_index = parseRequestHeader(request_msg_utf16, next_parse_index,
                                          rtsp_request_message_item);
    if (next_parse_index == INVALID_INDEX)
    {
        return NULL_SHARED_PTR(message::MessageInterfacePtr);
    }

    return message::request::MessageFactory::createMsg(rtsp_request_message_item);
}

int RequestParser::parseRequestLine(const std::wstring& msg,
                                        int next_parse_index,
                                        message::request::MessageItem& msg_item)
{
    using namespace rtsp::message;

    std::wstring method;
    int end_index = subString(msg, syntax::W_SP, next_parse_index, method);
    if (end_index == INVALID_INDEX)
    {
        return INVALID_INDEX;
    }

    std::wstring uri;
    end_index = subString(msg, syntax::W_SP, end_index, uri);
    if (end_index == INVALID_INDEX)
    {
        return INVALID_INDEX;
    }

    std::wstring version;
    end_index = subString(msg, syntax::W_CRLF, end_index, version);
    if (end_index == INVALID_INDEX)
    {
        return INVALID_INDEX;
    }

    msg_item.setRequestLine(method, uri, version);

    return end_index;
}

int RequestParser::parseRequestHeader(const std::wstring& msg,
                                          int next_parse_index,
                                          message::request::MessageItem& msg_item)
{
    using namespace rtsp::message;

    std::size_t end_of_header_index = msg.find(syntax::W_END_OF_HEADER);
    if (end_of_header_index == std::wstring::npos)
    {
        return INVALID_INDEX;
    }

    int end_index = next_parse_index;
    while (end_index <= (int)end_of_header_index)
    {
        std::wstring name;
        end_index = subString(msg, syntax::W_COLON, end_index, name);
        if (end_index == INVALID_INDEX)
        {
            return INVALID_INDEX;
        }
        boost::algorithm::trim(name);

        std::wstring value;
        end_index = subString(msg, syntax::W_CRLF, end_index, value);
        if (end_index == INVALID_INDEX)
        {
            return INVALID_INDEX;
        }
        boost::algorithm::trim(value);

        msg_item.addHeader(name, value);
    }

    return end_of_header_index;
}

} // namespace parser
} // namespace rtsp
} // namespace rtsp_server
