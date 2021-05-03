#ifndef _PROCESS_UTILS
#define _PROCESS_UTILS

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>

#include "structures.hpp"



namespace process_utils
{
    namespace travel_monitor
    {
        void _create_pipe(char** named_pipe, const char* type, const size_t & id);
        void create_pipes(structures::CommunicationPipes pipes[], const uint16_t & num_monitors);
        void _free_and_delete_pipe(const char* named_pipe);
        void free_and_delete_pipes(structures::CommunicationPipes pipes[], const uint16_t & num_monitors);
        void open_all_pipes(const structures::CommunicationPipes pipes[], int comm_fds[], const mode_t & comm_perms,
                            int data_fds[], const mode_t & data_perms, const uint16_t & num_monitors);
        void close_all_pipes(const int comm_fds[], const int data_fds[], const uint16_t & num_monitors);
        void create_monitors(pid_t monitor_pids[], structures::CommunicationPipes pipes[], const u_int16_t & num_monitors);
    }
}





#endif