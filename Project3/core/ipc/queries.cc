#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>
#include <dirent.h>

#include "../../include/ipc/ipc.hpp"
#include "../../include/ipc/setup.hpp"
#include "../../include/ipc/queries.hpp"
#include "../../include/utils/parsing.hpp"
#include "../../include/utils/process_utils.hpp"



void ipc::travel_monitor::queries::travel_request(travelMonitorIndex* tm_index, structures::NetworkCommunication* network_info,
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
    int fd = network_info[monitor_with_from_country].client_socket;

    /* send the data of the query */
    ipc::_send_message(fd, fd, TRAVEL_REQUEST_SEND_DATA, tr_data_as_byte_arr, len, tm_index->input->socket_buffer_size);

    /* receive the response from the monitor */
    uint8_t msg_id = REJECT;
    char buf[128] = {0};
    ipc::_receive_message(fd, fd, msg_id, buf, len, tm_index->input->socket_buffer_size);

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
            std::cout << "This message should have never been printed. In ipc::travel_monitor::queries::travel_request()";
            break;
    }
    std::cout << std::endl;
}



void ipc::monitor::queries::travel_request(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[])
{
    structures::TRData tr_data(data);
    Date* vaccination_date = m_index->virus_list->get_vaccination_date(tr_data.citizen_id, tr_data.virus_name);
    bool was_accepted = false;

    if (vaccination_date == NULL || *tr_data.date <= *vaccination_date)
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NOT_VACCINATED, NULL, 0, m_index->input->socket_buffer_size);
    }
    else if (!tr_data.date->is_within_6_months_in_the_past(vaccination_date))
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NEED_VACCINATION, NULL, 0, m_index->input->socket_buffer_size);
    }
    else
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_OK, NULL, 0, m_index->input->socket_buffer_size);
        was_accepted = true;
    }

    structures::TRQuery* query = new structures::TRQuery(tr_data.date, tr_data.country_from, tr_data.virus_name, was_accepted);
    m_index->logger->insert(query);

    delete tr_data.date;
}



