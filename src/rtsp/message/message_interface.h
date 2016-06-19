#ifndef __RTSP_SERVER_RTSP_MESSAGE_MESSAGE_INTERFACE_H__
#define __RTSP_SERVER_RTSP_MESSAGE_MESSAGE_INTERFACE_H__

#include <boost/shared_ptr.hpp>

namespace rtsp_server {
namespace rtsp {
namespace message {

class MessageInterface
{
public:
    enum Type {
                REQUEST_METHOD_OPTIONS,
                REQUEST_METHOD_DESCRIBE,
                REQUEST_METHOD_SETUP,
                REQUEST_METHOD_PLAY,
                REQUEST_METHOD_TEARDOWN,
                RESPONSE_METHOHD_OPTIONS,
                RESPONSE_METHOD_DESCRIBE,
                RESPONSE_METHOD_SETUP,
                RESPONSE_METHOD_PLAY,
                RESPONSE_METHOD_TEARDOWN,
                NONE,
              };

    explicit MessageInterface(Type type) : m_type(type) {}
    virtual ~MessageInterface() {}

    Type getType() const { return m_type; }

    bool isRequestMessage()
    {
        if (m_type == REQUEST_METHOD_OPTIONS  ||
            m_type == REQUEST_METHOD_DESCRIBE ||
            m_type == REQUEST_METHOD_SETUP    ||
            m_type == REQUEST_METHOD_PLAY     ||
            m_type == REQUEST_METHOD_TEARDOWN)
        {
            return true;
        }

        return false;
    }

private:
    Type m_type;
};

typedef boost::shared_ptr<MessageInterface> MessageInterfacePtr;

} // namespace message
} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_MESSAGE_MESSAGE_H__
