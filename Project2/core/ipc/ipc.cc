#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/process_utils.hpp"
#include "../../include/ipc/ipc.hpp"
#include "../../include/data_structures/list.hpp"



void ipc::_poll_until_read(const int & fd)
{
    struct pollfd fdarr[1];
    fdarr[0].fd = fd;
    fdarr[0].events = POLLIN;
    int rc = poll(fdarr, 1, 1500);
    while (rc != 1 || (fdarr[0].revents != POLLIN && fdarr[0].revents != (POLLIN | POLLHUP)))
    {
        if (errno != 0) errno = 0;
        rc = poll(fdarr, 1, 1500);
    }
}


size_t ipc::_poll_until_any_read(int fds[], bool fd_has_read[], const size_t & num_fds, const size_t & count_read)
{
    size_t num_to_read = num_fds - count_read;
    size_t current_fd = 0;
    
    struct pollfd fdarr[num_to_read];
    size_t map[num_to_read] = {0};
    for (size_t i = 0; i < num_fds; i++)
    {
        if (!fd_has_read[i])
        {
            fdarr[current_fd].fd = fds[i];
            fdarr[current_fd].events = POLLIN;
            map[current_fd] = i;
            current_fd++;
        }
    }
    
    int rc = poll(fdarr, num_to_read, 500);
    while (rc < 1 || process_utils::travel_monitor::ready_fd(fdarr, num_to_read) == -1)
    {
        rc = poll(fdarr, num_to_read, 500);
    }
    return map[process_utils::travel_monitor::ready_fd(fdarr, num_to_read)];
}


void ipc::_wait_ack(const int & fd)
{
    uint8_t response = REJECT;
    while (response != ACK)
    {
        ipc::_poll_until_read(fd);
        ssize_t ret = read(fd, &response, sizeof(uint8_t));
    }
}



void ipc::_send_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size)
{
    /* let the other process know that we want to send a numeric value */
    uint8_t msg_id = NUMERIC;
    ssize_t ret = write(output_fd, &msg_id, sizeof(msg_id));

    char send_buf[buffer_size] = {0};
    size_t bytes_sent = 0;
    size_t bytes_left = sizeof(numeric);
    char* _numeric_ptr = (char *) &numeric;

    while (bytes_left > 0)
    {
        size_t bytes_to_write = (bytes_left <= buffer_size) ? bytes_left : buffer_size;

        memcpy(send_buf, _numeric_ptr + bytes_sent, bytes_to_write);
        ret = write(output_fd, send_buf, bytes_to_write);
        ipc::_wait_ack(input_fd);

        bytes_sent += bytes_to_write;
        bytes_left -= bytes_to_write;
    }
}


void ipc::_receive_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size)
{
    /* wait a signal from the other process to start receiving the numeric value */
    uint8_t dummy_msg_id = 0;
    ipc::_poll_until_read(input_fd);
    ssize_t ret = read(input_fd, &dummy_msg_id, sizeof(dummy_msg_id));

    size_t bytes_received = 0;
    size_t bytes_left = sizeof(numeric);
    char* _numeric_ptr = (char *) &numeric;
    uint8_t ack = ACK;

    while (bytes_left > 0)
    {
        size_t bytes_to_receive = (bytes_left <= buffer_size) ? bytes_left : buffer_size;

        /* wait until the next byte(s) is(are) sent */
        ipc::_poll_until_read(input_fd);

        /* read the corresponding bytes and send ACK */
        ret = read(input_fd, _numeric_ptr + bytes_received, bytes_to_receive);
        ret = write(output_fd, &ack, sizeof(ack));

        bytes_received += bytes_to_receive;
        bytes_left -= bytes_to_receive;
    }
}



void ipc::_notify_and_wait_ack(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const size_t & bytes_out, const uint64_t & buffer_size)
{
    ssize_t ret = write(output_fd, &msg_id, sizeof(msg_id));
    ipc::_wait_ack(input_fd);
    ipc::_send_numeric(input_fd, output_fd, bytes_out, buffer_size);
}


void ipc::_send_message(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size)
{
    char send_buf[buffer_size] = {0};

    /* write the message ID to inform the end process that we want to send a specific message */
    ipc::_notify_and_wait_ack(input_fd, output_fd, msg_id, bytes_out, buffer_size);

    /* if there is data to be sent */
    if (bytes_out > 0)
    {
        /* chop down the message in blocks of `buffer_size` bytes or less */
        size_t bytes_sent = 0;
        while (bytes_out > 0)
        {
            size_t bytes_to_write = (bytes_out <= buffer_size) ? bytes_out : buffer_size;
            memcpy(send_buf, buf + bytes_sent, bytes_to_write);

            /* wait for an "ACK" response from the end process in order to send the next block of bytes */
            ssize_t ret = write(output_fd, send_buf, bytes_to_write);
            ipc::_wait_ack(input_fd);

            bytes_out -= bytes_to_write;
            bytes_sent += bytes_to_write;
        }
    }
}


