#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/process_utils.hpp"
#include "../../include/ipc/ipc.hpp"



void ipc::_poll_until_read(const int & fd)
{
    struct pollfd fdarr[1];
    fdarr[0].fd = fd;
    fdarr[0].events = POLLIN;
    int rc = poll(fdarr, 1, 1500);
    while (rc != 1 || (fdarr[0].revents != POLLIN && fdarr[0].revents != (POLLIN | POLLHUP)))
    {
        rc = poll(fdarr, 1, 1500);
    }
    if (errno != 0)
        errno = 0;
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
    while (rc < 1 || ipc::ready_fd(fdarr, num_to_read) == -1)
        rc = poll(fdarr, num_to_read, 500);
    return map[ipc::ready_fd(fdarr, num_to_read)];
}


int ipc::ready_fd(struct pollfd fdarr[], size_t num_fds)
{
    for (size_t i = 0; i < num_fds; i++)
        if (fdarr[i].revents == POLLIN || fdarr[i].revents == (POLLIN | POLLHUP))
            return i;
    return -1;    
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


void ipc::wait_for_command(const int & input_fd, const int & output_fd, uint8_t & msg_id, char message[], const uint64_t & buffer_size)
{
    /* receive the command */
    size_t len = 0;
    ipc::_receive_message(input_fd, output_fd, msg_id, message, len, buffer_size);
}
