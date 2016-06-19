#include "rtsp/parser/parser_manager.h"

#include <boost/foreach.hpp>

#include "rtsp/parser/parser_interface.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtsp {
namespace parser {

ParserManager::~ParserManager()
{
    BOOST_FOREACH(ParserInterface* parser, m_parser_set)
    {
        util::safe_delete(parser);
    }
    m_parser_set.clear();
}

void ParserManager::addParser(ParserInterface* parser)
{
    m_parser_set.insert(parser);
}

message::MessageInterfacePtr ParserManager::parse(const std::string& msg,
                                                  int& end_of_message)
{
    BOOST_FOREACH(ParserInterface* parser, m_parser_set)
    {
        message::MessageInterfacePtr target_message =
            parser->parse(msg, end_of_message);
        if (! target_message)
        {
            continue;
        }
        // end_of_message += body_size;

        return target_message;
    }

    return NULL_SHARED_PTR(message::MessageInterfacePtr);
}

} // namespace rtsp
} // namespace parser
} // namespace rtsp_server
