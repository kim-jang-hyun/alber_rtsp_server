#include "streaming_service_helper.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace rtp {
namespace streaming_service_helper {

void getJpegParserOutput(rtp::payload::JpegFrameParser& parser,
    JpegParserOutput& output)
{
    output.width = parser.width();
    output.height = parser.height();
    output.type = parser.type();
    output.typeSpecific = parser.typeSpecific();
    output.precision = parser.precision();
    output.qFactor = parser.qFactor();
    output.qTables = (unsigned char*)parser.quantizationTables(output.qTablesLength);
    output.restartInterval = parser.restartInterval();
    output.scandata = (unsigned char*)parser.scandata(output.scandataLength);
}

void makePayload(const JpegParserOutput& jpeg_parser_output,
    unsigned char* fragment_buf,
    size_t fragment_size,
    size_t fragment_offset,
    rtp::payload::JpegPayload& payload)
{
    payload.setTypeSpecific(jpeg_parser_output.typeSpecific);
    payload.setFragmentOffset(fragment_offset);
    payload.setType(jpeg_parser_output.type);
    payload.setQ(jpeg_parser_output.qFactor);
    payload.setWidth(jpeg_parser_output.width);
    payload.setHeight(jpeg_parser_output.height);

    unsigned char type = jpeg_parser_output.type;
    if (64 <= type && type <= 127)
    {
        unsigned short dri = jpeg_parser_output.restartInterval;
        payload.setRestartInterval(dri);
        payload.setF(1);
        payload.setL(1);
        payload.setRestartCount(0X3FFF);

        payload.setRestartMarkerHeader(true);
    }

    unsigned char q_factor = jpeg_parser_output.qFactor;
    if (fragment_offset == 0 &&
        (128 <= q_factor && q_factor <= 255))
    {
        payload.setMbz(0);
        payload.setPrecision(jpeg_parser_output.precision);
        payload.setLength(jpeg_parser_output.qTablesLength);
        payload.setQTable(jpeg_parser_output.qTables);

        payload.setQuantizationTableHeader(true);
    }

    payload.setScanData(fragment_buf, fragment_size);
}

} // streaming_service_helper
} // namespace rtp
} // namespace rtsp_server
