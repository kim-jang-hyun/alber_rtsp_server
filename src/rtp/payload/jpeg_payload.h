#ifndef __RTSP_SERVER_RTP_PAYLOAD_JPEG_PAYLOAD_H__
#define __RTSP_SERVER_RTP_PAYLOAD_JPEG_PAYLOAD_H__

#include <boost/cstdint.hpp>

namespace rtsp_server {
namespace rtp {
namespace payload {

class JpegPayload
{
public:
    enum BitSize {
        // main header
        TYPE_SPECIFIC_BIT_SIZE = 8,
        FRAGMENT_OFFSET_BIT_SIZE = 24,
        TYPE_BIT_SIZE = 8,
        Q_BIT_SIZE = 8,
        WIDTH_BIT_SIZE = 8,
        HEIGHT_BIT_SIZE = 8,

        // restart marker header
        RESTART_INTERVAL_BIT_SIZE = 16,
        F_BIT_SIZE = 1,
        L_BIT_SIZE = 1,
        RESTART_COUNT_BIT_SIZE = 14,

        // quantization table header
        MBZ_BIT_SIZE = 8,
        PRECISION_BIT_SIZE = 8,
        LENGTH_BIT_SIZE = 16,
    };

    JpegPayload();
    ~JpegPayload();

    unsigned char* marshall(std::size_t& payload_buf_size);

    std::size_t getHeaderBufSize() const { return m_header_buf_size; }
    std::size_t getDataSize() const { return m_scan_data_buf_length; }

    // property funtion - begin
    void setTypeSpecific(boost::uint8_t type_specific);
    void setFragmentOffset(boost::uint32_t fragment_offset);
    void setType(boost::uint8_t type);
    void setQ(boost::uint8_t q);
    void setWidth(boost::uint8_t width);
    void setHeight(boost::uint8_t height);

    void setRestartInterval(boost::uint16_t restart_interval);
    void setF(bool f);
    void setL(bool l);
    void setRestartCount(boost::uint16_t restart_count);

    void setMbz(boost::uint8_t mbz);
    void setPrecision(boost::uint8_t precision);
    void setLength(boost::uint16_t length);
    void setQTable(unsigned char* q_table);

    void setRestartMarkerHeader(bool flag);
    void setQuantizationTableHeader(bool flag);

    void setScanData(unsigned char* data, unsigned int length);
    // property funtion - end

private:
    int getHeaderSize();

    void marshallHeader();
    void marshallMainHeader();
    void marshallRestartMarkerHeader();
    void marshallQuantizationTableHeader();

private:
    // header - begin
    boost::uint8_t m_type_specific; // 8bit
    boost::uint32_t m_fragment_offset; // 24bit
    boost::uint8_t m_type; // 8bit
    boost::uint8_t m_q; // 8bit
    boost::uint8_t m_width; // 8bit
    boost::uint8_t m_height; // 8bit

    boost::uint16_t m_restart_interval; // 16bit
    bool m_f; // 1bit
    bool m_l; // 1bit
    boost::uint16_t m_restart_count; // 14bit

    boost::uint8_t m_mbz; // 8bit
    boost::uint8_t m_precision; // 8bit
    boost::uint16_t m_length; // 16bit
    unsigned char* m_q_table;
    // header - end

    bool m_restart_marker_header_is_existed;
    bool m_quantization_table_header_is_existed;

    unsigned char* m_header_buf;
    std::size_t m_header_buf_size;

    unsigned char* m_scan_data_buf;
    std::size_t m_scan_data_buf_length;
};

} // namespace payload
} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PAYLOAD_JPEG_PAYLOAD_H__
