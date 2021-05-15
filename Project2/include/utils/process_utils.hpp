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
#include "errors.hpp"
#include "../data_structures/bloom_filter.hpp"
#include "../data_structures/indices.hpp"


namespace process_utils
{
    namespace travel_monitor
    {
        void _create_pipe(char** named_pipe, const char* type, const size_t & id);
        void create_pipes(structures::CommunicationPipes* pipes, const uint16_t & num_monitors);

        void _free_and_delete_pipe(const char* named_pipe);
        void free_and_delete_pipes(structures::CommunicationPipes* pipes, const uint16_t & num_monitors);
        
        void open_all_pipes(const structures::CommunicationPipes* pipes, int comm_fds[], const mode_t & input_perms, int data_fds[], const mode_t & output_perms, const uint16_t & num_monitors);
        void close_all_pipes(const int comm_fds[], const int data_fds[], const uint16_t & num_monitors);
        
        void _create_monitor(pid_t monitor_pids[], structures::CommunicationPipes* pipes, const size_t & position);
        void create_monitors(pid_t monitor_pids[], structures::CommunicationPipes* pipes, const u_int16_t & num_monitors);
        
        void kill_minitors_and_wait(pid_t monitor_pids[], travelMonitorIndex* tm_index);
        void cleanup(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, pid_t* monitor_pids);
        
        int ready_fd(struct pollfd fdarr[], size_t num_fds);
        int dead_monitor(pid_t* monitor_pids, const pid_t & terminated_process, const size_t & num_monitors);
    }

    namespace monitor
    {
        void parse_countries(MonitorIndex* m_index, const std::string & root_dir, ErrorHandler & handler);
    }
}





#endif