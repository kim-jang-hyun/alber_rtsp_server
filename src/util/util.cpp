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

} // namespace util
} // namespace rtsp_server
