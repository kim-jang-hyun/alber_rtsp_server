#ifndef __RTSP_SERVER_RTSP_SESSION_H__
#define __RTSP_SERVER_RTSP_SESSION_H__

#include "network/tcp_service_handler_observer.h"

#include <vector>
#include <boost/shared_ptr.hpp>

#include "network/tcp_service_handler.h"
#include "rtsp/parser/parser_manager.h"
#include "session_observer.h"
#include "task/worker.h"
#include "util/unique_id.h"

namespace rtsp_server {
namespace rtsp {

class Session : public network::TcpServiceHandlerObserver
{
public:
    Session();

    void init(parser::ParserManagerPtr rtsp_parser_manager,
              task::WorkerPtr worker);
    
    void sendRtspPacket(unsigned char* data, std::size_t size);

    // derived from network::TcpServiceHandlerObserver - begin
    virtual void onRead(unsigned char* data, std::size_t size);
    virtual void onWrite(unsigned char* data, std::size_t size);
    virtual void onClose();
    // derived from network::TcpServiceHandlerObserver - end

    // property funtion - begin
    void setServiceHandler(
        network::TcpServiceHandlerPtr service_handler);
    void setObserver(SessionObserver* observer) { m_observer = observer; }
    void setSessionId(const util::unique_id& id) { m_session_id = id; }

    util::unique_id getSessonId() const { return m_session_id; }

    std::string getLocalAddress() const;
    std::string getRemoteAddress() const;
    // property funtion - end

private:
    std::vector<unsigned char> m_rtsp_message;
    parser::ParserManagerPtr m_rtsp_parser_manager;

    task::WorkerPtr m_worker;
    network::TcpServiceHandlerPtr m_service_handler;

    SessionObserver* m_observer;

    util::unique_id m_session_id;
};
typedef boost::shared_ptr<Session> SessionPtr;

} // namespace rtsp
} // namespace rtsp_server

#endif // __RTSP_SERVER_RTSP_SESSION_H__
