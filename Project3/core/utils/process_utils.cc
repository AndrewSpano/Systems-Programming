#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../../include/utils/parsing.hpp"
#include "../../include/utils/process_utils.hpp"
#include "../../include/utils/network_utils.hpp"




void process_utils::travel_monitor::enumerate_countries(travelMonitorIndex* tm_index)
{
    char path[256] = {0};
    struct dirent **namelist;
    int num_countries = scandir(tm_index->input->root_dir.c_str(), &namelist, NULL, alphasort);
    tm_index->init_countries(num_countries - 2);
    size_t country_id = 0;

    for (size_t i = 0; i < num_countries; i++)
    {
        if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
            tm_index->countries[country_id++] = std::string(namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);
}



void process_utils::travel_monitor::create_addresses(structures::NetworkCommunication* network_info, const uint16_t & num_monitors)
{
    /* get the hostname of the current machine */
    char hostname[128] = {0};
    if (gethostname(hostname, 128) < 0)
        utils::perror_exit("gethostname() @ process_utils::travel_monitor::create_addresses()");
    struct hostent* foundhost = gethostbyname(hostname);

    /* define a specific port for each monitor: start from one port and increment for the next monitor */
    int port = rand() % 25000 + 6900;
    for (size_t i = 0; i < num_monitors; i++)
    {
        network_info[i].port = port + i;
        network_utils::init_address(&network_info[i].server_address, port + i, foundhost);
    }
}



void process_utils::travel_monitor::create_monitors(pid_t monitor_pids[], structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index)
{
    /* convert key variables to strings */
    char num_threads_as_str[16] = {0}, socket_buffer_size_as_str[16] = {0}, cyclic_buffer_size_as_str[16] = {0}, bloom_filter_size_as_str[16] = {0};
    sprintf(num_threads_as_str, "%hu", tm_index->input->num_threads);
    sprintf(socket_buffer_size_as_str, "%lu", tm_index->input->socket_buffer_size);
    sprintf(cyclic_buffer_size_as_str, "%hu", tm_index->input->cyclic_buffer_size);
    sprintf(bloom_filter_size_as_str, "%lu", tm_index->input->bf_size);

    /* iterate to create each monitor separately */
    for (size_t i = 0; i < tm_index->input->num_monitors; i++)
    {
        monitor_pids[i] = fork();
        if (monitor_pids[i] < 0)
        {
            utils::perror_exit("fork() @ process_utils::travel_monitor::create_monitors()");
        }
        else if (monitor_pids[i] == 0)
        {
            char port_as_str[16] = {0};
            sprintf(port_as_str, "%d", network_info[i].port);
            
            uint16_t num_countries_of_monitor = tm_index->num_countries_of_monitor(i);
            size_t num_arguments = 11 + num_countries_of_monitor + 1;

            /* executable arguments */
            char** argv = new char*[num_arguments];

            /* standard arguments to be copied in the arguments of the executable */
            const char* const _argv[] = {"bin/monitorServer", "-p", port_as_str,
                                                              "-t", num_threads_as_str,
                                                              "-b", socket_buffer_size_as_str,
                                                              "-c", cyclic_buffer_size_as_str,
                                                              "-s", bloom_filter_size_as_str};
            /* copy them */
            for (size_t i = 0; i < 11; i++)
            {
                argv[i] = new char[32];
                sprintf(argv[i], "%s", _argv[i]);
            }
            
            /* countries paths arguments */
            std::string countries[num_countries_of_monitor];
            tm_index->get_countries_of_monitor(countries, i);
            for (size_t arg = 11; arg < num_arguments - 1; arg++)
            {
                argv[arg] = new char[256];
                sprintf(argv[arg], "%s/%s", tm_index->input->root_dir.c_str(), countries[arg - 11].c_str());
            }

            /* NULL */
            argv[num_arguments - 1] = NULL;

            /* execute the Monitor Server */
            execvp(argv[0], const_cast<char* const*>(argv));
            utils::perror_exit("execvp() @ process_utils::travel_monitor::create_monitors()");
        }
    }
}



void process_utils::travel_monitor::create_connections(structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index)
{
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    for (size_t i = 0; i < active_monitors; i++)
        network_info[i].client_socket = network_utils::create_socket_and_connect(&network_info[i].server_address);
}



void process_utils::travel_monitor::close_connections(structures::NetworkCommunication* network_info, travelMonitorIndex* tm_index)
{
    size_t active_monitors = (tm_index->input->num_monitors <= tm_index->num_countries) ? tm_index->input->num_monitors : tm_index->num_countries;
    for (size_t i = 0; i < active_monitors; i++)
        if (close(network_info[i].client_socket) < 0)
            utils::perror_exit("close() @ process_utils::travel_monitor::close_connections()");
}



void process_utils::monitor::establish_connection(structures::NetworkCommunication & network_info)
{
    /* get the hostname of the current machine */
    char hostname[128] = {0};
    if (gethostname(hostname, 128) < 0)
        utils::perror_exit("gethostname() @ process_utils::monitor::establish_connection()");
    struct hostent* foundhost = gethostbyname(hostname);

    /* initialize the address information of the Monitor server */
    network_utils::init_address(&network_info.server_address, network_info.port, foundhost);

    /* create a socket for the server */
    network_info.server_socket = network_utils::create_server_socket(&network_info.server_address, 5);

    /* accept the connection from the client */
    network_info.client_socket = network_utils::accept_connection(network_info.server_socket);
}



void process_utils::monitor::close_connection(structures::NetworkCommunication & network_info)
{
    if (close(network_info.server_socket) < 0)
        utils::perror_exit("close() @ process_utils::travel_monitor::close_connections()");
}



// void process_utils::monitor::parse_countries(MonitorIndex* m_index, const std::string & root_dir, ErrorHandler & handler)
// {
//     for (size_t country_id = 0; country_id < m_index->num_countries; country_id++)
//     {
//         std::string* country = &(m_index->countries[country_id]);
//         char country_dir_path[256] = {0};
//         sprintf(country_dir_path, "%s/%s", root_dir.c_str(), country->c_str());

//         struct dirent **namelist;
//         int num_files = scandir(country_dir_path, &namelist, NULL, alphasort);

//         for (size_t i = 0; i < num_files; i++)
//         {
//             if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
//             {
//                 std::string* filename = new std::string(namelist[i]->d_name);
//                 char path[256];
//                 sprintf(path, "%s/%s", country_dir_path, filename->c_str()); 

//                 parsing::dataset::parse_country_dataset(country, path, m_index, handler);
//                 m_index->files_per_country[country_id]->insert(filename);
//             }
//             free(namelist[i]);
//         }
//         free(namelist);
//     }
// }
