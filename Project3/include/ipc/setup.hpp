#ifndef _SETUP
#define _SETUP


#include "../utils/structures.hpp"
#include "../data_structures/indices.hpp"
#include "../data_structures/bloom_filter.hpp"



namespace ipc
{
    namespace travel_monitor
    {
        namespace setup
        {
            // void _receive_bloom_filters(travelMonitorIndex* tm_index, const int & input_fd, const int & output_fd);
            // void receive_bloom_filters(travelMonitorIndex* tm_index, const structures::CommunicationPipes pipes[]);
        }
    }

    namespace monitor
    {
        namespace setup
        {
            // void send_bloom_filters(MonitorIndex* m_index, const structures::CommunicationPipes* pipes);
        }
    }
}


#endif