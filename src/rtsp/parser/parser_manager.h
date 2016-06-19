#ifndef __RTSP_SERVER_RTSP_PARSER_PARSER_MANAGER_H__
#define __RTSP_SERVER_RTSP_PARSER_PARSER_MANAGER_H__

#include <set>
#include <string>
#include <boost/shared_ptr.hpp>

#include "rtsp/message/message_interface.h"

namespace rtsp_server {
namespace rtsp {
namespace parser {

class ParserInterface;

class ParserManager
{
public:
    ~ParserManager();

    void addParser(ParserInterface* parser);
    message::MessageInterfacePtr parse(const std::string& msg,
                                       int& end_of_message);

private:
    typedef std::set<ParserInterface*> ParserSet;

    ParserSet m_parser_set;
};
typedef boost::shared_ptr<ParserManager> ParserManagerPtr;

} // namespace parser
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_PARSER_PARSER_MANAGER_H__
