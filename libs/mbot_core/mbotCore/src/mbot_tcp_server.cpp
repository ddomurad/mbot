#include "inc/mbot_defs.h"
#include "inc/mbot_tcp_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace mbot;

TcpServer::TcpServer()
{
    fd_listener = 0;
    is_closing = false;
}

TcpServer::~TcpServer()
{
    stop();
}

result TcpServer::start(uint16_t port)
{
    int __yes = 1;
    struct sockaddr_in serveraddr;

    LOG_INFO("Starting tcp server on port: %d\n", port);

    R_ASSERT_ERRNO((fd_listener = socket(AF_INET, SOCK_STREAM, 0)) == -1);
    R_ASSERT_ERRNO(setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR, &__yes, sizeof(int)) == -1);

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(port);

    R_ASSERT_ERRNO(bind(fd_listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1);
    R_ASSERT_ERRNO(listen(fd_listener, MAX_WAITING_CONNECTION) == -1);

    return Success;
}

int TcpServer::accept()
{
    struct sockaddr_in clientaddr;
    uint32_t addrlen = sizeof(clientaddr);

    memset(&clientaddr, 0, addrlen);
    int fd_client = -1;

    R_BEGIN_ASSERT_ERRNO((fd_client = ::accept(fd_listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1);
    RX_END_ASSERT(-1);

    LOG_INFO("New connection from: %s", inet_ntoa(clientaddr.sin_addr));

    return fd_client;
}

void TcpServer::stop()
{
    if(fd_listener == 0)
        return;

    LOG_INFO("Stoping tcp server.");
    is_closing = true;
    close(fd_listener);
    fd_listener = 0;
    return ;
}
