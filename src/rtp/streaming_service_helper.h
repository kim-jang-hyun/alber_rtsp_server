#ifndef __RTSP_SERVER_RTP_STREAMING_SERVICE_HELPER_H__
#define __RTSP_SERVER_RTP_STREAMING_SERVICE_HELPER_H__

#include "rtp/payload/jpeg_frame_parser.h"
#include "rtp/payload/jpeg_payload.h"

namespace rtsp_server {
namespace rtp {
namespace streaming_service_helper {

struct JpegParserOutput
{
    unsigned char width; 
    unsigned char height; 
    unsigned char type;
    unsigned char typeSpecific;
    unsigned char precision; 
    unsigned char qFactor; 

    unsigned char* qTables; 
    unsigned short qTablesLength; 

    unsigned short restartInterval; 

    unsigned char* scandata; 
    unsigned int   scandataLength; 
};

void getJpegParserOutput(rtp::payload::JpegFrameParser& parser,
                         JpegParserOutput& output);

void makePayload(const JpegParserOutput& jpeg_parser_output,
                 unsigned char* fragment_buf,
                 size_t fragment_size,
                 size_t fragment_offset,
                 rtp::payload::JpegPayload& payload);

} // streaming_service_helper
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_STREAMING_SERVICE_HELPER_H__
