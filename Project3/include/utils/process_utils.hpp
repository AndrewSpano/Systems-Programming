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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "errors.hpp"
#include "structures.hpp"
#include "../data_structures/indices.hpp"
#include "../data_structures/bloom_filter.hpp"


namespace process_utils
{
    namespace travel_monitor
    {
        void enumerate_countries(travelMonitorIndex* tm_index);

        void create_addresses(structures::NetworkCommunication* network_info, const uint16_t & num_monitors);
        
        void create_monitors(pid_t monitor_pids[], structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index);

        void create_connections(structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index);
        
        void close_connections(structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index);
    }

    namespace monitor
    {
        void establish_connection(structures::NetworkCommunication & network_info);

        void close_connection(structures::NetworkCommunication & network_info);

        void parse_countries(MonitorIndex* m_index, const std::string & root_dir, ErrorHandler & handler);
    }
}





#endif