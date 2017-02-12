#include "rtp/payload/h264/h264_payload.h"

#include <boost/assert.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/integer/integer_mask.hpp>

#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

const int FU_A_PACKET_TYPE = 28;

} // namespace

namespace rtsp_server {
namespace rtp {
namespace payload {
namespace h264 {

H264Payload::H264Payload() : m_fragmentation_mode(false),
                             m_header_buf(NULL), m_header_buf_size(0),
                             m_scan_data_buf(NULL), m_scan_data_buf_length(0)
{

}

H264Payload::~H264Payload()
{
    util::safe_delete_array(m_header_buf);
    util::safe_delete_array(m_scan_data_buf);
}

void H264Payload::makePayload(const H264FrameParser::NalUnit::Header& nal_header,
                              bool start_bit, bool end_bit,
                              const unsigned char* fragment_buf,
                              size_t fragment_size,
                              bool fragmentation_mode)
{
    setFragmentationMode(fragmentation_mode);

    setForbiddenZeroBit(nal_header.forbidden_zero_bit == 1 ? true : false);
    setNalRefIdc(nal_header.nal_ref_idc);

    if (fragmentation_mode)
    {
        setNalUnitPacketType(FU_A_PACKET_TYPE);

        setStartBit(start_bit);
        setEndBit(end_bit);
        setReservedBit(0);
        setNalUnitType(nal_header.nal_unit_type);
    }
    else
    {
        setNalUnitPacketType(nal_header.nal_unit_type);
    }

    setScanData(fragment_buf, fragment_size);
}

unsigned char* H264Payload::marshall(std::size_t& payload_buf_size)
{
    marshallHeader();

    payload_buf_size = m_header_buf_size + m_scan_data_buf_length;
    unsigned char* payload_buf = new unsigned char[payload_buf_size];

    memcpy(payload_buf, m_header_buf, m_header_buf_size);
    memcpy(payload_buf + m_header_buf_size, m_scan_data_buf,
           m_scan_data_buf_length);

    return payload_buf;
}

void H264Payload::setFragmentationMode(bool fragmentation_mode)
{
    m_fragmentation_mode = fragmentation_mode;
}

void H264Payload::setForbiddenZeroBit(bool forbidden_zero_bit)
{
    m_forbidden_zero_bit = forbidden_zero_bit;
}

void H264Payload::setNalRefIdc(boost::uint8_t nal_ref_idc)
{
    boost::uint_t<NAL_REF_IDC_SIZE>::fast NalRefIdcMaxValue =
        boost::low_bits_mask_t<NAL_REF_IDC_SIZE>::sig_bits;
    BOOST_VERIFY(nal_ref_idc <= NalRefIdcMaxValue);

    m_nal_ref_idc = nal_ref_idc;
}

void H264Payload::setNalUnitPacketType(boost::uint8_t nal_unit_packet_type)
{
    boost::uint_t<NAL_UNIT_PACKET_TYPE_SIZE>::fast NalUnitPacketTypeMaxValue =
        boost::low_bits_mask_t<NAL_UNIT_PACKET_TYPE_SIZE>::sig_bits;
    BOOST_VERIFY(nal_unit_packet_type <= NalUnitPacketTypeMaxValue);

    m_nal_unit_packet_type = nal_unit_packet_type;
}

void H264Payload::setStartBit(bool start_bit)
{
    m_start_bit = start_bit;
}

void H264Payload::setEndBit(bool end_bit)
{
    m_end_bit = end_bit;
}

void H264Payload::setReservedBit(bool reserved_bit)
{
    m_reserved_bit = reserved_bit;
}

void H264Payload::setNalUnitType(boost::uint8_t nal_unit_type)
{
    boost::uint_t<NAL_UNIT_TYPE_SIZE>::fast NalUnitTypeMaxValue =
        boost::low_bits_mask_t<NAL_UNIT_TYPE_SIZE>::sig_bits;
    BOOST_VERIFY(nal_unit_type <= NalUnitTypeMaxValue);

    m_nal_unit_type = nal_unit_type;
}

void H264Payload::setScanData(const unsigned char* data, unsigned int length)
{
    m_scan_data_buf_length = length;

    m_scan_data_buf = new unsigned char[length];
    memcpy(m_scan_data_buf, data, length);
}

int H264Payload::getHeaderSize()
{
    int bit_members_byte_size = (FORBIDDEN_ZERO_BIT_SIZE +
                                 NAL_REF_IDC_SIZE + 
                                 NAL_UNIT_PACKET_TYPE_SIZE) / 8;

    if (m_fragmentation_mode)
    {
        bit_members_byte_size += (START_BIT_SIZE +
                                 END_BIT_SIZE +
                                 RESERVED_BIT_SIZE +
                                 NAL_UNIT_TYPE_SIZE) / 8;
    }

    return bit_members_byte_size;
}

void H264Payload::marshallHeader()
{
    int header_size = getHeaderSize();
    BOOST_VERIFY(header_size >= 0);

    m_header_buf = new unsigned char[header_size];

    marshallNalUnitHeader();

    if (m_fragmentation_mode)
    {
        marshallFragmentatioHeader();
    }
}

void H264Payload::marshallNalUnitHeader()
{
    boost::uint8_t chunk = 0;

    {
        boost::uint8_t forbidden_zero_bit = m_forbidden_zero_bit;
        forbidden_zero_bit = boost::endian::native_to_big(forbidden_zero_bit);

        chunk |= (forbidden_zero_bit <<
                      util::toLeastSignificantBitFirst(FORBIDDEN_ZERO_BIT_SIZE,
                                                       FORBIDDEN_ZERO_BIT_INDEX,
                                                       sizeof(chunk)));
    }

    {
        boost::uint8_t nal_ref_idc = m_nal_ref_idc;
        nal_ref_idc = boost::endian::native_to_big(nal_ref_idc);

        chunk |= (nal_ref_idc <<
                      util::toLeastSignificantBitFirst(NAL_REF_IDC_SIZE,
                                                       NAL_REF_IDC_INDEX,
                                                       sizeof(chunk)));
    }

    {
        boost::uint8_t nal_unit_packet_type = m_nal_unit_packet_type;
        nal_unit_packet_type = boost::endian::native_to_big(nal_unit_packet_type);

        chunk |= (nal_unit_packet_type <<
                      util::toLeastSignificantBitFirst(NAL_UNIT_PACKET_TYPE_SIZE,
                                                       NAL_UNIT_PACKET_TYPE_INDEX,
                                                       sizeof(chunk)));
    }

    int chunk_size = sizeof(chunk);
    std::memcpy(m_header_buf + m_header_buf_size, &chunk, chunk_size);
    m_header_buf_size += chunk_size;
}

void H264Payload::marshallFragmentatioHeader()
{
    boost::uint8_t chunk = 0;

    {
        boost::uint8_t start_bit = m_start_bit;
        start_bit = boost::endian::native_to_big(start_bit);

        chunk |= (start_bit <<
                      util::toLeastSignificantBitFirst(START_BIT_SIZE,
                                                       START_BIT_INDEX,
                                                       sizeof(chunk)));
    }

    {
        boost::uint8_t end_bit = m_end_bit;
        end_bit = boost::endian::native_to_big(end_bit);

        chunk |= (end_bit <<
                      util::toLeastSignificantBitFirst(END_BIT_SIZE,
                                                       END_BIT_INDEX,
                                                       sizeof(chunk)));
    }

    {
        boost::uint8_t reserved_bit = m_reserved_bit;
        reserved_bit = boost::endian::native_to_big(reserved_bit);

        chunk |= (reserved_bit <<
                      util::toLeastSignificantBitFirst(RESERVED_BIT_SIZE,
                                                       RESERVED_BIT_INDEX,
                                                       sizeof(chunk)));
    }

    {
        boost::uint8_t nal_unit_type = m_nal_unit_type;
        nal_unit_type = boost::endian::native_to_big(nal_unit_type);

        chunk |= (nal_unit_type <<
                      util::toLeastSignificantBitFirst(NAL_UNIT_TYPE_SIZE,
                                                       NAL_UNIT_TYPE_INDEX,
                                                       sizeof(chunk)));
    }

    int chunk_size = sizeof(chunk);
    std::memcpy(m_header_buf + m_header_buf_size, &chunk, chunk_size);
    m_header_buf_size += chunk_size;
}

} // namespace h264
} // namespace payload
} // namespace rtp
} // namespace rtsp_server
