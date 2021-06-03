#include <chrono>
#include <thread>

#include "../../include/utils/utils.hpp"
#include "../../include/utils/network_utils.hpp"


void network_utils::init_address(struct sockaddr_in* address, const int & port, struct hostent* foundhost)
{
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    memcpy(&address->sin_addr, foundhost->h_addr, foundhost->h_length);
}


int network_utils::create_socket_and_connect(struct sockaddr_in* address)
{
    int new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket < 0)
        utils::perror_exit("socket() @ network_utils::create_socket_and_connect()");

    while (connect(new_socket, (struct sockaddr *) address, sizeof(*address)) < 0)
    {
        errno = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return new_socket;
}


int network_utils::create_server_socket(struct sockaddr_in* address, const int & max_backlog_queue_connections)
{
    int new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket < 0)
        utils::perror_exit("socket() @ network_utils::create_server_socket()");

    int on = 1; 
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
        utils::perror_exit("setsockopt() @ network_utils::create_server_socket()");

    if (bind(new_socket, (struct sockaddr *) address, sizeof(*address)) < 0)
        utils::perror_exit("bind() @ network_utils::create_server_socket()");

    if (listen(new_socket, max_backlog_queue_connections) < 0)
        utils::perror_exit("listen() @ network_utils::create_server_socket()");

    return new_socket;
}


int network_utils::accept_connection(const int & server_socket)
{
    struct sockaddr_in client_address;
	socklen_t clientlen;
    return accept(server_socket, (struct sockaddr *) &client_address, &clientlen);
}
