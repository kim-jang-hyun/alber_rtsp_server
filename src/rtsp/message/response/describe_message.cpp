#include "rtsp/message/response/describe_message.h"

#include <boost/assert.hpp>

#include "util/util.h"
#include "rtsp/message/entity_header.h"
#include "rtsp/message/general_header.h"
#include "rtsp/message/request/request_header.h"
#include "util/time_util.h"
#include "util/utf8_converter.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace sdp = rtsp_server::rtsp::message::sdp;

namespace {

static const wchar_t* FILE_SAMPLE[] = 
{
    L"mjpeg_test.avi",
    L"test_1.264",
    L"test_2.264",
    L"test_3.264",
    L"test_4.264",
};

bool checkMrl(std::wstring uri)
{
    for (int i = 0; i < sizeof(FILE_SAMPLE) / sizeof(FILE_SAMPLE[0]); ++i)
    {
        std::size_t found = uri.find(FILE_SAMPLE[i]);
        if (found != std::wstring::npos)
        {
            return true;
        }
    }

    return false;
}

} // namespace

namespace rtsp_server {
namespace rtsp {
namespace message {
namespace response {

DescribeMessage::DescribeMessage(const std::string& local_ip)
    : Message(MessageInterface::RESPONSE_METHOD_DESCRIBE),
      m_local_ip(local_ip)
{

}

DescribeMessage::~DescribeMessage()
{

}

bool DescribeMessage::interprete(const request::MessageItem& request_msg_item)
{
    if (! Message::interprete(request_msg_item))
    {
        return false;
    }

    int error_code = errorCheck(request_msg_item);
    if (error_code != 200)
    {
        m_response_msg_item.setStatus(error_code);
        return false;
    }

    m_sdp = sdp::createSdp(request_msg_item.request_line.uri);
    if (! m_sdp)
    {
        return false;
    }

    m_sdp->generate(m_local_ip);

    addDateHeader();
    addContentType();
    addContentLength();

    addSdp();
    return true;
}

int DescribeMessage::errorCheck(const request::MessageItem& request_msg_item) const
{
    if (! checkMrl(request_msg_item.request_line.uri))
    {
        return 404;
    }

    std::wstring accept_value;
    if (request_msg_item.getHeaderValue(request::W_ACCEPT, accept_value))
    {
        if (accept_value != sdp::W_SDP_MEDIA_TYPE)
        {
            return 406;
        }
    }

    return 200;
}

void DescribeMessage::addDateHeader()
{
    util::TimeUtil time;
    std::string date_header_value = time.getCurrentTimeByHttpDateFormat();
    std::wstring date_header_value_utf16 = util::toUtf16(date_header_value);

    m_response_msg_item.addHeader(W_DATE, date_header_value_utf16);
}

void DescribeMessage::addContentType()
{
    m_response_msg_item.addHeader(W_CONTENT_TYPE, sdp::W_SDP_MEDIA_TYPE);
}

void DescribeMessage::addContentLength()
{
    int sdp_length = m_sdp->getContentLength();
    std::string sdp_length_str = util::toString<std::string>(sdp_length);

    m_response_msg_item.addHeader(W_CONTENT_LENGTH, util::toUtf16(sdp_length_str));
}

void DescribeMessage::addSdp()
{
    std::string sdp_content = m_sdp->getContent();

    m_response_msg_item.addBody(util::toUtf16(sdp_content));
}

} // namespace response
} // namespace message
} // namespace rtsp
} // namespace rtsp_server
