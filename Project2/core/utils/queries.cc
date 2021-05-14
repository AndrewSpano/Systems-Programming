#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/queries.hpp"
#include "../../include/utils/comm_utils.hpp"


void queries::travel_monitor::travel_request(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, const structures::Input & input,
                                             structures::TRData & tr_data, ErrorHandler & handler, bool & was_accepted)
{
    /* make sure that the from country exists in the database */
    int country_from_id = tm_index->country_id(tr_data.country_from);
    if (country_from_id == -1)
    {
        handler.status = UNKNOWN_COYNTRY;
        handler.invalid_value = tr_data.country_from;
        return;
    }

    /* make sure that the virus exists in the database */
    BFPair* bf_pair = tm_index->bloom_filters->get(tr_data.virus_name);
    if (bf_pair == NULL)
    {
        handler.status = UNKNOWN_VIRUS;
        handler.invalid_value = tr_data.virus_name;
        return;
    }

    /* check the bloom filter for the vaccination status of the citizen */
    if (!bf_pair->bloom_filter->is_probably_in(tr_data.citizen_id))
    {
        std::cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED" << std::endl;
        was_accepted = false;
        return;
    }

    /* the bloom filter was not able to give a deterministic approach, therefore ask the corresponding Monitor */
    char tr_data_as_byte_arr[256] = {0};
    size_t len = tr_data.to_str(tr_data_as_byte_arr);
    
    size_t monitor_with_from_country = tm_index->monitor_with_country(country_from_id);
    int input_fd = open(pipes[monitor_with_from_country].input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes[monitor_with_from_country].output, O_WRONLY);


    /* send the data of the query */
    comm_utils::_send_message(input_fd, output_fd, TRAVEL_REQUEST_SEND_DATA, tr_data_as_byte_arr, len, input.buffer_size);

    /* receive the response from the monitor */
    uint8_t msg_id = REJECT;
    char buf[128] = {0};
    comm_utils::_receive_message(input_fd, output_fd, msg_id, buf, len, input.buffer_size);

    /* close the file descriptors */
    close(input_fd);
    close(output_fd);

    /* print the corresponding message and return */
    switch (msg_id)
    {
        case TRAVEL_REQUEST_NOT_VACCINATED:
            std::cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED";
            was_accepted = false;
            break;
        case TRAVEL_REQUEST_NEED_VACCINATION:
            std::cout << "REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE";
            was_accepted = false;
            break;
        case TRAVEL_REQUEST_OK:
            std::cout << "REQUEST ACCEPTED - HAPPY TRAVELS";
            was_accepted = true;
            break;
        default:
            std::cout << "This message should have never been printed. In queries::travel_monitor::travel_request()";
            break;
    }
    std::cout << std::endl;
}



void queries::monitor::travel_request(MonitorIndex* m_index, const int & input_fd, const int & output_fd, const structures::Input & input, char data[])
{
    structures::TRData tr_data(data);
    Date* vaccination_date = m_index->virus_list->get_vaccination_date(tr_data.citizen_id, tr_data.virus_name);
    bool was_accepted = false;

    if (vaccination_date == NULL)
    {
        comm_utils::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NOT_VACCINATED, NULL, 0, input.buffer_size);
    }
    else if (!tr_data.date->is_within_6_months_in_the_past(vaccination_date))
    {
        comm_utils::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NEED_VACCINATION, NULL, 0, input.buffer_size);
    }
    else
    {
        comm_utils::_send_message(input_fd, output_fd, TRAVEL_REQUEST_OK, NULL, 0, input.buffer_size);
        was_accepted = true;
    }

    structures::TRQuery* query = new structures::TRQuery(tr_data.date, tr_data.country_from, tr_data.virus_name, was_accepted);
    m_index->logger->insert(query);

    delete tr_data.date;
}



void queries::travel_monitor::travel_stats(travelMonitorIndex* tm_index, const structures::TSData & ts_data, ErrorHandler & handler)
{
    size_t accepted = 0;
    size_t rejected = 0;

    size_t num_queries = tm_index->logger->query_list->get_size();
    if (num_queries > 0)
    {
        structures::TRQuery** queries = tm_index->logger->query_list->get_as_arr();

        for (size_t i = 0; i < num_queries; i++)
            if ((ts_data.country == "" || ts_data.country == queries[i]->country) && ts_data.virus_name == queries[i]->virus_name &&
                *ts_data.date_1 <= *queries[i]->date && *queries[i]->date <= ts_data.date_2)
                (queries[i]->was_accepted) ? accepted++
                                           : rejected++;    

        delete[] queries;
    }

    std::cout << "TOTAL REQUESTS " << (accepted + rejected) << std::endl;
    std::cout << "ACCEPTED " << accepted << std::endl;
    std::cout << "REJECTED " << rejected << std::endl;
}
