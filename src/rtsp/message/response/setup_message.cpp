#include "rtsp/message/response/setup_message.h"

#include <string>
#include <sstream>
#include <boost/tokenizer.hpp>
#include "util/util.h"

#include "rtsp/message/request/request_header.h"
#include "rtsp/message/response/response_header.h"
#include "rtsp/message/syntax.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

const std::wstring W_TRANSPORT_PROTOCOL = L"RTP";
const std::wstring W_PROFILE = L"AVP";
const std::wstring W_LOWER_TRANSPORT = L"UDP";

const std::wstring W_UNICAST_PARAMETER = L"unicast";
const std::wstring W_CLIENT_PORT_PARAMETER = L"client_port";
const std::wstring W_SERVER_PORT_PARAMETER = L"server_port";

std::wstring getClientPortParameter(const std::wstring& request_transport_header)
{
    std::wstring target_string = W_CLIENT_PORT_PARAMETER + L"=";

    return rtsp_server::util::truncateString<std::wstring>(request_transport_header,
                                                target_string);
}

void getClientPort(const std::wstring& client_port_parameter,
                   unsigned short& rtp_port, unsigned short& rtcp_port)
{
    // separate client_port_parameter into token_vec.
    std::vector<std::wstring> token_vec;
    {
        typedef boost::tokenizer<boost::char_separator<wchar_t>,
                                 std::wstring::const_iterator,
                                 std::wstring> tokenizer;
        boost::char_separator<wchar_t> sep(L"=-");

        tokenizer tokens(client_port_parameter, sep);
        for(tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it)
        {
            token_vec.push_back(*it);
        }
        assert(token_vec.size() == 3);
    }

    rtsp_server::util::toInteger(token_vec[1], rtp_port);
    rtsp_server::util::toInteger(token_vec[2], rtcp_port);
}

} // namespace

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

SetupMessage::SetupMessage(unsigned short rtp_port, unsigned short rtcp_port)
    : Message(RESPONSE_METHOD_SETUP),
      m_client_rtp_port(0), m_client_rtcp_port(0),
      m_server_rtp_port(rtp_port), m_server_rtcp_port(rtcp_port),
      m_session_id(util::noneId())
{

}

SetupMessage::~SetupMessage()
{

}

bool SetupMessage::interprete(const request::MessageItem& request_msg_item)
{
    if (! Message::interprete(request_msg_item))
    {
        return false;
    }

    addSession();
    addTransport(request_msg_item);

    return true;
}

void SetupMessage::addSession()
{
    m_session_id = util::generateId();
    m_response_msg_item.addHeader(W_SESSION, util::to_wstring(m_session_id));
}

void SetupMessage::addTransport(const request::MessageItem& request_msg_item)
{
    using namespace syntax;

    // client에서 rtp를 tcp로 보내는 경우를 처리하지 못했다.
    // lower_transport가 TCP인 경우.
    std::wstring request_transport_header;
    request_msg_item.getHeaderValue(request::W_TRANSPORT,
                                    request_transport_header);

    std::wstring client_port_parameter = getClientPortParameter(
        request_transport_header);
    assert(! client_port_parameter.empty());

    getClientPort(client_port_parameter, m_client_rtp_port, m_client_rtcp_port);

    std::wstringstream transport_header_stream;
    transport_header_stream << W_TRANSPORT_PROTOCOL << W_SLASH
                            << W_PROFILE << W_SLASH
                            << W_LOWER_TRANSPORT << W_SEMI_COLON
                            << W_UNICAST_PARAMETER << W_SEMI_COLON
                            << client_port_parameter << W_SEMI_COLON
                            << W_SERVER_PORT_PARAMETER << L"="
                            << m_server_rtp_port << L"-" << m_server_rtcp_port;

    m_response_msg_item.addHeader(W_TRANSPORT,
                                  transport_header_stream.str());
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
