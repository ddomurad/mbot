#ifndef ROVER_TCP_SERVER_H
#define ROVER_TCP_SERVER_H

#include "mbot_defs.h"

namespace mbot {

#define MAX_WAITING_CONNECTION 3

class TcpServer
{
private:
    int fd_listener;
    bool is_closing;
public:
    TcpServer();
    ~TcpServer();

    result start(uint16_t port);
    int accept();
    void stop();    
};
}

#endif //ROVER_VIDEO_SERVER_H