void ipc::travel_monitor::queries::travel_stats(travelMonitorIndex* tm_index, const structures::TSData & ts_data, ErrorHandler & handler)
{
    /* make sure that the from country exists in the database */
    if (ts_data.country != "" && tm_index->country_id(ts_data.country) == -1)
    {
        handler.status = UNKNOWN_COYNTRY;
        handler.invalid_value = ts_data.country;
        return;
    }

    /* make sure that the virus exists in the database */
    BFPair* bf_pair = tm_index->bloom_filters->get(ts_data.virus_name);
    if (bf_pair == NULL)
    {
        handler.status = UNKNOWN_VIRUS;
        handler.invalid_value = ts_data.virus_name;
        return;
    }

    /* perform the query */
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



void ipc::travel_monitor::queries::add_vaccination_records(travelMonitorIndex* tm_index, structures::NetworkCommunication* network_info,
                                                           const std::string & country, ErrorHandler & handler)
{
    /* make sure that the from country exists in the database */
    int country_id = tm_index->country_id(country);
    if (country_id == -1)
    {
        handler.status = UNKNOWN_COYNTRY;
        handler.invalid_value = country;
        return;
    }

    /* find the Monitor with that country */
    size_t monitor_with_from_country = tm_index->monitor_with_country(country_id);
    int fd = network_info[monitor_with_from_country].client_socket;

    /* send the country and the message to the corresponding Monitor */
    ipc::_send_message(fd, fd, ADD_VACCINATION_RECORDS_SEND_DATA, country.c_str(), country.length() + 1, tm_index->input->socket_buffer_size);

    /* receive the bloom filters */
    ipc::travel_monitor::setup::_receive_bloom_filters(tm_index, fd, fd);

    /* receive a confirmation message */
    uint8_t dummy_msg_id = REJECT;
    size_t bytes_in = 0;
    ipc::_receive_message(fd, fd, dummy_msg_id, NULL, bytes_in, tm_index->input->socket_buffer_size);
}



void ipc::monitor::queries::add_vaccination_records(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[], ErrorHandler & handler)
{
    /* now send the bloom filters back the the parent process */
    ipc::monitor::setup::send_bloom_filters(m_index, input_fd, output_fd);
    /* send also a confirmation message */
    ipc::_send_message(input_fd, output_fd, ADD_VACCINATION_RECORDS_OK, NULL, 0, m_index->input->socket_buffer_size);
}



void ipc::travel_monitor::queries::search_vaccination_status(travelMonitorIndex* tm_index, structures::NetworkCommunication* network_info, const std::string & id, ErrorHandler & handler)
{
    /* get the file descriptors for the active monitors */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    int fds[active_monitors] = {0};
    for (size_t i = 0; i < active_monitors; i++)
        fds[i] = network_info[i].client_socket;

    /* send the citizen ID it to all the monitors */
    for (size_t i = 0; i < active_monitors; i++)
        ipc::_send_message(fds[i], fds[i], SEARCH_VACCINATION_STATUS_SEND_DATA, id.c_str(), id.length() + 1, tm_index->input->socket_buffer_size);
    bool flag_id_was_found = false;

    /* variables used for receiving messages */
    uint8_t msg_id = REJECT;
    size_t bytes_in = 0;
    char message[512] = {0};

    /* receive a message (has-does't have citizendID and vaccination status if it has) from every (active) monitor */
    bool finished_monitors[active_monitors] = {false};
    for (size_t count_finished_monitors = 0; count_finished_monitors < active_monitors; count_finished_monitors++)
    {
        size_t ready_monitor = ipc::_poll_until_any_read(fds, finished_monitors, active_monitors, count_finished_monitors);

        /* receive the message ID */
        ipc::_receive_message(fds[ready_monitor], fds[ready_monitor], msg_id, message, bytes_in, tm_index->input->socket_buffer_size);

        /* if this is the monitor that contains the citizen with the needed ID */
        if (msg_id == SEARCH_VACCINATION_STATUS_CONTAINS)
        {
            flag_id_was_found = true;
            /* create the record in order to print information about it */
            Record record(message);
            std::cout << record.id << " " << record.name << " " << record.surname << " " << *record.country << std::endl
                      << "AGE " << +record.age << std::endl;
            delete record.country;

            /* recaive the number of viruses for which the citizen has information about */
            size_t num_viruses = 0;
            ipc::_receive_numeric(fds[ready_monitor], fds[ready_monitor], num_viruses, tm_index->input->socket_buffer_size);

            /* iterate for that amount to receive information about each virus separately */
            char buf[128] = {0};
            for (size_t i = 0; i < num_viruses; i++)
            {
                ipc::_receive_message(fds[ready_monitor], fds[ready_monitor], msg_id, buf, bytes_in, tm_index->input->socket_buffer_size);
                structures::VaccinationStatus info(buf);
                std::cout << info.virus_name << " ";
                (info.status) ? std::cout << "VACCINATED ON " << *info.date
                              : std::cout << "NOT YET VACCINATED";
                std::cout << std::endl;
                delete info.date;
                memset(buf, 0, 128);
            }
        }
        finished_monitors[ready_monitor] = true;
    }

    /* handle the case where the citizen was not found in the dataset */
    if (!flag_id_was_found)
    {
        handler.status = UNKNOWN_ID;
        handler.invalid_value = id;
    }
}



void ipc::monitor::queries::search_vaccination_status(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[])
{
    /* get the Record with the specific citizen ID */
    std::string citizen_id(data);
    Record* record = m_index->records->get(citizen_id);

    /* if it doesn't exist in this monitor, send an appropriate message and return */
    if (record == NULL)
    {
        ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_DOES_NOT_CONTAIN, NULL, 0, m_index->input->socket_buffer_size);
        return;
    }

    /* else, the record exists in this monitor, so we have to send it back to the travelMonitor along with vaccine status information */
    char record_str[512] = {0};
    size_t bytes_out = record->to_str(record_str);
    ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_CONTAINS, record_str, bytes_out, m_index->input->socket_buffer_size);

    /* find out how many viruses the citizen has information for, and the corresponding status */
    size_t num_viruses = 0;
    structures::VaccinationStatus** info = m_index->virus_list->get_vaccination_status_of_citizen(citizen_id, num_viruses);

    /* send the number of viruses to the travel Monitor */
    ipc::_send_numeric(input_fd, output_fd, num_viruses, m_index->input->socket_buffer_size);

    /* loop for each piece of information in order to send it to the travelMonitor */
    char buf[128] = {0};
    for (size_t i = 0; i < num_viruses; i++)
    {
        size_t bytes_out = info[i]->to_str(buf);
        ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_DATA, buf, bytes_out, m_index->input->socket_buffer_size);
        delete info[i];
        memset(buf, 0, 128);
    }

    /* free the allocated memory and return */
    delete[] info;
}



void ipc::travel_monitor::queries::quit_monitors(travelMonitorIndex* tm_index, structures::NetworkCommunication* network_info)
{
    /* get the file descriptors for the active monitors */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    int fds[active_monitors] = {0};
    for (size_t i = 0; i < active_monitors; i++)
        fds[i] = network_info[i].client_socket;

    /* send an "EXIT" message to all the monitors */
    for (size_t i = 0; i < active_monitors; i++)
        ipc::_send_message(fds[i], fds[i], EXIT, NULL, 0, tm_index->input->socket_buffer_size);
}
