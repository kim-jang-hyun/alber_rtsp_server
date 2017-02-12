#ifndef __RTSP_SERVER_RTP_PAYLOAD_FRAME_PARSER_H__
#define __RTSP_SERVER_RTP_PAYLOAD_FRAME_PARSER_H__

#include <boost/shared_ptr.hpp>
#include "rtp/payload/payload_common.h"

namespace rtsp_server {
namespace rtp {
namespace payload {

class FrameParser
{
public:
    FrameParser();
    virtual ~FrameParser();

    virtual bool parse(const unsigned char* src, std::size_t src_size) = 0;
};
typedef boost::shared_ptr<FrameParser> FrameParserPtr;

FrameParserPtr createFrameParser(PayloadType payload_type);

} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_FRAME_PARSER_H__
