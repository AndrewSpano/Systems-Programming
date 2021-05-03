#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/process_utils.hpp"
#include "../../include/utils/comm_utils.hpp"



void comm_utils::_poll_until_read(const int & fd)
{
    struct pollfd fdarr[1];
    fdarr[0].fd = fd;
    fdarr[0].events = POLLIN;
    int rc = poll(fdarr, 1, 500);
    while (rc != 1 || (fdarr[0].revents != POLLIN && fdarr[0].revents != (POLLIN | POLLHUP)))
    {
        rc = poll(fdarr, 1, 500);
    }
}


void comm_utils::_wait_ack(const int & fd)
{
    uint8_t response = REJECT;
    while (response != ACK)
    {
        comm_utils::_poll_until_read(fd);
        ssize_t ret = read(fd, &response, sizeof(uint8_t));
    }
}


void comm_utils::_send_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size)
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
        comm_utils::_wait_ack(input_fd);

        bytes_sent += bytes_to_write;
        bytes_left -= bytes_to_write;
    }
}


void comm_utils::_receive_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size)
{
    /* wait a signal from the other process to start receiving the numeric value */
    uint8_t dummy_msg_id = 0;
    comm_utils::_poll_until_read(input_fd);
    ssize_t ret = read(input_fd, &dummy_msg_id, sizeof(dummy_msg_id));

    size_t bytes_received = 0;
    size_t bytes_left = sizeof(numeric);
    char* _numeric_ptr = (char *) &numeric;
    uint8_t ack = ACK;

    while (bytes_left > 0)
    {
        size_t bytes_to_receive = (bytes_left <= buffer_size) ? bytes_left : buffer_size;

        /* wait until the next byte(s) is(are) sent */
        comm_utils::_poll_until_read(input_fd);

        /* read the corresponding bytes and send ACK */
        ret = read(input_fd, _numeric_ptr + bytes_received, bytes_to_receive);
        ret = write(output_fd, &ack, sizeof(ack));

        bytes_received += bytes_to_receive;
        bytes_left -= bytes_to_receive;
    }
}


void comm_utils::_notify_and_wait_ack(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const size_t & bytes_out, const uint64_t & buffer_size)
{
    ssize_t ret = write(output_fd, &msg_id, sizeof(msg_id));
    comm_utils::_wait_ack(input_fd);
    comm_utils::_send_numeric(input_fd, output_fd, bytes_out, buffer_size);
}


void comm_utils::_send_message(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size)
{
    char send_buf[buffer_size] = {0};

    /* write the message ID to inform the end process that we want to send a specific message */
    comm_utils::_notify_and_wait_ack(input_fd, output_fd, msg_id, bytes_out, buffer_size);

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
            comm_utils::_wait_ack(input_fd);

            bytes_out -= bytes_to_write;
            bytes_sent += bytes_to_write;
        }
    }
}


void comm_utils::_receive_message(const int & input_fd, const int & output_fd, uint8_t & msg_id, char buf[], size_t & bytes_in, const uint64_t & buffer_size)
{
    uint8_t ack = ACK;

    /* wait until a message arrives */
    comm_utils::_poll_until_read(input_fd);

    /* read the message ID, send an ACK and then read the length of the message */
    ssize_t ret = read(input_fd, &msg_id, sizeof(msg_id));
    ret = write(output_fd, &ack, sizeof(msg_id));
    comm_utils::_receive_numeric(input_fd, output_fd, bytes_in, buffer_size);

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
            comm_utils::_poll_until_read(input_fd);
            ret = read(input_fd, buf + bytes_received, bytes_to_read);
            ret = write(output_fd, &ack, sizeof(ack));

            bytes_received += bytes_to_read;
            bytes_left -= bytes_to_read;
        }
    }
}


void comm_utils::travel_monitor::send_args(const structures::CommunicationPipes pipes[], const structures::Input & input)
{
    size_t len = input.root_dir.length() + 1;
    /* open all pipes and get their file descriptors */
    int input_fds[input.num_monitors] = {0};
    int output_fds[input.num_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDWR | O_NONBLOCK, output_fds, O_WRONLY, input.num_monitors);

    for (size_t i = 0; i < input.num_monitors; i++)
    {
        /* write: 1) buffer size, 2) bloom filter size, 3) root_dir */
        comm_utils::_send_numeric(input_fds[i], output_fds[i], input.buffer_size, 1);
        comm_utils::_send_numeric(input_fds[i], output_fds[i], input.bf_size, input.buffer_size);
        comm_utils::_send_message(input_fds[i], output_fds[i], INPUT, input.root_dir.c_str(), input.root_dir.length() + 1, input.buffer_size);
    }

    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, input.num_monitors);
}


void comm_utils::travel_monitor::assign_countries(const structures::CommunicationPipes pipes[], const structures::Input & input)
{
    /* open all pipes and get their file descriptors */
    int input_fds[input.num_monitors] = {0};
    int output_fds[input.num_monitors] = {0};
    process_utils::travel_monitor::open_all_pipes(pipes, input_fds, O_RDWR | O_NONBLOCK, output_fds, O_WRONLY, input.num_monitors);

    char path[300] = {0};
    struct dirent **namelist;
    int num_countries = scandir(input.root_dir.c_str(), &namelist, NULL, alphasort);
    size_t round_robin = 0;

    for (size_t i = 0; i < num_countries; i++)
    {
        /* send country to specific moniror */
        if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
        {
            comm_utils::_send_message(input_fds[round_robin], output_fds[round_robin], SEND_COUNTRY, namelist[i]->d_name, strlen(namelist[i]->d_name) + 1, input.buffer_size);
            round_robin = (round_robin + 1) % input.num_monitors;
        }
        free(namelist[i]);
    }
    free(namelist);

    /* inform the Monitors that no more countries will be arriving */
    for (size_t i = 0; i < input.num_monitors; i++)
    {
        comm_utils::_send_message(input_fds[i], output_fds[i], COUNTRIES_SENT, NULL, 0, input.buffer_size);
    }

    /* close all pipes */
    process_utils::travel_monitor::close_all_pipes(input_fds, output_fds, input.num_monitors);
}


void comm_utils::monitor::init_args(const structures::CommunicationPipes & pipes, structures::Input & input)
{
    int input_fd = open(pipes.input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes.output, O_WRONLY);

    /* variables used to get root directory */
    char buf[1028];
    uint8_t dummy_msg_id = 0;
    size_t dummy_bytes_in = 0;

    /* get 1) buffer size, 2) bloom filter size and 3) root directory */
    comm_utils::_receive_numeric(input_fd, output_fd, input.buffer_size, 1);
    comm_utils::_receive_numeric(input_fd, output_fd, input.bf_size, input.buffer_size);
    comm_utils::_receive_message(input_fd, output_fd, dummy_msg_id, buf, dummy_bytes_in, input.buffer_size);
    input.root_dir.assign(buf);

    close(input_fd);
    close(output_fd);
}


void comm_utils::monitor::receive_countries(const structures::CommunicationPipes & pipes, structures::Input & input)
{
    uint16_t num_countries = 0;
    uint8_t msg_id = REJECT;
    
    char buf[128];
    size_t bytes_in = 0;

    int input_fd = open(pipes.input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes.output, O_WRONLY);

    while (msg_id != COUNTRIES_SENT)
    {
        comm_utils::_receive_message(input_fd, output_fd, msg_id, buf, bytes_in, input.buffer_size);
        printf("From pipe: %s: Message ID: %u, and value received: %s\n", pipes.input, msg_id, buf);
    }

    close(input_fd);
    close(output_fd);
}
