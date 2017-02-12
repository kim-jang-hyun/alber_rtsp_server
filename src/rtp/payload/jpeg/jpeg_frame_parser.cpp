#include "rtp/payload/jpeg/jpeg_frame_parser.h" 

#include <boost/foreach.hpp>
#include <boost/endian/conversion.hpp>
#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

enum
{ 
    START_MARKER = 0xFF,
    SOI_MARKER = 0xD8,
    JFIF_MARKER = 0xE0,
    DQT_MARKER = 0xDB,

    SOF0_MARKER = 0xC0,
    SOF1_MARKER = 0xC1,
    SOF2_MARKER = 0xC2,
    SOF3_MARKER = 0xC3,
    SOF5_MARKER = 0xC5,
    SOF6_MARKER = 0xC6,
    SOF7_MARKER = 0xC7,
    SOF9_MARKER = 0xC9,
    SOFA_MARKER = 0xCA,
    SOFB_MARKER = 0xCB,
    SOFD_MARKER = 0xCD,
    SOFE_MARKER = 0xCE,
    SOFF_MARKER = 0xCF,

    DHT_MARKER = 0xC4,
    SOS_MARKER = 0xDA,
    DRI_MARKER = 0xDD,

    RST0_MARKER = 0xD0,
    RST1_MARKER = 0xD1,
    RST2_MARKER = 0xD2,
    RST3_MARKER = 0xD3,
    RST4_MARKER = 0xD4,
    RST5_MARKER = 0xD5,
    RST6_MARKER = 0xD6,
    RST7_MARKER = 0xD7,

    DNL_MARKER = 0xDC,
    EOI_MARKER = 0xD9,
    COM_MARKER = 0xFE,
};

} // namespace

