#include "rtsp/session.h"

#include <iostream>
#include "util/util.h"

#include "rtsp/message/message_interface.h"
#include "util/utf8_converter.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

bool isUtf8(unsigned char* str, std::size_t len)
{
    namespace util = rtsp_server::util;

    std::string error_msg;
    bool is_utf8 = (util::isUtf8(str, len, error_msg) == 0 ? true : false);
    if (! is_utf8)
    {
        std::cout << error_msg << std::endl;
        return false;
    }

    return true;
}

} // namespace

namespace rtsp_server {
namespace rtsp {

Session::Session() : m_observer(NULL), m_session_id(util::noneId())
{

}

void Session::init(parser::ParserManagerPtr rtsp_parser_manager,
                   task::WorkerPtr worker)
{
    m_rtsp_parser_manager = rtsp_parser_manager;
    m_worker = worker;
}

void Session::sendRtspPacket(unsigned char* data, std::size_t size)
{
    m_service_handler->writeAsync(data, size);
}

void Session::onRead(unsigned char* data, std::size_t size)
{
    if (! isUtf8(data, size))
    {
        return;
    }
    m_rtsp_message.insert(m_rtsp_message.end(), data, data+size);

    int end_of_rtsp_message = -1;
    std::string rtsp_message_str(m_rtsp_message.begin(), m_rtsp_message.end());

    message::MessageInterfacePtr message =
        m_rtsp_parser_manager->parse(rtsp_message_str, end_of_rtsp_message);
    if (! message)
    {
        return;
    }

    m_rtsp_message.erase(m_rtsp_message.begin(),
                         m_rtsp_message.begin() + end_of_rtsp_message);
//   vector_clear(m_rtsp_message);

    if (m_observer)
    {
        m_observer->onReceive(message);
    }
}

void Session::onWrite(unsigned char* /*data*/, std::size_t /*size*/)
{

}

void Session::onClose()
{

}

void Session::setServiceHandler(
    network::TcpServiceHandlerPtr service_handler)
{
    m_service_handler = service_handler;
}

std::string Session::getLocalAddress() const
{
    return m_service_handler->getLocalIp();
}

std::string Session::getRemoteAddress() const
{
    return m_service_handler->getRemoteIp();
}

} // namespace rtsp
} // namespace rtsp_server
