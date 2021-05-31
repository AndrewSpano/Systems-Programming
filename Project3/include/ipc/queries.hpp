#ifndef _QUERIES
#define _QUERIES

#include <iostream>

#include "../data_structures/indices.hpp"
#include "../utils/structures.hpp"
#include "../utils/errors.hpp"
#include "../utils/date.hpp"


namespace ipc
{
    namespace travel_monitor
    {
        namespace queries
        {
            void travel_request(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, structures::TRData & tr_data, ErrorHandler & handler, bool & was_accepted);
            void travel_stats(travelMonitorIndex* tm_index, const structures::TSData & ts_data, ErrorHandler & handler);
            void add_vaccination_records(travelMonitorIndex* tm_index, pid_t monitor_pids[], structures::CommunicationPipes* pipes, const std::string & country, ErrorHandler & handler);
            void search_vaccination_status(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, const std::string & id, ErrorHandler & handler);
            void handle_sigchld(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, pid_t* monitor_pids);
        }
    }

    namespace monitor
    {
        namespace queries
        {
            void travel_request(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[]);
            void add_vaccination_records(MonitorIndex* m_index, const structures::CommunicationPipes* pipes, ErrorHandler & handler);
            void search_vaccination_status(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[]);
        }
    }
}

#endif