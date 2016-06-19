#ifndef __RTSP_SERVER_RTP_PAYLOAD_JPEG_FRAME_PARSER_H__
#define __RTSP_SERVER_RTP_PAYLOAD_JPEG_FRAME_PARSER_H__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace rtsp_server {
namespace rtp {
namespace payload {

class JpegFrameParser 
{ 
public: 
    JpegFrameParser(); 
    virtual ~JpegFrameParser(); 

    bool parse(const unsigned char* src, const std::size_t src_size);

    // property funtion - begin
    unsigned char width() const;
    unsigned char height() const;
    unsigned char type() const;
    unsigned char typeSpecific() const;
    unsigned char precision() const;
    unsigned char qFactor() const;
    unsigned short restartInterval() const;
    unsigned char const* quantizationTables(unsigned short& length) const;
    unsigned char const* scandata(unsigned int& length) const;
    // property funtion - end

private:
    void parseApp0(const unsigned char*& data, std::size_t& offset);
    void parseDqt(const unsigned char*& data, std::size_t& offset);
    void parseSof(const unsigned char*& data, std::size_t& offset);
    void parseSos(const unsigned char*& data, std::size_t& offset);
    void parseDri(const unsigned char*& data, std::size_t& offset);

private:
    struct Avi1Tag
    {
        unsigned short header_length;
        unsigned char interleaved_field;
    };
    Avi1Tag m_avi1_tag;

    struct QuantizationTable
    {
        unsigned short header_length;
        unsigned char precision;
        unsigned char table_number;
        unsigned char* table_data;
    };
    std::vector<QuantizationTable> m_quantization_table_vec;

    struct StartOfFrame
    {
        unsigned char dct_mode;

        unsigned short header_length;
        unsigned char precision;
        unsigned short height;
        unsigned short width;
        unsigned char component_count;

        struct Component
        {
            Component() : component_number(0), horizontal_sampling_factor(0),
                          vertical_sampling_factor(0),
                          quantization_table_number(0) {}

            unsigned char component_number;
            unsigned char horizontal_sampling_factor;
            unsigned char vertical_sampling_factor;
            unsigned char quantization_table_number;
        };
        std::vector<Component> component_vec;
    };
    StartOfFrame m_start_of_frame;

    struct StartOfScan
    {
        unsigned short header_length;
        unsigned char component_count;

        struct Component
        {
            unsigned char component_number;
            unsigned char dc_huffman_table_number;
            unsigned char ac_huffman_table_number;
        };
        std::vector<Component> component_vec;

        unsigned char spectral_selection_start;
        unsigned char spectral_selection_end;
        unsigned char successive_approximation_high;
        unsigned char successive_approximation_low;
    };
    StartOfScan m_start_of_scan;

    unsigned short m_restart_interval;
    unsigned char* m_scan_data;
    size_t m_scan_data_size;
};
typedef boost::shared_ptr<JpegFrameParser> JpegFrameParserPtr;

} // namespace rtsp_server
} // namespace rtp
} // namespace payload

#endif // __RTSP_SERVER_RTP_PAYLOAD_JPEG_FRAME_PARSER_H__
