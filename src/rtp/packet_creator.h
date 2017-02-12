#ifndef __RTSP_SERVER_RTP_PACKET_CREATOR_H__
#define __RTSP_SERVER_RTP_PACKET_CREATOR_H__

#include <list>
#include <boost/shared_ptr.hpp>

namespace rtsp_server {
namespace rtp {

struct RtpPacket
{
    RtpPacket() : rtp_header(NULL), rtp_header_size(0),
                  rtp_data(NULL), rtp_data_size(0) {}

    unsigned char* rtp_header;
    std::size_t rtp_header_size;

    unsigned char* rtp_data;
    std::size_t rtp_data_size;
};

class PacketCreator
{
public:
    PacketCreator();

    void setFrameRate(unsigned short frame_rate) { m_frame_rate = frame_rate; }

    void makePacket(bool first_fragment, bool last_fragment,
                    int payload_type,
                    unsigned char* payload_buf, size_t payload_buf_size,
                    RtpPacket& rtp_packet);

private:
    void makeHeader(bool first_fragment, bool last_fragment, int payload_type,
                    unsigned char*& rtp_header, std::size_t& rtp_header_size);
    void makeData(unsigned char* payload_buf, std::size_t payload_buf_size,
                  unsigned char*& rtp_data, std::size_t& rtp_data_size);
private:
    unsigned int m_sequence_number;
    unsigned int m_time_stamp;

    unsigned short m_frame_rate;

    unsigned int m_ssrc;
    std::list<unsigned int> m_ssrc_list;
};
typedef boost::shared_ptr<PacketCreator> PacketCreatorPtr;

} // namespace rtp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTP_PACKET_CREATOR_H__
