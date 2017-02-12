#ifndef __RTSP_SERVER_RTP_PAYLOAD_H264_H264_FRAME_PARSER_H__
#define __RTSP_SERVER_RTP_PAYLOAD_H264_H264_FRAME_PARSER_H__

#include "rtp/payload/frame_parser.h"

#include <vector>

namespace rtsp_server {
namespace rtp {
namespace payload {
namespace h264 {

class H264FrameParser : public FrameParser
{
public:
    // declare typedef - begin
    struct NalUnit
    {
        NalUnit() : header(), data(NULL), data_size(0) {}

        struct Header
        {
            Header() : forbidden_zero_bit(0), nal_ref_idc(0), nal_unit_type(0) {}

            unsigned char forbidden_zero_bit;
            unsigned char nal_ref_idc;
            unsigned char nal_unit_type;
        };
        Header header;

        unsigned char* data;
        std::size_t data_size;
    };
    typedef std::vector<NalUnit> NalUnitVec;
    // declare typedef - end

    H264FrameParser();
    virtual ~H264FrameParser();

    virtual bool parse(const unsigned char* src, std::size_t src_size);

    // property function - begin
    NalUnitVec getNalUnitVec() const { return m_nal_unit_vec; }
    // property function - end

private:
    void parseNalHeader(const unsigned char* chunk,
                        int nal_begin_pos, int nal_end_pos,
                        NalUnit::Header& nal_header);
    void parseNalData(const unsigned char* chunk,
                      int nal_begin_pos, int nal_end_pos,
                      unsigned char*& data, std::size_t& data_size);

private:
    std::vector<NalUnit> m_nal_unit_vec;
};
typedef boost::shared_ptr<H264FrameParser> H264FrameParserPtr;

} // namespace h264
} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_H264_H264_FRAME_PARSER_H__
