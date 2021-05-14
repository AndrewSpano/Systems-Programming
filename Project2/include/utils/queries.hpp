#ifndef _QUERIES
#define _QUERIES

#include <iostream>
#include "../data_structures/indices.hpp"
#include "structures.hpp"
#include "errors.hpp"
#include "date.hpp"


namespace queries
{
    namespace travel_monitor
    {
        void travel_request(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, const structures::Input & input,
                            structures::TRData & tr_data, ErrorHandler & handler, bool & was_accepted);
        void travel_stats(travelMonitorIndex* tm_index, const structures::TSData & ts_data, ErrorHandler & handler);
    }

    namespace monitor
    {
        void travel_request(MonitorIndex* m_index, const int & input_fd, const int & output_fd, const structures::Input & input, char data[]);
    }
}


#endif