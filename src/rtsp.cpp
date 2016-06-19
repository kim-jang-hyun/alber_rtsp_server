#include "rtsp_server.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

int main()
{
    using namespace rtsp_server;

    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

    RtspServer rtsp_server(RtspServer::DEFAULT_RTSP_SESSION_PORT);
    rtsp_server.run();

    return 0;
}