void ipc::_receive_message(const int & input_fd, const int & output_fd, uint8_t & msg_id, char buf[], size_t & bytes_in, const uint64_t & buffer_size)
{
    uint8_t ack = ACK;

    /* wait until a message arrives */
    ipc::_poll_until_read(input_fd);

    /* read the message ID, send an ACK and then read the length of the message */
    ssize_t ret = read(input_fd, &msg_id, sizeof(msg_id));
    ret = write(output_fd, &ack, sizeof(msg_id));
    ipc::_receive_numeric(input_fd, output_fd, bytes_in, buffer_size);

    /* if there is data to be transfered in the message */
    if (bytes_in > 0)
    {
        /* start reading the actual data of the message */
        size_t bytes_received = 0;
        size_t bytes_left = bytes_in;
        while (bytes_left > 0)
        {
            size_t bytes_to_read = (bytes_left <= buffer_size) ? bytes_left : buffer_size;

            /* wait until there is data to read, then read it and send ACK */
            ipc::_poll_until_read(input_fd);
            ret = read(input_fd, buf + bytes_received, bytes_to_read);
            ret = write(output_fd, &ack, sizeof(ack));

            bytes_received += bytes_to_read;
            bytes_left -= bytes_to_read;
        }
    }
}



void ipc::travel_monitor::_send_args(const int & input_fd, const int & output_fd, const structures::Input & input)
{
    ipc::_send_numeric(input_fd, output_fd, input.buffer_size, 1);
    ipc::_send_numeric(input_fd, output_fd, input.bf_size, input.buffer_size);
    ipc::_send_message(input_fd, output_fd, INPUT, input.root_dir.c_str(), input.root_dir.length() + 1, input.buffer_size);
}


void ipc::travel_monitor::send_args(const structures::CommunicationPipes pipes[], const structures::Input & input)
{
    size_t len = input.root_dir.length() + 1;
    /* open all pipes and get their file descriptors */
    int input_fds[input.num_monitors] = {0};
    int output_fds[input.num_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDONLY | O_NONBLOCK, output_fds, O_WRONLY, input.num_monitors);

    for (size_t i = 0; i < input.num_monitors; i++)
    {
        /* write: 1) buffer size, 2) bloom filter size, 3) root_dir */
        ipc::travel_monitor::_send_args(input_fds[i], output_fds[i], input);
    }

    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, input.num_monitors);
}



void ipc::travel_monitor::assign_countries(travelMonitorIndex* tm_index, const structures::CommunicationPipes pipes[])
{
    /* open all pipes and get their file descriptors */
    int input_fds[tm_index->input->num_monitors] = {0};
    int output_fds[tm_index->input->num_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDONLY | O_NONBLOCK, output_fds, O_WRONLY, tm_index->input->num_monitors);

    char path[256] = {0};
    struct dirent **namelist;
    int num_countries = scandir(tm_index->input->root_dir.c_str(), &namelist, NULL, alphasort);
    tm_index->init_countries(num_countries - 2);
    size_t round_robin = 0;
    size_t country_id = 0;

    for (size_t i = 0; i < num_countries; i++)
    {
        /* send country to specific monitor */
        if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
        {
            ipc::_send_message(input_fds[round_robin], output_fds[round_robin], SEND_COUNTRY, namelist[i]->d_name, strlen(namelist[i]->d_name) + 1, tm_index->input->buffer_size);
            tm_index->countries[country_id] = std::string(namelist[i]->d_name);
            country_id++;
            round_robin = (round_robin + 1) % tm_index->input->num_monitors;
        }
        free(namelist[i]);
    }
    free(namelist);

    /* inform the Monitors that no more countries will be arriving */
    for (size_t i = 0; i < tm_index->input->num_monitors; i++)
        ipc::_send_message(input_fds[i], output_fds[i], COUNTRIES_SENT, NULL, 0, tm_index->input->buffer_size);

    /* close all pipes */
    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, tm_index->input->num_monitors);
}


void ipc::travel_monitor::_receive_bloom_filters(travelMonitorIndex* tm_index, const int & input_fd, const int & output_fd)
{
    /* variables used for receiving messages */
    size_t num_viruses = 0;
    uint8_t msg_id = REJECT;
    size_t bytes_in = 0;
    char virus_name[128] = {0};
    char bf_bits[tm_index->input->bf_size] = {0};

    /* receive the number of viruses */
    ipc::_receive_numeric(input_fd, output_fd, num_viruses, tm_index->input->buffer_size);
    for (size_t i = 0; i < num_viruses; i++)
    {
        /* receive the virus name and its bloom filter, then add/update them to the list */
        ipc::_receive_message(input_fd, output_fd, msg_id, virus_name, bytes_in, tm_index->input->buffer_size);
        std::string _virus_name_str(virus_name);
        ipc::_receive_message(input_fd, output_fd, msg_id, bf_bits, bytes_in, tm_index->input->buffer_size);

        BFPair* existing_bf_pair = tm_index->bloom_filters->get(_virus_name_str);
        if (existing_bf_pair == NULL)
            tm_index->bloom_filters->insert(new BFPair(_virus_name_str, tm_index->input->bf_size, DEFAULT_K, (uint8_t *) bf_bits));
        else
            existing_bf_pair->bloom_filter->update((uint8_t *) bf_bits);
    }
}


