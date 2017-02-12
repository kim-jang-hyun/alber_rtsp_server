#include "util/util.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

namespace rtsp_server {
namespace util {

std::size_t toLeastSignificantBitFirst(std::size_t bit_index,
                                       std::size_t bit_size,
                                       std::size_t byte_size)
{
    std::size_t bit_posiion = (byte_size*8) - bit_index - bit_size;
    return static_cast<std::size_t>(bit_posiion);
}

unsigned char getValueFromBitRange(std::size_t bit_index, std::size_t bit_size,
                                   unsigned char chunk)
{ 
    std::size_t chunk_size = sizeof(chunk) * 8;
    unsigned char left_shift = chunk << bit_index;
    
    return left_shift >> (chunk_size - bit_size);
}

// uri : ex)rtsp://127.0.0.1/mjpeg_test.avi
bool splitFileName(std::string uri, std::string& file_name)
{
    std::size_t found = uri.find_last_of("/");
    if (found == std::string::npos)
    {
        return false;
    }

    file_name= uri.substr(found+1);

    return true;
}

} // namespace util
} // namespace rtsp_server
