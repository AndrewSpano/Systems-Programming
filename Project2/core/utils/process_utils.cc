#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/process_utils.hpp"


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


void process_utils::travel_monitor::create_pipes(structures::CommunicationPipes pipes[], const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        _create_pipe(&pipes[i].input, "input", i);
        _create_pipe(&pipes[i].output, "output", i);        
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


void process_utils::travel_monitor::free_and_delete_pipes(structures::CommunicationPipes pipes[], const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        _free_and_delete_pipe(pipes[i].input);
        _free_and_delete_pipe(pipes[i].output);
    }
}


void process_utils::travel_monitor::open_all_pipes(const structures::CommunicationPipes pipes[], int comm_fds[], const mode_t & comm_perms,
                                                   int data_fds[], const mode_t & data_perms, const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        comm_fds[i] = open(pipes[i].input, comm_perms);
        data_fds[i] = open(pipes[i].output, data_perms);
    }
}


void process_utils::travel_monitor::close_all_pipes(const int comm_fds[], const int data_fds[], const uint16_t & num_monitors)
{
    for (size_t i = 0; i < num_monitors; i++)
    {
        close(comm_fds[i]);
        close(data_fds[i]);
    }
}


void process_utils::travel_monitor::create_monitors(pid_t monitor_pids[], structures::CommunicationPipes pipes[], const u_int16_t & num_monitors)
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
            const char* const argv[] = {"bin/Monitor", "-i", pipes[i].output, "-o", pipes[i].input, NULL};
            execvp(argv[0], const_cast<char* const*>(argv));
            perror("execvp() failed in process_utils::create_monitors()");
            exit(-1);
        }
    }
}
