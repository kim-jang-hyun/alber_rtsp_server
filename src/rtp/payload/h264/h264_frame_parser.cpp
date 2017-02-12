#include "rtp/payload/h264/h264_frame_parser.h" 

#include <boost/foreach.hpp>

#include "rtp/payload/h264/h264_payload.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

bool findNalUnitRange(const unsigned char* src, std::size_t src_size,
                      int& nal_begin_pos, int& nal_end_pos)
{
    typedef rtsp_server::rtp::payload::h264::H264Payload H264Payload;
    using rtsp_server::util::read3byte;
    using rtsp_server::util::read4byte;

    std::size_t offset = 0;

    if (src_size > 3 &&
        read3byte(src) == H264Payload::NAL_START_3BYTE_PREFIX)
    {
        offset += 3;
    }
    else if (src_size > 4 &&
             read4byte(src) == H264Payload::NAL_START_4BYTE_PREFIX)
    {
        offset += 4;
    }
    else
    {
        return false;
    }

    nal_begin_pos = offset;

    while (offset < src_size)
    {
        if (src_size - offset < 4)
        {
            offset = src_size - 1;
            break;
        }

        if (read3byte(src + offset) == H264Payload::NAL_START_3BYTE_PREFIX)
        {
            --offset;
            break;
        }
        else if (read4byte(src + offset) == H264Payload::NAL_START_4BYTE_PREFIX)
        {
            --offset;
            break;
        }
        else
        {
            ++offset;
        }
    }

    nal_end_pos = offset;

    return true;
}

} // namespace

namespace rtsp_server {
namespace rtp {
namespace payload {
namespace h264 {

H264FrameParser::H264FrameParser()
{

}

H264FrameParser::~H264FrameParser()
{
    BOOST_FOREACH(NalUnit& nal_unit, m_nal_unit_vec)
    {
        util::safe_delete_array(nal_unit.data);
    }
}

bool H264FrameParser::parse(const unsigned char* src, std::size_t src_size)
{
    std::size_t offset = 0;

    while (offset < src_size)
    {
        const unsigned char* chunk = src + offset;
        std::size_t chunk_size = src_size - offset;

        int nal_begin_pos = 0, nal_end_pos = 0;
        bool find_ret = findNalUnitRange(chunk, chunk_size, nal_begin_pos, nal_end_pos);
        if (! find_ret)
        {
            break;
        }

        NalUnit nal_unit;
        {
            parseNalHeader(chunk, nal_begin_pos, nal_end_pos, nal_unit.header);
            parseNalData(chunk, nal_begin_pos, nal_end_pos, nal_unit.data, nal_unit.data_size);
        }
        m_nal_unit_vec.push_back(nal_unit);

        offset = offset + (nal_end_pos + 1);
    }

    return true;
}

void H264FrameParser::parseNalHeader(const unsigned char* chunk,
                                     int nal_begin_pos, int /*nal_end_pos*/,
                                     NalUnit::Header& nal_header)
{
    unsigned char header = util::read1byte(chunk + nal_begin_pos);

    nal_header.forbidden_zero_bit = util::getValueFromBitRange(
                                        H264Payload::FORBIDDEN_ZERO_BIT_INDEX,
                                        H264Payload::FORBIDDEN_ZERO_BIT_SIZE,
                                        header);
    nal_header.nal_ref_idc = util::getValueFromBitRange(
                                        H264Payload::NAL_REF_IDC_INDEX,
                                        H264Payload::NAL_REF_IDC_SIZE,
                                        header);
    nal_header.nal_unit_type = util::getValueFromBitRange(
                                        H264Payload::NAL_UNIT_PACKET_TYPE_INDEX,
                                        H264Payload::NAL_UNIT_PACKET_TYPE_SIZE,
                                        header);
}

void H264FrameParser::parseNalData(const unsigned char* chunk,
                                   int nal_begin_pos, int nal_end_pos,
                                   unsigned char*& data, std::size_t& data_size)
{
    int nal_size = nal_end_pos - nal_begin_pos + 1;
    data_size = nal_size - H264Payload::NAL_HEADER_SIZE;

    data = new unsigned char[data_size];

    memcpy(data, chunk + nal_begin_pos + H264Payload::NAL_HEADER_SIZE, data_size);
}

} // namespace h64
} // namespace payload
} // namespace rtp
} // namespace rtsp_server
