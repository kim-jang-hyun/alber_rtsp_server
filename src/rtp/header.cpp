#include "rtp/header.h"

#include <boost/assert.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/foreach.hpp>
#include <boost/integer/integer_mask.hpp>

#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

enum BitIndex { // 8 bit
                VERSION_BIT_INDEX = 0, PADDING_BIT_INDEX = 2, 
                EXTENSION_BIT_INDEX = 3, CSRC_COUNT_BIT_INDEX = 4,
                // 8bit
                MARKER_BIT_INDEX = 0, PAYLOAD_TYPE_BIT_INDEX = 1, };

} // namespace

namespace rtsp_server {
namespace rtp {

Header::Header() : m_buffer(NULL), m_buffer_size(0)
{

}

Header::~Header()
{
    util::safe_delete_array(m_buffer);
}

unsigned char* Header::marshall(std::size_t& buf_size)
{
    int header_size = getHeaderSize();
    BOOST_VERIFY(header_size >= 0);

    m_buffer = new unsigned char[header_size];

    {
        unsigned char chunk = {0, };
        chunk |= (m_version <<
            util::toLeastSignificantBitFirst(VERSION_BIT_INDEX,
                                             VERSION_BIT_SIZE,
                                             sizeof(chunk)));
        chunk |= (static_cast<unsigned char>(m_padding) << 
            util::toLeastSignificantBitFirst(PADDING_BIT_INDEX,
                                             PADDING_BIT_SIZE,
                                             sizeof(chunk)));
        chunk |= (static_cast<unsigned char>(m_extension) <<
            util::toLeastSignificantBitFirst(EXTENSION_BIT_INDEX,
                                             EXTENSION_BIT_SIZE,
                                             sizeof(chunk)));
        chunk |= (m_csrc_count <<
            util::toLeastSignificantBitFirst(CSRC_COUNT_BIT_INDEX,
                                             CSRC_COUNT_BIT_SIZE,
                                             sizeof(chunk)));

        int chunk_size = sizeof(chunk);
        std::memcpy(m_buffer + m_buffer_size, &chunk, chunk_size);
        m_buffer_size += chunk_size;
    }

    {
        unsigned char chunk = {0, };
        chunk |= (static_cast<unsigned char>(m_marker) <<
            util::toLeastSignificantBitFirst(MARKER_BIT_INDEX,
                                             MARKER_BIT_SIZE,
                                             sizeof(chunk)));
        chunk |= (m_payload_type <<
            util::toLeastSignificantBitFirst(PAYLOAD_TYPE_BIT_INDEX,
                                             PAYLOAD_TYPE_BIT_SIZE,
                                             sizeof(chunk)));

        int chunk_size = sizeof(chunk);
        std::memcpy(m_buffer + m_buffer_size, &chunk, chunk_size);
        m_buffer_size += chunk_size;
    }

    {
        m_sequence_number = boost::endian::native_to_big(m_sequence_number);

        int chunk_size = sizeof(m_sequence_number);
        std::memcpy(m_buffer + m_buffer_size, &m_sequence_number, chunk_size);
        m_buffer_size += chunk_size;
    }

    {
        m_timestamp = boost::endian::native_to_big(m_timestamp);

        int chunk_size = sizeof(m_timestamp);
        std::memcpy(m_buffer + m_buffer_size, &m_timestamp, chunk_size);
        m_buffer_size += chunk_size;
    }

    {
        m_ssrc = boost::endian::native_to_big(m_ssrc);

        int chunk_size = sizeof(m_ssrc);
        std::memcpy(m_buffer + m_buffer_size, &m_ssrc, chunk_size);
        m_buffer_size += chunk_size;
    }

    BOOST_FOREACH(CsrcVector::value_type csrc, m_csrc_list)
    {
        csrc = boost::endian::native_to_big(csrc);

        int chunk_size = sizeof(csrc);
        std::memcpy(m_buffer + m_buffer_size, &csrc, chunk_size);
        m_buffer_size += chunk_size;
    }

    buf_size = m_buffer_size;
    unsigned char* buf = new unsigned char[buf_size];
    memcpy(buf, m_buffer, m_buffer_size);

    return buf;
}

void Header::setVersion(unsigned char version)
{
    boost::uint_t<VERSION_BIT_SIZE>::fast VersionMaxValue =
        boost::low_bits_mask_t<VERSION_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(version <= VersionMaxValue);

    m_version = version;
}

void Header::setPadding(bool set)
{
    m_padding = set;
}

void Header::setExtension(bool set)
{
    m_extension = set;
}

void Header::setCsrcCount(unsigned char csrc_count)
{
    boost::uint_t<CSRC_COUNT_BIT_SIZE>::fast CsrcCountMaxValue =
        boost::low_bits_mask_t<CSRC_COUNT_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(csrc_count <= CsrcCountMaxValue);

    m_csrc_count = csrc_count;
}

void Header::setMarker(bool set)
{
    m_marker = set;
}

void Header::setPayloadType(unsigned char payload_type)
{
    boost::uint_t<PAYLOAD_TYPE_BIT_SIZE>::fast PayloadTypeMaxValue =
        boost::low_bits_mask_t<PAYLOAD_TYPE_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(payload_type <= PayloadTypeMaxValue);

    m_payload_type = payload_type;
}

void Header::setSequenceNumber(boost::uint16_t sequence_number)
{
    boost::uint_t<SEQUENCE_NUMBER_BIT_SIZE>::fast SequenceNumberMaxValue =
        boost::low_bits_mask_t<SEQUENCE_NUMBER_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(sequence_number <= SequenceNumberMaxValue);

    m_sequence_number = sequence_number;
}

void Header::setTimeStamp(boost::uint32_t timestamp)
{
    boost::uint_t<TIMESTAMP_BIT_SIZE>::fast TimeStampMaxValue =
        boost::low_bits_mask_t<TIMESTAMP_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(timestamp <= TimeStampMaxValue);

    m_timestamp = timestamp;
}

void Header::setSsrc(boost::uint32_t ssrc)
{
    boost::uint_t<SSRC_BIT_SIZE>::fast SsrcMaxValue =
        boost::low_bits_mask_t<SSRC_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(ssrc <= SsrcMaxValue);

    m_ssrc = ssrc;
}

void Header::setCsrcList(const std::vector<boost::uint32_t>& csrc_list)
{
    m_csrc_list = csrc_list;
}

int Header::getHeaderSize()
{
    int bit_members_byte_size = (VERSION_BIT_SIZE +
                                 PADDING_BIT_SIZE + 
                                 EXTENSION_BIT_SIZE +
                                 CSRC_COUNT_BIT_SIZE +
                                 MARKER_BIT_SIZE +
                                 PAYLOAD_TYPE_BIT_SIZE) / 8;

    return bit_members_byte_size +
           sizeof(m_sequence_number) +
           sizeof(m_timestamp) +
           sizeof(m_ssrc) +
           sizeof(CsrcVector::value_type) * m_csrc_list.size();
}

} // namespace rtp
} // namespace rtsp_server
