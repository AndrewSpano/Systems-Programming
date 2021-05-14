#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>

#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/utils/comm_utils.hpp"
#include "../include/utils/process_utils.hpp"
#include "../include/utils/structures.hpp"
#include "../include/utils/queries.hpp"
#include "../include/data_structures/indices.hpp"


MonitorIndex* m_index;
structures::CommunicationPipes* pipes;

// #include <thread>
// #include <chrono>
// std::this_thread::sleep_for(std::chrono::milliseconds(x));


int main(int argc, char* argv[])
{
    /* arguments */
    pipes = new structures::CommunicationPipes;
    ErrorHandler handler;
    parsing::arguments::parse_monitor_args(argc, argv, pipes, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize key variables */
    structures::Input input;
    comm_utils::monitor::init_args(pipes, input);
    m_index = new MonitorIndex(input.bf_size);

    /* get the assigned countries */
    comm_utils::monitor::receive_countries(m_index, pipes, input);

    /* die if no countries were given */
    if (m_index->num_countries == 0)
    {
        delete[] pipes->input;
        delete[] pipes->output;
        delete pipes;
        delete m_index;

        return EXIT_SUCCESS;
    }

    /* parse the data and insert it in the data structures */
    process_utils::monitor::parse_countries(m_index, input.root_dir, handler);

    /* send the bloom filters to the travelMonitor */
    comm_utils::monitor::send_bloom_filters(m_index, pipes, input);


    /* open the pipes in order to accept commands */
    int input_fd = open(pipes->input, O_RDONLY | O_NONBLOCK);
    int output_fd = open(pipes->output, O_WRONLY);
    uint8_t msg_id = REJECT;
    char message[512] = {0};
    
    /* accept commands and execute them */
    // while (420 != 69)
    // {
        /* now wait until a command has been given */
        memset(message, 0, 512);
        comm_utils::monitor::wait_for_command(m_index, input_fd, output_fd, input, msg_id, message);

        /* determine which command was given */
        switch (msg_id)
        {
            case TRAVEL_REQUEST_SEND_DATA:
                queries::monitor::travel_request(m_index, input_fd, output_fd, input, message);
                break;
            case SEARCH_VACCINATION_STATUS_SEND_DATA:
                queries::monitor::search_vaccination_status(m_index, input_fd, output_fd, input, message);
                break;
            default:
                std::cout << "This code should have never been executed. Monitor::main()" << std::endl;
                break;
        }
    // }

    
    /* close the pipes */
    close(input_fd);
    close(output_fd);

    /* free allocated memory */
    delete[] pipes->input;
    delete[] pipes->output;
    delete pipes;
    delete m_index;

    std::cout << "Monitor Exiting!\n";
    return EXIT_SUCCESS;
}
