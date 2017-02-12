#include "rtp/payload/frame_parser.h"

#include "rtp/payload/jpeg/jpeg_frame_parser.h"
#include "rtp/payload/h264/h264_frame_parser.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtp {
namespace payload {

FrameParser::FrameParser()
{

}

FrameParser::~FrameParser()
{

}

FrameParserPtr createFrameParser(PayloadType payload_type)
{
    switch (payload_type)
    {
    case MJPEG_PAYLOAD_TYPE: return FrameParserPtr(new jpeg::JpegFrameParser);
    case H264_PAYLOAD_TYPE: return FrameParserPtr(new h264::H264FrameParser);
    }

    return NULL_SHARED_PTR(FrameParserPtr);
}

} // namespace payload
} // namespace rtp
} // namespace rtsp_server
