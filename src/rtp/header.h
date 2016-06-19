#ifndef __RTSP_SERVER_RTP_HEADER_H__
#define __RTSP_SERVER_RTP_HEADER_H__

#include <vector>
#include <boost/cstdint.hpp>

namespace rtsp_server {
namespace rtp {

class Header
{
public:
    static const int VERSION = 2;
    static const int JPEG_PAYLOAD = 26;
    static const int TIME_STAMP_HERTZ_UNIT = 90000; // Hz

    enum BitSize { // 8bit - octet
                   VERSION_BIT_SIZE = 2, PADDING_BIT_SIZE = 1,
                   EXTENSION_BIT_SIZE = 1, CSRC_COUNT_BIT_SIZE = 4,
                   // 8bit - octet
                   MARKER_BIT_SIZE = 1, PAYLOAD_TYPE_BIT_SIZE = 7,
                   // octets
                   SEQUENCE_NUMBER_BIT_SIZE = 16,
                   TIMESTAMP_BIT_SIZE = 32,
                   SSRC_BIT_SIZE = 32, };


    Header();
    ~Header();

    unsigned char* marshall(std::size_t& buf_size);
    size_t getBufferSize() const { return m_buffer_size; }

    // property funtion - begin
    void setVersion(unsigned char version);
    void setPadding(bool set);
    void setExtension(bool set);
    void setCsrcCount(unsigned char csrc_count);
    void setMarker(bool set);
    void setPayloadType(unsigned char payload_type);
    void setSequenceNumber(boost::uint16_t sequence_number);
    void setTimeStamp(boost::uint32_t timestamp);
    void setSsrc(boost::uint32_t ssrc);
    void setCsrcList(const std::vector<boost::uint32_t>& csrc_list);
    // property funtion - end

private:
    int getHeaderSize();

private:
    unsigned char* m_buffer;
    size_t m_buffer_size;

    unsigned char m_version; // 2 bit
    bool m_padding; // 1 bit
    bool m_extension; // 1 bit
    unsigned char m_csrc_count; // 4 bit

    bool m_marker; // 1 bit
    unsigned char m_payload_type; // 7 bit

    boost::uint16_t m_sequence_number; // 16 bit
    boost::uint32_t m_timestamp; // 32 bit
    boost::uint32_t m_ssrc; // 32 bit

    typedef std::vector<boost::uint32_t> CsrcVector;
    CsrcVector m_csrc_list;
};

} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_HEADER_H__