namespace rtsp_server {
namespace rtp {
namespace payload {
namespace jpeg {

using util::readByte;

JpegFrameParser::JpegFrameParser()
    : m_restart_interval(0), m_scan_data(NULL), m_scan_data_size(0)
{

}

JpegFrameParser::~JpegFrameParser()
{
    util::safe_delete_array(m_scan_data);
    BOOST_FOREACH(QuantizationTable& qt_table, m_quantization_table_vec)
    {
        util::safe_delete_array(qt_table.table_data);
    }
}

bool JpegFrameParser::parse(const unsigned char* src, std::size_t src_size)
{
    std::size_t scan_data_begin_offset = 0;
    std::size_t scan_data_end_offset = 0;

    const unsigned char* data = src;
    std::size_t offset = 0;

    while (offset < src_size)
    {
        unsigned char one_byte_data = '0';
        readByte(data, offset, one_byte_data);

        if (one_byte_data == START_MARKER)
        {
            unsigned char marker = '0';
            readByte(data, offset, marker);

            switch (marker)
            {
            case SOI_MARKER:
                {
                    // start of image
                    break;
                }
            case JFIF_MARKER:
                {
                    parseApp0(data, offset);
                    break;
                }
            case DQT_MARKER:
                {
                    parseDqt(data, offset);
                    break;
                }
            case SOF0_MARKER:   m_start_of_frame.dct_mode = 0x00;
            case SOF1_MARKER:   m_start_of_frame.dct_mode = 0x01;
            case SOF2_MARKER:   m_start_of_frame.dct_mode = 0x02;
            case SOF3_MARKER:   m_start_of_frame.dct_mode = 0x03;
            case SOF5_MARKER:   m_start_of_frame.dct_mode = 0x05;
            case SOF6_MARKER:   m_start_of_frame.dct_mode = 0x06;
            case SOF7_MARKER:   m_start_of_frame.dct_mode = 0x07;
            case SOF9_MARKER:   m_start_of_frame.dct_mode = 0x09;
            case SOFA_MARKER:   m_start_of_frame.dct_mode = 0x0a;
            case SOFB_MARKER:   m_start_of_frame.dct_mode = 0x0b;
            case SOFD_MARKER:   m_start_of_frame.dct_mode = 0x0d;
            case SOFE_MARKER:   m_start_of_frame.dct_mode = 0x0e;
            case SOFF_MARKER:   m_start_of_frame.dct_mode = 0x0f;
                {
                    parseSof(data, offset);
                    break;
                }
            case DHT_MARKER:
                {
                    // define huffman table
                    break;
                }
            case SOS_MARKER:
                {
                    parseSos(data, offset);

                    scan_data_begin_offset = offset;
                    break;
                }
            case DRI_MARKER:
                {
                    parseDri(data, offset);
                    break;
                }
            case RST0_MARKER:
            case RST1_MARKER:
            case RST2_MARKER:
            case RST3_MARKER:
            case RST4_MARKER:
            case RST5_MARKER:
            case RST6_MARKER:
            case RST7_MARKER:
                {
                    break;
                }
            case DNL_MARKER:
                {
                    break;
                }
            case EOI_MARKER:
                {
                    // end of image
                    size_t eoi_marker_size = 2; //  0xFFD9
                    scan_data_end_offset = (offset - eoi_marker_size);
                    break;
                }
            case COM_MARKER:
                {
                    // comment
                    break;
                }
            }
        }
    }

    BOOST_ASSERT(scan_data_begin_offset != 0);
    BOOST_ASSERT(scan_data_end_offset != 0);

    m_scan_data_size = scan_data_end_offset - scan_data_begin_offset;
    m_scan_data = new unsigned char[m_scan_data_size];
    memcpy(m_scan_data, src + scan_data_begin_offset, m_scan_data_size);

    return true;
}

unsigned char JpegFrameParser::width() const
{
    return (unsigned char)(m_start_of_frame.width / 8);
}

unsigned char JpegFrameParser::height() const
{
    return (unsigned char)(m_start_of_frame.height / 8);
}

unsigned char JpegFrameParser::type() const
{
//    BOOST_ASSERT(m_start_of_frame.dct_mode == 0);
    BOOST_ASSERT(m_start_of_frame.component_count == 3);

    StartOfFrame::Component component1, component2, component3;
    for (size_t i = 0; i < m_start_of_frame.component_count; ++i)
    {
        const StartOfFrame::Component& component =
            m_start_of_frame.component_vec[i];

        if      (component.component_number == 0) component1 = component;
        else if (component.component_number == 1) component2 = component;
        else if (component.component_number == 2) component3 = component;
        else                                      BOOST_ASSERT(false);
    }

    unsigned char type = 0;
    if (component1.horizontal_sampling_factor == 2 &&
        component1.vertical_sampling_factor   == 1 &&
        component1.quantization_table_number  == 0 &&
        component2.horizontal_sampling_factor == 1 &&
        component2.vertical_sampling_factor   == 1 &&
        component2.quantization_table_number  == 1 &&
        component3.horizontal_sampling_factor == 1 &&
        component3.vertical_sampling_factor   == 1 &&
        component3.quantization_table_number  == 1)
    {
        if (restartInterval() != 0)
        {
            type += 64;
        }
    }
    else if (component1.horizontal_sampling_factor == 2 &&
             component1.vertical_sampling_factor   == 2 &&
             component1.quantization_table_number  == 0 &&
             component2.horizontal_sampling_factor == 1 &&
             component2.vertical_sampling_factor   == 1 &&
             component2.quantization_table_number  == 1 &&
             component3.horizontal_sampling_factor == 1 &&
             component3.vertical_sampling_factor   == 1 &&
             component3.quantization_table_number  == 1)
    {
        if (restartInterval() != 0)
        {
            type += 64;
        }
    }
    else
    {
        BOOST_ASSERT(false);
    }

    return type;
}

unsigned char JpegFrameParser::typeSpecific() const
{
    return 0;
}

unsigned char JpegFrameParser::precision() const
{
    return m_start_of_frame.precision;
}

unsigned char JpegFrameParser::qFactor() const
{
    return 85;
}

unsigned short JpegFrameParser::restartInterval() const
{
    return m_restart_interval;
}

unsigned char const* JpegFrameParser::quantizationTables(unsigned short& length) const
{
    length = 0;
    return NULL;
}

unsigned char const* JpegFrameParser::scandata(unsigned int& length) const
{
    length = m_scan_data_size;
    return m_scan_data;
}

void JpegFrameParser::parseApp0(const unsigned char*& data, std::size_t& offset)
{
    boost::uint16_t header_size = 0;
    {
        readByte(data, offset, header_size);
        boost::endian::big_to_native_inplace(header_size);
    }

    char jfif_id[5] = {0, };
    {
        readByte(data, offset, jfif_id);
    }

    boost::uint8_t interleaved_field = 0;
    {
        readByte(data, offset, interleaved_field);
    }

    m_avi1_tag.header_length = header_size;
    m_avi1_tag.interleaved_field = interleaved_field;
}

void JpegFrameParser::parseDqt(const unsigned char*& data, std::size_t& offset)
{
    boost::uint16_t quantization_table_header_size = 0;
    {
        readByte(data, offset, quantization_table_header_size);
        boost::endian::big_to_native_inplace(quantization_table_header_size);
    }

    boost::uint8_t quantization_table_precision = 0;
    boost::uint8_t quantization_table_number = 0;
    {
        boost::uint8_t chunk = 0;
        readByte(data, offset, chunk);

        quantization_table_precision = util::getMost4Bit(chunk);
        quantization_table_number = util::getLeast4Bit(chunk);
    }

    unsigned char* quantization_table = NULL;
    {
        if (quantization_table_precision == 0)
        {
            unsigned char q_table_array[64] = {0, };
            readByte(data, offset, q_table_array);

            quantization_table = new unsigned char[64];
            memcpy(quantization_table, q_table_array, sizeof(q_table_array));
        }
        else if (quantization_table_precision == 1)
        {
            unsigned char q_table_array[128] = {0, };
            readByte(data, offset, q_table_array);

            quantization_table = new unsigned char[128];
            memcpy(quantization_table, q_table_array, sizeof(q_table_array));
        }
        else
        {
            BOOST_ASSERT(false);
        }
    }

    QuantizationTable qt;
    qt.header_length = quantization_table_header_size;
    qt.precision = quantization_table_precision;
    qt.table_number = quantization_table_number;
    qt.table_data = quantization_table;

    m_quantization_table_vec.push_back(qt);
}

void JpegFrameParser::parseSof(const unsigned char*& data, std::size_t& offset)
{
    boost::uint16_t frame_header_size = 0;
    {
        readByte(data, offset, frame_header_size);
        boost::endian::big_to_native_inplace(frame_header_size);
    }

    boost::uint8_t frame_precision = 0;
    {
        readByte(data, offset, frame_precision);
    }

    boost::uint16_t height = 0;
    {
        readByte(data, offset, height);
        boost::endian::big_to_native_inplace(height);
    }

    boost::uint16_t width = 0;
    {
        readByte(data, offset, width);
        boost::endian::big_to_native_inplace(width);
    }

    boost::uint8_t component_count = 0;
    {
        readByte(data, offset, component_count);
    }

    std::vector<StartOfFrame::Component> component_vec;
    for (int i = 0; i < component_count; ++i)
    {
        boost::uint8_t component_number = 0;
        {
            readByte(data, offset, component_number);
        }

        boost::uint8_t horizontal_sample_factor = 0;
        boost::uint8_t vertical_sample_factor = 0;
        {
            boost::uint8_t chunk = 0;
            readByte(data, offset, chunk);

            horizontal_sample_factor = util::getMost4Bit(chunk);
            vertical_sample_factor = util::getLeast4Bit(chunk);
        }

        boost::uint8_t quantization_table_number = 0;
        {
            readByte(data, offset, quantization_table_number);
        }

        StartOfFrame::Component component;
        component.component_number = component_number;
        component.horizontal_sampling_factor = horizontal_sample_factor;
        component.vertical_sampling_factor = vertical_sample_factor;
        component.quantization_table_number = quantization_table_number;

        component_vec.push_back(component);
    }

    m_start_of_frame.header_length = frame_header_size;
    m_start_of_frame.precision = frame_precision;
    m_start_of_frame.height = height;
    m_start_of_frame.width = width;
    m_start_of_frame.component_count = component_count;
    m_start_of_frame.component_vec = component_vec;
}

void JpegFrameParser::parseSos(const unsigned char*& data, std::size_t& offset)
{
    boost::uint16_t header_size = 0;
    {
        readByte(data, offset, header_size);
        boost::endian::big_to_native_inplace(header_size);
    }

    boost::uint8_t component_count = 0;
    {
        readByte(data, offset, component_count);
    }

    std::vector<StartOfScan::Component> component_vec;
    for (int i = 0; i < component_count; ++i)
    {
        boost::uint8_t component_number = 0;
        {
            readByte(data, offset, component_number);
        }

        boost::uint8_t dc_huffman_table_number = 0;
        boost::uint8_t ac_huffman_table_number = 0;
        {
            boost::uint8_t chunk = 0;
            readByte(data, offset, chunk);

            dc_huffman_table_number = util::getMost4Bit(chunk);
            ac_huffman_table_number = util::getLeast4Bit(chunk);
        }

        StartOfScan::Component component;
        component.component_number = component_number;
        component.dc_huffman_table_number = dc_huffman_table_number;
        component.ac_huffman_table_number = ac_huffman_table_number;

        component_vec.push_back(component);
    }

    boost::uint8_t spectral_selection_start = 0;
    {
        readByte(data, offset, spectral_selection_start);
    }

    boost::uint8_t spectral_selection_end = 0;
    {
        readByte(data, offset, spectral_selection_end);
    }

    boost::uint8_t successive_approximation_high = 0;
    boost::uint8_t successive_approximation_low = 0;
    {
        boost::uint8_t chunk = 0;
        readByte(data, offset, chunk);

        successive_approximation_high = util::getMost4Bit(chunk);
        successive_approximation_low = util::getLeast4Bit(chunk);
    }

    m_start_of_scan.header_length = header_size;
    m_start_of_scan.component_count = component_count;
    m_start_of_scan.component_vec = component_vec;
    m_start_of_scan.spectral_selection_start = spectral_selection_start;
    m_start_of_scan.spectral_selection_end = spectral_selection_end;
    m_start_of_scan.successive_approximation_high = successive_approximation_high;
    m_start_of_scan.successive_approximation_low = successive_approximation_low;
}

void JpegFrameParser::parseDri(const unsigned char*& data, std::size_t& offset)
{
    boost::uint16_t restart_interval = 0;
    {
        readByte(data, offset, restart_interval);
        boost::endian::big_to_native_inplace(restart_interval);
    }

    m_restart_interval = restart_interval;
}

} // namespace jpeg
} // namespace payload
} // namespace rtp
} // namespace rtsp_server
