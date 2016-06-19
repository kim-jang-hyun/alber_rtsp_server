#ifndef __RTSP_SERVER_RTSP_PARSER_PARSER_INTERFACE_H__
#define __RTSP_SERVER_RTSP_PARSER_PARSER_INTERFACE_H__

#include <string>
#include "rtsp/message/message_interface.h"

namespace rtsp_server {
namespace rtsp {
namespace parser {

class ParserInterface
{
public:
    virtual ~ParserInterface() {}

    virtual message::MessageInterfacePtr parse(const std::string& msg,
                                               int& end_of_message_index) = 0;
};

} // namespace parser
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_PARSER_PARSER_INTERFACE_H__
