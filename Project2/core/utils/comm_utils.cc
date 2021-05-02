#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/comm_utils.hpp"


void process_utils::travel_monitor::_create_pipe(char** named_pipe, const char* type, const size_t & id)
{
    *named_pipe = new char[40];
    sprintf(*named_pipe, "pipes/%s_%lu", type, id);
    if (mkfifo(*named_pipe, 0666) < 0)
    {
        perror("mkfifo() failed in process_utils::_create_pipe()");
        exit(-1);
    }
}


void process_utils::travel_monitor::create_pipes(structures::commPipes comm_pipes[], const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        _create_pipe(&comm_pipes[i].coordination_pipe, "coordination", i);
        _create_pipe(&comm_pipes[i].data_pipe, "data", i);        
    }
}


void process_utils::travel_monitor::_free_and_delete_pipe(const char* named_pipe)
{
    if (unlink(named_pipe) < 0)
    {
        perror("unlink() failed in process_utils::_free_and_delete_pipe()");
        exit(-1);
    }
    delete[] named_pipe;
}


void process_utils::travel_monitor::free_and_delete_pipes(structures::commPipes comm_pipes[], const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        _free_and_delete_pipe(comm_pipes[i].coordination_pipe);
        _free_and_delete_pipe(comm_pipes[i].data_pipe);
    }
}


void process_utils::travel_monitor::create_monitors(pid_t monitor_pids[], structures::commPipes comm_pipes[], const u_int16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        monitor_pids[i] = fork();
        if (monitor_pids[i] < 0)
        {
            perror("fork() failed in process_utils::create_monitors()");
            exit(-1);
        }
        else if (monitor_pids[i] == 0)
        {
            const char* const argv[] = {"bin/Monitor", "-c", comm_pipes[i].coordination_pipe, "-d", comm_pipes[i].data_pipe, NULL};
            execvp(argv[0], const_cast<char* const*>(argv));
            perror("execvp() failed in process_utils::create_monitors()");
            exit(-1);
        }
    }
}


void process_utils::travel_monitor::send_args(const structures::commPipes comm_pipes[], const structures::Input & input)
{
    size_t len = input.root_dir.length() + 1;
    for (size_t i = 0; i < input.num_monitors; i++)
    {
        int fd = open(comm_pipes[i].data_pipe, O_WRONLY);
        if (fd == -1)
        {
            perror("open fail");
            exit(-1);
        }

        /* write: 1) buffer size, 2) bloom filter size, 3) length of the root_dir string, 4) root_dir */
        ssize_t ret = write(fd, &input.buffer_size, sizeof(input.buffer_size));
        ret = write(fd, &input.bf_size, sizeof(input.bf_size));
        ret = write(fd, &len, sizeof(len));
        ret = write(fd, input.root_dir.c_str(), len);

        close(fd);
    }
}


void process_utils::travel_monitor::assign_countries(const structures::commPipes comm_pipes[], const structures::Input & input)
{
    struct dirent **namelist;
    int num_countries = 0;
    char buf[300] = {0};

    num_countries = scandir(input.root_dir.c_str(), &namelist, NULL, alphasort);
    printf("Found %d files.\n\n", num_countries - 2);

    for (size_t i = 0; i < num_countries; i++)
    {
        if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
            continue;
        
        sprintf(buf, "%s/%s", input.root_dir.c_str(), namelist[i]->d_name);
        // send country of buf in monitor
        printf("%s\n", buf);

        // process_utils::send_message(comm_pipes, uint8_t msgid, data, buffer_size)
        free(namelist[i]);
    }

    free(namelist);
}


void process_utils::monitor::init_args(const structures::commPipes & comm_pipes, structures::Input & input)
{
    int data_fd = open(comm_pipes.data_pipe, O_RDONLY);

    // check later if signal can be sent for failing to read the right anount of bytes
    ssize_t ret = read(data_fd, &input.buffer_size, sizeof(input.buffer_size));
    ret = read(data_fd, &input.bf_size, sizeof(input.bf_size));
    size_t len = 0;
    ret = read(data_fd, &len, sizeof(len));
    char buffer[len] = {0};
    ret = read(data_fd, buffer, len);
    input.root_dir.assign(buffer);

    close(data_fd);
}


void process_utils::_wait_ack(const int & fd)
{
    struct pollfd fdarr[1];
    fdarr[0].fd = fd;
    fdarr[0].events = POLLIN;

    uint8_t response = REJECT;
    while (response != ACK)
    {
        int rc = poll(fdarr, 1, 300);
        while (rc != 1 || fdarr[0].revents != POLLIN)
            rc = poll(fdarr, 1, 300);
        
        ssize_t ret = read(fd, &response, sizeof(uint8_t));
    }
}


void process_utils::_write_and_wait_ack(const int & comm_fd, const int & data_fd, const uint8_t & msg_id, const size_t & bytes_out)
{
    ssize_t ret = write(comm_fd, &msg_id, sizeof(uint8_t));
    ret = write(data_fd, &bytes_out, sizeof(size_t));
    process_utils::_wait_ack(comm_fd);
}


void process_utils::_send_message(const structures::commPipes & comm_pipes, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size)
{
    char send_buf[buffer_size] = {0};
    int comm_fd = open(comm_pipes.coordination_pipe, O_RDWR | O_NONBLOCK);
    int data_fd = open(comm_pipes.data_pipe, O_WRONLY);

    /* write the message ID to inform the end process that we want to send a specific message */
    process_utils::_write_and_wait_ack(comm_fd, data_fd, msg_id, bytes_out);

    /* chop down the message in blocks of `buffer_size` bytes or less */
    size_t bytes_sent = 0;
    size_t bytes_to_write = 0;
    while (bytes_out > 0)
    {
        bytes_to_write = (bytes_out < buffer_size) ? bytes_out : buffer_size;
        memcpy(send_buf, buf + bytes_sent, bytes_to_write);

        /* wait for an "ACK" response from the end process in order to send the next block of bytes */
        process_utils::_wait_ack(comm_fd);
        ssize_t ret = write(data_fd, send_buf, bytes_to_write);

        bytes_out -= bytes_to_write;
        bytes_sent += bytes_to_write;
    }

    close(comm_fd);
    close(data_fd);
}


void process_utils::_receive_message(const structures::commPipes & comm_pipes, const uint8_t & msg_id, const char buf[])
{
    
}