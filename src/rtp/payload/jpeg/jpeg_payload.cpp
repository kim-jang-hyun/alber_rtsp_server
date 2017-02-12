#include "rtp/payload/jpeg/jpeg_payload.h"

#include <boost/assert.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/integer/integer_mask.hpp>

#include "rtp/payload/jpeg/jpeg_frame_parser.h"
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

enum BitIndex {
    TYPE_SPECIFIC_BIT_INDEX = 0, FRAGMENT_OFFSET_BIT_INDEX = 8,
    F_BIT_INDEX = 0, L_BIT_INDEX = 1, RESTART_COUNT_BIT_INDEX = 2,
};

} // namespace


namespace rtsp_server {
namespace rtp {
namespace payload {
namespace jpeg {

JpegPayload::JpegPayload()
    : m_header_buf(NULL), m_header_buf_size(0),
      m_restart_marker_header_is_existed(false),
      m_quantization_table_header_is_existed(false),
      m_scan_data_buf(NULL), m_scan_data_buf_length(0)
{

}

JpegPayload::~JpegPayload()
{
    util::safe_delete_array(m_header_buf);
    util::safe_delete_array(m_scan_data_buf);
}

void JpegPayload::makePayload(FrameParserPtr frame_parser,
                              const unsigned char* fragment_buf,
                              size_t fragment_size,
                              size_t fragment_offset)
{
    JpegFrameParserPtr jpeg_frame_parser = 
        boost::dynamic_pointer_cast<JpegFrameParser>(frame_parser);
    BOOST_ASSERT(jpeg_frame_parser != NULL);

    setTypeSpecific(jpeg_frame_parser->typeSpecific());
    setFragmentOffset(fragment_offset);
    setType(jpeg_frame_parser->type());
    setQ(jpeg_frame_parser->qFactor());
    setWidth(jpeg_frame_parser->width());
    setHeight(jpeg_frame_parser->height());

    unsigned char type = jpeg_frame_parser->type();
    if (64 <= type && type <= 127)
    {
        unsigned short dri = jpeg_frame_parser->restartInterval();
        setRestartInterval(dri);
        setF(1);
        setL(1);
        setRestartCount(0X3FFF);

        setRestartMarkerHeader(true);
    }

    unsigned char q_factor = jpeg_frame_parser->qFactor();
    if (fragment_offset == 0 &&
        (128 <= q_factor && q_factor <= 255))
    {
        setMbz(0);
        setPrecision(jpeg_frame_parser->precision());

        unsigned short q_table_length = 0;
        unsigned char* q_table = (unsigned char*)jpeg_frame_parser->quantizationTables(q_table_length);

        setQTable(q_table);
        setLength(q_table_length);
        setQuantizationTableHeader(true);
    }

    setScanData(fragment_buf, fragment_size);
}

unsigned char* JpegPayload::marshall(std::size_t& payload_buf_size)
{
    marshallHeader();

    payload_buf_size = m_header_buf_size + m_scan_data_buf_length;
    unsigned char* payload_buf = new unsigned char[payload_buf_size];

    memcpy(payload_buf, m_header_buf, m_header_buf_size);
    memcpy(payload_buf + m_header_buf_size, m_scan_data_buf,
           m_scan_data_buf_length);

    return payload_buf;
}

void JpegPayload::setTypeSpecific(boost::uint8_t type_specific)
{
    boost::uint_t<TYPE_SPECIFIC_BIT_SIZE>::fast TypeSpecificMaxValue =
        boost::low_bits_mask_t<TYPE_SPECIFIC_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(type_specific <= TypeSpecificMaxValue);

    m_type_specific = type_specific;
}

void JpegPayload::setFragmentOffset(boost::uint32_t fragment_offset)
{
    boost::uint_t<FRAGMENT_OFFSET_BIT_SIZE>::fast FragmentOffsetMaxValue =
        boost::low_bits_mask_t<FRAGMENT_OFFSET_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(fragment_offset <= FragmentOffsetMaxValue);

    m_fragment_offset = fragment_offset;
}

void JpegPayload::setType(boost::uint8_t type)
{
    boost::uint_t<TYPE_BIT_SIZE>::fast TypeMaxValue =
        boost::low_bits_mask_t<TYPE_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(type <= TypeMaxValue);

    m_type = type;
}

void JpegPayload::setQ(boost::uint8_t q)
{
    boost::uint_t<Q_BIT_SIZE>::fast QMaxValue =
        boost::low_bits_mask_t<Q_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(q <= QMaxValue);

    m_q = q;
}

void JpegPayload::setWidth(boost::uint8_t width)
{
    boost::uint_t<WIDTH_BIT_SIZE>::fast WidthMaxValue =
        boost::low_bits_mask_t<WIDTH_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(width <= WidthMaxValue);

    m_width = width;
}

void JpegPayload::setHeight(boost::uint8_t height)
{
    boost::uint_t<HEIGHT_BIT_SIZE>::fast HeightMaxValue =
        boost::low_bits_mask_t<HEIGHT_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(height <= HeightMaxValue);

    m_height = height;
}

void JpegPayload::setRestartInterval(boost::uint16_t restart_interval)
{
    boost::uint_t<RESTART_INTERVAL_BIT_SIZE>::fast RestartIntervalMaxValue =
        boost::low_bits_mask_t<RESTART_INTERVAL_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(restart_interval <= RestartIntervalMaxValue);

    m_restart_interval = restart_interval;
}

void JpegPayload::setF(bool f)
{
    m_f = f;
}

void JpegPayload::setL(bool l)
{
    m_l = l;
}

void JpegPayload::setRestartCount(boost::uint16_t restart_count)
{
    boost::uint_t<RESTART_COUNT_BIT_SIZE>::fast RestartCountMaxValue =
        boost::low_bits_mask_t<RESTART_COUNT_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(restart_count <= RestartCountMaxValue);

    m_restart_count = restart_count;
}

void JpegPayload::setMbz(boost::uint8_t mbz)
{
    boost::uint_t<MBZ_BIT_SIZE>::fast MbzMaxValue =
        boost::low_bits_mask_t<MBZ_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(mbz <= MbzMaxValue);

    m_mbz = mbz;
}

void JpegPayload::setPrecision(boost::uint8_t precision)
{
    boost::uint_t<PRECISION_BIT_SIZE>::fast PrecisionMaxValue =
        boost::low_bits_mask_t<PRECISION_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(precision <= PrecisionMaxValue);

    m_precision = precision;
}

void JpegPayload::setLength(boost::uint16_t length)
{
    boost::uint_t<LENGTH_BIT_SIZE>::fast LengthMaxValue =
        boost::low_bits_mask_t<LENGTH_BIT_SIZE>::sig_bits;
    BOOST_VERIFY(length <= LengthMaxValue);

    m_length = length;
}

void JpegPayload::setQTable(unsigned char* q_table)
{
    m_q_table = q_table;
}

void JpegPayload::setRestartMarkerHeader(bool flag)
{
    m_restart_marker_header_is_existed = flag;
}

void JpegPayload::setQuantizationTableHeader(bool flag)
{
    m_quantization_table_header_is_existed = flag;
}

void JpegPayload::setScanData(const unsigned char* data, unsigned int length)
{
    m_scan_data_buf_length = length;
    
    m_scan_data_buf = new unsigned char[length];
    memcpy(m_scan_data_buf, data, length);
}

int JpegPayload::getHeaderSize()
{
    int bit_members_byte_size = (TYPE_SPECIFIC_BIT_SIZE+
                                 FRAGMENT_OFFSET_BIT_SIZE + 
                                 TYPE_BIT_SIZE +
                                 Q_BIT_SIZE +
                                 WIDTH_BIT_SIZE +
                                 HEIGHT_BIT_SIZE) / 8;

    if (m_restart_marker_header_is_existed)
    {
        bit_members_byte_size += (RESTART_INTERVAL_BIT_SIZE +
                                 F_BIT_SIZE +
                                 L_BIT_SIZE +
                                 RESTART_COUNT_BIT_SIZE) / 8;
    }

    if (m_quantization_table_header_is_existed)
    {
        bit_members_byte_size += (MBZ_BIT_SIZE +
                                 PRECISION_BIT_SIZE +
                                 LENGTH_BIT_SIZE) / 8;
    }

    return bit_members_byte_size + m_length;
}

void JpegPayload::marshallHeader()
{
    int header_size = getHeaderSize();
    BOOST_VERIFY(header_size >= 0);

    m_header_buf = new unsigned char[header_size];

    marshallMainHeader();

    if (m_restart_marker_header_is_existed)
    {
        marshallRestartMarkerHeader();
    }
    if (m_quantization_table_header_is_existed)
    {
        marshallQuantizationTableHeader();
    }
}

void JpegPayload::marshallMainHeader()
{
    {
        boost::uint32_t chunk = 0;

        boost::uint32_t type_specific = m_type_specific;
        type_specific = boost::endian::native_to_big(type_specific);

        chunk |= (type_specific <<
            util::toLeastSignificantBitFirst(TYPE_SPECIFIC_BIT_INDEX,
            TYPE_SPECIFIC_BIT_SIZE,
            sizeof(chunk)));

        boost::uint32_t fragment_offset = m_fragment_offset;
        fragment_offset = boost::endian::native_to_big(fragment_offset);

        chunk |= (m_fragment_offset <<
            util::toLeastSignificantBitFirst(FRAGMENT_OFFSET_BIT_INDEX,
            FRAGMENT_OFFSET_BIT_SIZE,
            sizeof(chunk)));

        int chunk_size = sizeof(chunk);
        std::memcpy(m_header_buf + m_header_buf_size, &chunk, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        int chunk_size = sizeof(m_type);
        std::memcpy(m_header_buf + m_header_buf_size, &m_type, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        int chunk_size = sizeof(m_q);
        std::memcpy(m_header_buf + m_header_buf_size, &m_q, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        int chunk_size = sizeof(m_width);
        std::memcpy(m_header_buf + m_header_buf_size, &m_width, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        int chunk_size = sizeof(m_height);
        std::memcpy(m_header_buf + m_header_buf_size, &m_height, chunk_size);
        m_header_buf_size += chunk_size;
    }
}

void JpegPayload::marshallRestartMarkerHeader()
{
    {
        m_restart_interval = boost::endian::native_to_big(m_restart_interval);

        int chunk_size = sizeof(m_restart_interval);
        std::memcpy(m_header_buf + m_header_buf_size, &m_restart_interval,
                    chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        boost::uint16_t chunk = 0;

        boost::uint16_t f = m_f;
        f = boost::endian::native_to_big(f);

        chunk |= (f << util::toLeastSignificantBitFirst(F_BIT_SIZE,
                                                        F_BIT_INDEX,
                                                        sizeof(chunk)));

        boost::uint16_t l = m_l;
        l = boost::endian::native_to_big(l);

        chunk |= (l << util::toLeastSignificantBitFirst(L_BIT_SIZE,
                                                        L_BIT_INDEX,
                                                        sizeof(chunk)));

        boost::uint16_t restart_count = m_restart_count;
        restart_count = boost::endian::native_to_big(restart_count);

        chunk |= (restart_count <<
            util::toLeastSignificantBitFirst(RESTART_COUNT_BIT_SIZE,
                                             RESTART_COUNT_BIT_INDEX,
                                             sizeof(chunk)));

        int chunk_size = sizeof(chunk);
        std::memcpy(m_header_buf + m_header_buf_size, &chunk, chunk_size);
        m_header_buf_size += chunk_size;
    }
}

void JpegPayload::marshallQuantizationTableHeader()
{
    {
        int chunk_size = sizeof(m_mbz);
        std::memcpy(m_header_buf + m_header_buf_size, &m_mbz, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        int chunk_size = sizeof(m_precision);
        std::memcpy(m_header_buf + m_header_buf_size, &m_precision, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        boost::uint16_t length = boost::endian::native_to_big(m_length);

        int chunk_size = sizeof(length);
        std::memcpy(m_header_buf + m_header_buf_size, &length, chunk_size);
        m_header_buf_size += chunk_size;
    }

    {
        std::memcpy(m_header_buf + m_header_buf_size, m_q_table, m_length);
        m_header_buf_size += m_length;
    }
}

} // namespace jpeg
} // namespace payload
} // namespace rtp
} // namespace rtsp_server
