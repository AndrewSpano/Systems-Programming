#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>


#include "../../include/ipc/ipc.hpp"
#include "../../include/ipc/setup.hpp"
#include "../../include/utils/process_utils.hpp"



void ipc::travel_monitor::setup::_receive_bloom_filters(travelMonitorIndex* tm_index, const int & input_fd, const int & output_fd)
{
    /* variables used for receiving messages */
    size_t num_viruses = 0;
    uint8_t msg_id = REJECT;
    size_t bytes_in = 0;
    char virus_name[128] = {0};
    char bf_bits[tm_index->input->bf_size] = {0};

    /* receive the number of viruses */
    ipc::_receive_numeric(input_fd, output_fd, num_viruses, tm_index->input->socket_buffer_size);
    for (size_t i = 0; i < num_viruses; i++)
    {
        /* receive the virus name and its bloom filter, then add/update them to the list */
        ipc::_receive_message(input_fd, output_fd, msg_id, virus_name, bytes_in, tm_index->input->socket_buffer_size);
        std::string _virus_name_str(virus_name);
        ipc::_receive_message(input_fd, output_fd, msg_id, bf_bits, bytes_in, tm_index->input->socket_buffer_size);

        BFPair* existing_bf_pair = tm_index->bloom_filters->get(_virus_name_str);
        if (existing_bf_pair == NULL)
            tm_index->bloom_filters->insert(new BFPair(_virus_name_str, tm_index->input->bf_size, DEFAULT_K, (uint8_t *) bf_bits));
        else
            existing_bf_pair->bloom_filter->update((uint8_t *) bf_bits);
    }
}


void ipc::travel_monitor::setup::receive_bloom_filters(travelMonitorIndex* tm_index, structures::NetworkCommunication* network_info)
{
    /* place the file descriptors for the active monitors inside an array */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    int fds[active_monitors] = {0};
    for (size_t i = 0; i < active_monitors; i++)
        fds[i] = network_info[i].client_socket;

    /* receive messages (viruses and bloom filters) from every (active) monitor */
    bool finished_monitors[active_monitors] = {false};
    for (size_t count_finished_monitors = 0; count_finished_monitors < active_monitors; count_finished_monitors++)
    {
        size_t ready_monitor = ipc::_poll_until_any_read(fds, finished_monitors, active_monitors, count_finished_monitors);

        /* receive pairs of: a) virus name, b) bloom filter for that virus */
        ipc::travel_monitor::setup::_receive_bloom_filters(tm_index, fds[ready_monitor], fds[ready_monitor]);
        finished_monitors[ready_monitor] = true;
    }
}



void ipc::monitor::setup::send_bloom_filters(MonitorIndex* m_index, const int & input_fd, const int & output_fd)
{
    /* get the bloom filter of every virus */
    size_t num_viruses = m_index->virus_list->get_size();
    BFPair** bf_per_virus = new BFPair*[num_viruses];
    m_index->virus_list->get_bf_pairs(bf_per_virus);

    /* send the number of virus-bloom_filter that will be sent to the travel Monitor */
    ipc::_send_numeric(input_fd, output_fd, num_viruses, m_index->input->socket_buffer_size);

    /* now send each pair separately */
    for (size_t i = 0; i < num_viruses; i++)
    {
        std::string virus_name = bf_per_virus[i]->virus_name;
        ipc::_send_message(input_fd, output_fd, SEND_VIRUS_NAME, virus_name.c_str(), virus_name.length() + 1, m_index->input->socket_buffer_size);

        uint8_t* bits = bf_per_virus[i]->bloom_filter->get_bloom_filter();
        ipc::_send_message(input_fd, output_fd, SEND_BF, (char *) bits, m_index->input->bf_size, m_index->input->socket_buffer_size);
        delete[] bits;
    }

    /* delete the virus-bloom_filter pair information */
    for (size_t i = 0; i < num_viruses; i++)
        delete bf_per_virus[i];
    delete[] bf_per_virus;    
}
