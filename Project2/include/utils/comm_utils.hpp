#ifndef _COMM_UTILS
#define _COMM_UTILS

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>

#include "structures.hpp"


typedef enum communicationID
{
    ACK = 0,
    REJECT = 1,
    SEND_COUNTRY = 2,
    ACK_COUNTRY = 3
} communicationID;


namespace process_utils
{
    namespace travel_monitor
    {
        void _create_pipe(char** named_pipe, const char* type, const size_t & id);
        void create_pipes(structures::commPipes comm_pipes[], const uint16_t & num_monitors);
        void _free_and_delete_pipe(const char* named_pipe);
        void free_and_delete_pipes(structures::commPipes comm_pipes[], const uint16_t & num_monitors);
        void create_monitors(pid_t monitor_pids[], structures::commPipes comm_pipes[], const u_int16_t & num_monitors);

        void send_args(const structures::commPipes comm_pipes[], const structures::Input & input);
        void assign_countries(const structures::commPipes comm_pipes[], const structures::Input & input);
    }

    namespace monitor
    {
        void init_args(const structures::commPipes & comm_pipes, structures::Input & input);
        // void write_to_pipe(int fd, char buf[], const uint64_t & buffer_size);
    }

    void _wait_ack(const int & fd);
    void _write_and_wait_ack(const int & comm_fd, const int & data_fd, const uint8_t & msg_id, const size_t & bytes_out);
    void _send_message(const structures::commPipes & comm_pipes, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size);
}





#endif