void ipc::travel_monitor::receive_bloom_filters(travelMonitorIndex* tm_index, const structures::CommunicationPipes pipes[])
{
    /* open all pipes and get their file descriptors */
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    int input_fds[active_monitors] = {0};
    int output_fds[active_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDONLY | O_NONBLOCK, output_fds, O_WRONLY, active_monitors);

    /* receive messages (viruses and bloom filters) from every (active) monitor */
    bool finished_monitors[active_monitors] = {false};
    for (size_t count_finished_monitors = 0; count_finished_monitors < active_monitors; count_finished_monitors++)
    {
        size_t ready_monitor = ipc::_poll_until_any_read(input_fds, finished_monitors, active_monitors, count_finished_monitors);

        /* receive pairs of: a) virus name, b) bloom filter for that virus */
        ipc::travel_monitor::_receive_bloom_filters(tm_index, input_fds[ready_monitor], output_fds[ready_monitor]);
        finished_monitors[ready_monitor] = true;
    }

    /* close all pipes */
    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, active_monitors);
}


void ipc::monitor::init_args(const structures::CommunicationPipes* pipes, structures::Input & input)
{
    int input_fd = open(pipes->input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes->output, O_WRONLY);

    /* variables used to get root directory */
    char buf[1028];
    uint8_t dummy_msg_id = 0;
    size_t dummy_bytes_in = 0;

    /* get 1) buffer size, 2) bloom filter size and 3) root directory */
    ipc::_receive_numeric(input_fd, output_fd, input.buffer_size, 1);
    ipc::_receive_numeric(input_fd, output_fd, input.bf_size, input.buffer_size);
    ipc::_receive_message(input_fd, output_fd, dummy_msg_id, buf, dummy_bytes_in, input.buffer_size);
    input.root_dir.assign(buf);

    close(input_fd);
    close(output_fd);
}


void ipc::monitor::receive_countries(MonitorIndex* m_index, const structures::CommunicationPipes* pipes)
{
    uint8_t msg_id = REJECT;
    
    char buf[128];
    size_t bytes_in = 0;

    int input_fd = open(pipes->input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes->output, O_WRONLY);

    /* build the countries array by creating a list to save all the countries, and then converting it to an array */
    List<std::string> countries;
    ipc::_receive_message(input_fd, output_fd, msg_id, buf, bytes_in, m_index->input->buffer_size);
    while (msg_id != COUNTRIES_SENT)
    {
        std::string* country = new std::string(buf);
        countries.insert(country);
        ipc::_receive_message(input_fd, output_fd, msg_id, buf, bytes_in, m_index->input->buffer_size);
    }
    if (countries.get_size() > 0)
        m_index->init_countries(countries);

    close(input_fd);
    close(output_fd);
}


void ipc::monitor::send_bloom_filters(MonitorIndex* m_index, const structures::CommunicationPipes* pipes)
{
    /* get the bloom filter of every virus */
    size_t num_viruses = m_index->virus_list->get_size();
    BFPair** bf_per_virus = new BFPair*[num_viruses];
    m_index->virus_list->get_bf_pairs(bf_per_virus);

    /* open the pipes */
    int input_fd = open(pipes->input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes->output, O_WRONLY);

    /* send the number of virus-bloom_filter that will be sent to the travel Monitor */
    ipc::_send_numeric(input_fd, output_fd, num_viruses, m_index->input->buffer_size);

    /* now send each pair separately */
    for (size_t i = 0; i < num_viruses; i++)
    {
        std::string virus_name = bf_per_virus[i]->virus_name;
        ipc::_send_message(input_fd, output_fd, SEND_VIRUS_NAME, virus_name.c_str(), virus_name.length() + 1, m_index->input->buffer_size);

        uint8_t* bits = bf_per_virus[i]->bloom_filter->get_bloom_filter();
        ipc::_send_message(input_fd, output_fd, SEND_BF, (char *) bits, m_index->input->bf_size, m_index->input->buffer_size);
        delete[] bits;
    }

    /* close the pipes */
    close(input_fd);
    close(output_fd);

    /* delete the virus-bloom_filter pair information */
    for (size_t i = 0; i < num_viruses; i++)
        delete bf_per_virus[i];
    delete[] bf_per_virus;    
}



void ipc::monitor::wait_for_command(MonitorIndex* m_index, const int & input_fd, const int & output_fd, uint8_t & msg_id, char message[])
{
    /* receive the command */
    size_t len = 0;
    ipc::_receive_message(input_fd, output_fd, msg_id, message, len, m_index->input->buffer_size);
}
