#ifndef __RTSP_SERVER_UTIL_UTIL_H__
#define __RTSP_SERVER_UTIL_UTIL_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

#define NULL_SHARED_PTR(SHARED_PTR_TYPE) \
    boost::shared_ptr<SHARED_PTR_TYPE::element_type>()

namespace rtsp_server {
namespace util {

template <typename T>
inline void safe_delete(T*& obj)
{
    delete obj;
    obj = NULL;
}

template <typename T>
inline void safe_delete_array(T*& obj)
{
    delete[] obj;
    obj = NULL;
}

template <typename T>
inline void vector_clear(std::vector<T>& obj)
{
    std::vector<T> temp_obj;
    temp_obj.swap(obj);
}

template <typename T>
inline bool isExisted(const T& map, const typename T::key_type& key)
{
    T::const_iterator it = map.find(key);
    if (it == map.end())
    {
        return false;
    }
    return true;
}

template <typename T>
inline bool getMapValue(const T& map, const typename T::key_type& key,
    typename T::mapped_type& value)
{
    T::const_iterator it = map.find(key);
    if (it == map.end())
    {
        return false;
    }

    value = it->second;
    return true;
}

template <typename StringType, typename IntegerType>
inline StringType toString(IntegerType integer)
{
    try
    {
        return boost::lexical_cast<StringType>(integer);
    }
    catch(const boost::bad_lexical_cast& e)
    {
        std::cout << e.what() << std::endl;
    }

    return StringType();
}

template <typename StringType, typename IntegerType>
inline bool toInteger(const StringType& string, IntegerType& integer)
{
    try
    {
        integer = boost::lexical_cast<IntegerType>(string);
        return true;
    }
    catch(const boost::bad_lexical_cast& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return false;
}

template <typename T>
inline T truncateString(const T& source,
    const T& begin_str, const T& end_str = T())
{
    std::size_t begin_pos = source.find(begin_str);
    if (begin_pos == T::npos)
    {
        return T();
    }

    std::size_t end_pos = 0;
    if (end_str == T())
    {
        end_pos = source.length();
    }
    else
    {
        end_pos = source.find(end_str, begin_pos);
    }

    return source.substr(begin_pos, end_pos - begin_pos);
}

inline unsigned char getMost4Bit(unsigned char src)
{
    return ((unsigned char)0xF0 & src) >> 4;
}

inline unsigned char getLeast4Bit(unsigned char src)
{
    return ((unsigned char)0x0F & src);
}

inline void printLog(const boost::system::error_code& e)
{
    std::cout << e.value() << std::endl;
    std::cout << e.category().name() << std::endl;
    std::cout << e.default_error_condition().message() << std::endl;
    std::cout << e.message() << std::endl;
}

std::size_t toLeastSignificantBitFirst(std::size_t bit_index,
                                       std::size_t bit_size,
                                       std::size_t byte_size);

} // namespace util
} // namespace rtsp_server

#endif // __RTSP_SERVER_UTIL_UTIL_H__
