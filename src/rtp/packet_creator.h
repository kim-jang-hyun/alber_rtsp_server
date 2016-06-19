#ifndef __RTSP_SERVER_RTP_PACKET_CREATOR_H__
#define __RTSP_SERVER_RTP_PACKET_CREATOR_H__

#include <list>
#include <boost/shared_ptr.hpp>

#include "rtp/header.h"
#include "payload/jpeg_payload.h"

namespace rtsp_server {
namespace rtp {

class PacketCreator
{
public:
    PacketCreator();

    void createHeader(bool first_fragment, bool last_fragment,
                      unsigned char*& rtp_header, std::size_t& rtp_header_size);
    void createData(payload::JpegPayload& jpeg_payload,
                    unsigned char*& rtp_data,
                    std::size_t& rtp_data_size);

    void setFrameRate(unsigned short frame_rate) { m_frame_rate = frame_rate; }

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
