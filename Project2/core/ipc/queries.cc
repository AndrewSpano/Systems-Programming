#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>
#include <dirent.h>

#include "../../include/ipc/ipc.hpp"
#include "../../include/ipc/queries.hpp"
#include "../../include/utils/parsing.hpp"
#include "../../include/utils/process_utils.hpp"



void ipc::travel_monitor::queries::travel_request(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes,
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
    ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_SEND_DATA, tr_data_as_byte_arr, len, tm_index->input->buffer_size);

    /* receive the response from the monitor */
    uint8_t msg_id = REJECT;
    char buf[128] = {0};
    ipc::_receive_message(input_fd, output_fd, msg_id, buf, len, tm_index->input->buffer_size);

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

    if (vaccination_date == NULL)
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NOT_VACCINATED, NULL, 0, m_index->input->buffer_size);
    }
    else if (!tr_data.date->is_within_6_months_in_the_past(vaccination_date))
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_NEED_VACCINATION, NULL, 0, m_index->input->buffer_size);
    }
    else
    {
        ipc::_send_message(input_fd, output_fd, TRAVEL_REQUEST_OK, NULL, 0, m_index->input->buffer_size);
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



void ipc::travel_monitor::queries::add_vaccination_records(travelMonitorIndex* tm_index, pid_t monitor_pids[], structures::CommunicationPipes* pipes,
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
    int input_fd = open(pipes[monitor_with_from_country].input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes[monitor_with_from_country].output, O_WRONLY);

    /* send a SIGURS1 to that monitor */
    kill(monitor_pids[monitor_with_from_country], SIGUSR1);

    /* receive the bloom filters */
    ipc::travel_monitor::_receive_bloom_filters(tm_index, input_fd, output_fd);

    /* close the pipes */
    close(input_fd);
    close(output_fd);

    std::cout << std::endl;
}



void ipc::monitor::queries::add_vaccination_records(MonitorIndex* m_index, const structures::CommunicationPipes* pipes, ErrorHandler & handler)
{
    /* check all the countries */
    for (size_t country_id = 0; country_id < m_index->num_countries; country_id++)
    {
        std::string* country = &(m_index->countries[country_id]);
        char country_dir_path[256] = {0};
        sprintf(country_dir_path, "%s/%s", m_index->input->root_dir.c_str(), country->c_str());

        struct dirent **namelist;
        int num_files = scandir(country_dir_path, &namelist, NULL, alphasort);

        for (size_t i = 0; i < num_files; i++)
        {
            std::string* filename = new std::string(namelist[i]->d_name);
            /* parse the files that haven't been seen for this country */
            if (*filename != "." && *filename != ".." && !m_index->files_per_country[country_id]->in(filename))
            {
                char path[256];
                sprintf(path, "%s/%s", country_dir_path, filename->c_str()); 

                parsing::dataset::parse_country_dataset(country, path, m_index, handler);
                m_index->files_per_country[country_id]->insert(filename);
            }
            else
            {
                delete filename;
            }
            free(namelist[i]);
        }
        free(namelist);
    }

    /* now send the bloom filters back the the parent process */
    ipc::monitor::send_bloom_filters(m_index, pipes);
}



void ipc::travel_monitor::queries::search_vaccination_status(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, const std::string & id)
{
    /* open all pipes and get their file descriptors */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    int input_fds[active_monitors] = {0};
    int output_fds[active_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDONLY | O_NONBLOCK, output_fds, O_WRONLY, active_monitors);


    /* send the citizen ID it to all the monitors */
    for (size_t i = 0; i < active_monitors; i++)
        ipc::_send_message(input_fds[i], output_fds[i], SEARCH_VACCINATION_STATUS_SEND_DATA, id.c_str(), id.length() + 1, tm_index->input->buffer_size);

    /* variables used for receiving messages */
    uint8_t msg_id = REJECT;
    size_t bytes_in = 0;
    char message[512] = {0};

    /* receive a message (has-does't have citizendID and vaccination status if it has) from every (active) monitor */
    bool finished_monitors[active_monitors] = {false};
    for (size_t count_finished_monitors = 0; count_finished_monitors < active_monitors; count_finished_monitors++)
    {
        size_t ready_monitor = ipc::_poll_until_any_read(input_fds, finished_monitors, active_monitors, count_finished_monitors);

        /* receive the message ID */
        ipc::_receive_message(input_fds[ready_monitor], output_fds[ready_monitor], msg_id, message, bytes_in, tm_index->input->buffer_size);

        /* if this is the monitor that contains the citizen with the needed ID */
        if (msg_id == SEARCH_VACCINATION_STATUS_CONTAINS)
        {
            /* create the record in order to print information about it */
            Record record(message);
            std::cout << record.id << " " << record.name << " " << record.surname << " " << *record.country << std::endl
                      << "AGE " << +record.age << std::endl;
            delete record.country;

            /* recaive the number of viruses for which the citizen has information about */
            size_t num_viruses = 0;
            ipc::_receive_numeric(input_fds[ready_monitor], output_fds[ready_monitor], num_viruses, tm_index->input->buffer_size);

            /* iterate for that amount to receive information about each virus separately */
            char buf[128] = {0};
            for (size_t i = 0; i < num_viruses; i++)
            {
                ipc::_receive_message(input_fds[ready_monitor], output_fds[ready_monitor], msg_id, buf, bytes_in, tm_index->input->buffer_size);
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

    /* close all pipes */
    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, active_monitors);
}



void ipc::monitor::queries::search_vaccination_status(MonitorIndex* m_index, const int & input_fd, const int & output_fd, char data[])
{
    /* get the Record with the specific citizen ID */
    std::string citizen_id(data);
    Record* record = m_index->records->get(citizen_id);

    /* if it doesn't exist in this monitor, send an appropriate message and return */
    if (record == NULL)
    {
        ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_DOES_NOT_CONTAIN, NULL, 0, m_index->input->buffer_size);
        return;
    }

    /* else, the record exists in this monitor, so we have to send it back to the travelMonitor along with vaccine status information */
    char record_str[512] = {0};
    size_t bytes_out = record->to_str(record_str);
    ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_CONTAINS, record_str, bytes_out, m_index->input->buffer_size);

    /* find out how many viruses the citizen has information for, and the corresponding status */
    size_t num_viruses = 0;
    structures::VaccinationStatus** info = m_index->virus_list->get_vaccination_status_of_citizen(citizen_id, num_viruses);

    /* send the number of viruses to the travel Monitor */
    ipc::_send_numeric(input_fd, output_fd, num_viruses, m_index->input->buffer_size);

    /* loop for each piece of information in order to send it to the travelMonitor */
    char buf[128] = {0};
    for (size_t i = 0; i < num_viruses; i++)
    {
        size_t bytes_out = info[i]->to_str(buf);
        ipc::_send_message(input_fd, output_fd, SEARCH_VACCINATION_STATUS_DATA, buf, bytes_out, m_index->input->buffer_size);
        delete info[i];
        memset(buf, 0, 128);
    }

    /* free the allocated memory and return */
    delete[] info;
}



void ipc::travel_monitor::queries::handle_sigchld(travelMonitorIndex* tm_index, structures::CommunicationPipes* pipes, pid_t* monitor_pids)
{
    /* see which monitors are "alive" because they have countries to process */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;

    /* get the process id of the process that terminated */
    pid_t dead_process_pid = waitpid(-1, NULL, WNOHANG);
    while (dead_process_pid > 0)
    {
        /* get the ID of the monitor that terminated by scanning all the pids */
        size_t dead_monitor = process_utils::travel_monitor::dead_monitor(monitor_pids, dead_process_pid, tm_index->input->num_monitors);

        /* if the monitor that terminated was an "active" monitor */
        if (dead_monitor < active_monitors)
        {
            /* create a new process that will replace the old one */
            process_utils::travel_monitor::_create_monitor(monitor_pids, pipes, dead_monitor);

            /* open the named pipes for that monitor */
            int input_fd = open(pipes[dead_monitor].input, O_RDONLY | O_NONBLOCK);
            int output_fd = open(pipes[dead_monitor].output, O_WRONLY);

            /* send the arguments to the process */
            ipc::travel_monitor::_send_args(input_fd, output_fd, *tm_index->input);

            /* now send all the countries */
            for (size_t i = 0; i < tm_index->num_countries; i++)
                if (tm_index->monitor_with_country(i) == dead_monitor)
                    ipc::_send_message(input_fd, output_fd, SEND_COUNTRY, tm_index->countries[i].c_str(), tm_index->countries[i].length() + 1, tm_index->input->buffer_size);
            /* let the monitor know that all the countries have been sent */
            ipc::_send_message(input_fd, output_fd, COUNTRIES_SENT, NULL, 0, tm_index->input->buffer_size);

            /* receive the bloom filters */
            ipc::travel_monitor::_receive_bloom_filters(tm_index, input_fd, output_fd);

            /* close the pipes as they are no longer needed */
            close(input_fd);
            close(output_fd);
        }

        /* get the pid of the next process that terminated, if any other process has terminated */
        dead_process_pid = waitpid(-1, NULL, WNOHANG);
    }
}