#include "rtp/packet_creator.h"

#include <ctime>
#include <iostream>

#include <boost/assert.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace {

void random(unsigned int begin, unsigned int end,
            unsigned int& random_number)
{
    static boost::random::mt19937 gen(static_cast<boost::uint32_t>(time(0)));

    boost::random::uniform_int_distribution<unsigned int> dist(begin, end);
    random_number = dist(gen);
}

inline unsigned int getSequenceNumberMaxValue()
{
    typedef rtsp_server::rtp::Header RtpHeader;

    static const boost::uint_t<RtpHeader::SEQUENCE_NUMBER_BIT_SIZE>::fast
        SEQUENCE_NUMBER_MAX_VALUE = 
            boost::low_bits_mask_t<RtpHeader::SEQUENCE_NUMBER_BIT_SIZE>::sig_bits;

    return SEQUENCE_NUMBER_MAX_VALUE;
}

unsigned int incrementSequenceNumber(unsigned int sequence_number)
{
    return ++sequence_number;
}

inline unsigned int getTimeStampMaxValue()
{
    typedef rtsp_server::rtp::Header RtpHeader;

    static const boost::uint_t<RtpHeader::TIMESTAMP_BIT_SIZE>::fast
        TIME_STAMP_MAX_VALUE =
            boost::low_bits_mask_t<RtpHeader::TIMESTAMP_BIT_SIZE>::sig_bits;

    return TIME_STAMP_MAX_VALUE;
}

unsigned int incrementTimeStamp(unsigned int time_stamp,
                                unsigned short frame_rate)
{
    BOOST_ASSERT(frame_rate != 0);
    typedef rtsp_server::rtp::Header RtpHeader;

    return time_stamp += (RtpHeader::TIME_STAMP_HERTZ_UNIT / frame_rate);
}

inline unsigned int getSsrcMaxValue()
{
    typedef rtsp_server::rtp::Header RtpHeader;

    static const boost::uint_t<RtpHeader::SSRC_BIT_SIZE>::fast
        SSRC_MAX_VALUE =
            boost::low_bits_mask_t<RtpHeader::SSRC_BIT_SIZE>::sig_bits;

    return SSRC_MAX_VALUE;
}

unsigned int generateSsrcExceptListValue(std::list<unsigned int> ssrc_list)
{
    for (;;)
    {
        unsigned int ssrc_number = 0;
        random(0, getSsrcMaxValue(), ssrc_number);

        // check if unique number
        std::list<unsigned int>::iterator it = 
            std::find(ssrc_list.begin(), ssrc_list.end(), ssrc_number);
        if (it == ssrc_list.end())
        {
            return ssrc_number;
        }
    }
}

} // namespace

namespace rtsp_server {
namespace rtp {

PacketCreator::PacketCreator()
    : m_sequence_number(0), m_time_stamp(0), m_frame_rate(0)
{
    m_ssrc = generateSsrcExceptListValue(m_ssrc_list);
    m_ssrc_list.push_back(m_ssrc);

    random(0, getTimeStampMaxValue(), m_time_stamp);
    random(0, getSequenceNumberMaxValue(), m_sequence_number);
}

void PacketCreator::createHeader(bool first_fragment, bool last_fragment,
                                 unsigned char*& rtp_header,
                                 std::size_t& rtp_header_size)
{
    bool marker = false;

    if (first_fragment)
    {
        m_time_stamp = incrementTimeStamp(m_time_stamp, m_frame_rate);
    }
    if (last_fragment)
    {
        marker = true;
    }

    rtp::Header header;
    header.setVersion(rtp::Header::VERSION);
    header.setPadding(false);
    header.setExtension(false);
    header.setCsrcCount(0);
    header.setMarker(marker);
    header.setPayloadType(rtp::Header::JPEG_PAYLOAD);

    m_sequence_number = incrementSequenceNumber(m_sequence_number);
    header.setSequenceNumber(static_cast<boost::uint16_t>(m_sequence_number));

    header.setTimeStamp(static_cast<boost::uint32_t>(m_time_stamp));
    header.setSsrc(m_ssrc);

    rtp_header = header.marshall(rtp_header_size);
}

void PacketCreator::createData(payload::JpegPayload& jpeg_payload,
                unsigned char*& rtp_data,
                std::size_t& rtp_data_size)
{
    rtp_data = jpeg_payload.marshall(rtp_data_size);
}

} // namespace rtp
} // namespace rtsp_server
