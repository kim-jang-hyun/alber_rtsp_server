#ifndef __RTSP_SERVER_RTP_PAYLOAD_H264_H264_PAYLOAD_H__
#define __RTSP_SERVER_RTP_PAYLOAD_H264_H264_PAYLOAD_H__

#include <boost/cstdint.hpp>

#include "rtp/payload/h264/h264_frame_parser.h"

namespace rtsp_server {
namespace rtp {
namespace payload {
namespace h264 {

class H264Payload
{
public:
    // declare typedef - begin
    enum BitSize {
        // nal unit header
        FORBIDDEN_ZERO_BIT_SIZE = 1,
        NAL_REF_IDC_SIZE = 2,
        NAL_UNIT_PACKET_TYPE_SIZE = 5,

        // fragmentation header
        START_BIT_SIZE = 1,
        END_BIT_SIZE = 1,
        RESERVED_BIT_SIZE = 1,
        NAL_UNIT_TYPE_SIZE = 5,
    };
    enum BitIndex {
        // nal unit header
        FORBIDDEN_ZERO_BIT_INDEX = 0,
        NAL_REF_IDC_INDEX = 1,
        NAL_UNIT_PACKET_TYPE_INDEX = 3,

        // fragmentation header
        START_BIT_INDEX = 0,
        END_BIT_INDEX = 1,
        RESERVED_BIT_INDEX = 2,
        NAL_UNIT_TYPE_INDEX = 3,
    };

    static const int NAL_START_3BYTE_PREFIX = 0x000001;
    static const int NAL_START_4BYTE_PREFIX = 0x00000001;

    static const int NAL_HEADER_SIZE = 1;
    // declare typedef - end

    H264Payload();
    ~H264Payload();

    void makePayload(const H264FrameParser::NalUnit::Header& nal_header,
                     bool start_bit, bool end_bit,
                     const unsigned char* fragment_buf,
                     size_t fragment_size, bool fragmentation_mode);
    unsigned char* marshall(std::size_t& payload_buf_size);

    // property function - begin
    void setFragmentationMode(bool fragmentation_mode);

    void setForbiddenZeroBit(bool forbidden_zero_bit);
    void setNalRefIdc(boost::uint8_t nal_ref_idc);
    void setNalUnitPacketType(boost::uint8_t nal_unit_packet_type);

    void setStartBit(bool start_bit);
    void setEndBit(bool end_bit);
    void setReservedBit(bool reserved_bit);
    void setNalUnitType(boost::uint8_t nal_unit_type);

    void setScanData(const unsigned char* data, unsigned int length);
    // property function - end

private:
    int getHeaderSize();

    void marshallHeader();
    void marshallNalUnitHeader();
    void marshallFragmentatioHeader();

private:
    bool m_fragmentation_mode;

    // header - begin
    bool m_forbidden_zero_bit; // 1bit
    boost::uint8_t m_nal_ref_idc; // 2bit
    boost::uint8_t m_nal_unit_packet_type; // 5bit

    bool m_start_bit; // 1bit
    bool m_end_bit; // 1bit
    bool m_reserved_bit; // 1bit
    boost::uint8_t m_nal_unit_type; // 5bit
    // header - end

    unsigned char* m_header_buf;
    std::size_t m_header_buf_size;

    unsigned char* m_scan_data_buf;
    std::size_t m_scan_data_buf_length;
};

} // namespace h264
} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_H264_H264_PAYLOAD_H__
