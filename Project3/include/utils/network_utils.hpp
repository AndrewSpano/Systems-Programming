#ifndef _NETWORK_UTILS
#define _NETWORK_UTILS

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "structures.hpp"


namespace network_utils
{
    void init_address(struct sockaddr_in* address, const int & port, struct hostent* foundhost);
    int create_socket_and_connect(struct sockaddr_in* address);
    int create_server_socket(struct sockaddr_in* address, const int & max_backlog_queue_connections);
    int accept_connection(const int & server_socket);
}





#endif