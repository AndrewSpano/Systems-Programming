#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>

#include "../include/ipc/ipc.hpp"
#include "../include/ipc/setup.hpp"
#include "../include/ipc/queries.hpp"
#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/utils/structures.hpp"
#include "../include/utils/process_utils.hpp"
#include "../include/data_structures/indices.hpp"



int main(int argc, char* argv[])
{
    /* arguments */
    ErrorHandler handler;
    structures::MonitorInput input;
    parsing::arguments::parse_monitor_args(argc, argv, input, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize key variables */
    MonitorIndex* m_index = new MonitorIndex(&input);
    structures::NetworkCommunication network_info(input.port);

    /* establish a connection with the parent process */
    process_utils::monitor::establish_connection(network_info);

    /* create threads */

    /* parse the bloom filters in multithreaded manner */

    /* send the bloom filters to the parent process */


    // /* parse the data and insert it in the data structures */
    // process_utils::monitor::parse_countries(m_index, input.root_dir, handler);

    // /* send the bloom filters to the travelMonitor */
    // ipc::monitor::setup::send_bloom_filters(m_index, pipes);


    // /* open the pipes in order to accept commands */
    // int input_fd = open(pipes->input, O_RDONLY | O_NONBLOCK);
    // int output_fd = open(pipes->output, O_RDWR);
    // uint8_t msg_id = REJECT;
    // char message[512] = {0};
    
    // /* accept commands and execute them */
    // while (420 != 69)
    // {
    //     /* now wait until a command has been given */
    //     memset(message, 0, 512);
    //     ipc::wait_for_command(m_index, input_fd, output_fd, msg_id, message);

    //     /* determine which command was given */
    //     switch (msg_id)
    //     {
    //         case TRAVEL_REQUEST_SEND_DATA:
    //             ipc::monitor::queries::travel_request(m_index, input_fd, output_fd, message);
    //             break;
    //         case SEARCH_VACCINATION_STATUS_SEND_DATA:
    //             ipc::monitor::queries::search_vaccination_status(m_index, input_fd, output_fd, message);
    //             break;
    //         default:
    //             std::cout << "This code should have never been executed. Monitor::main()" << std::endl;
    //             break;
    //     }
    // }
    
    // /* close the pipes */
    // close(input_fd);
    // close(output_fd);

    /* free allocated memory */
    delete m_index;

    return EXIT_SUCCESS;
}
