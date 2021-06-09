#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <poll.h>

#include "../include/ipc/ipc.hpp"
#include "../include/ipc/setup.hpp"
#include "../include/ipc/queries.hpp"
#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/utils/structures.hpp"
#include "../include/utils/thread_utils.hpp"
#include "../include/utils/process_utils.hpp"
#include "../include/data_structures/indices.hpp"



/* useful global variables */
structures::CyclicBuffer* cyclic_buffer;
structures::RaceConditions race_cond;
ErrorHandler handler;



int main(int argc, char* argv[])
{
    /* arguments */
    structures::MonitorInput input;
    parsing::arguments::parse_monitor_args(argc, argv, input, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;


    /* initialize key variables */
    MonitorIndex* m_index = new MonitorIndex(&input);
    structures::NetworkCommunication network_info(input.port);
    pthread_t pthread_ids[input.num_threads];
    cyclic_buffer = new structures::CyclicBuffer(input.cyclic_buffer_size);
    thread_utils::initialize_thread_variables();

    /* establish a connection with the parent process */
    process_utils::monitor::establish_connection(network_info);
    int fd = network_info.client_socket;

    /* create the threads and parse the data in a multithreaded manner */
    thread_utils::create_threads(pthread_ids, input.num_threads, m_index);
    thread_utils::produce(m_index);

    /* send the bloom filters to the parent process */
    ipc::monitor::setup::send_bloom_filters(m_index, fd, fd);


    /* accept commands */
    uint8_t msg_id = REJECT;
    char message[512] = {0};
    
    /* accept commands and execute them */
    while (msg_id != EXIT)
    {
        /* now wait until a command has been given */
        memset(message, 0, 512);
        ipc::wait_for_command(fd, fd, msg_id, message, input.socket_buffer_size);

        /* determine which command was given */
        switch (msg_id)
        {
            case TRAVEL_REQUEST_SEND_DATA:
                ipc::monitor::queries::travel_request(m_index, fd, fd, message);
                break;
            case ADD_VACCINATION_RECORDS_SEND_DATA:
                thread_utils::produce_new(m_index, message);
                ipc::monitor::queries::add_vaccination_records(m_index, fd, fd, message, handler);
                break;
            case SEARCH_VACCINATION_STATUS_SEND_DATA:
                ipc::monitor::queries::search_vaccination_status(m_index, fd, fd, message);
                break;
            case EXIT:
                thread_utils::exit_threads();
                break;
            default:
                std::cout << "This code should have never been executed. monitorServer::main()" << std::endl;
                break;
        }
    }

    /* wait for the threads to finish and then destroy the variables used for coordination */
    thread_utils::wait_for_threads(pthread_ids, input.num_threads);
    thread_utils::destroy_thread_variables();
    
    /* close the server socket */
    process_utils::monitor::close_connection(network_info);

    /* log information */
    m_index->logger->write_to_logfile();

    /* free allocated memory */
    delete m_index;
    delete cyclic_buffer;

    std::cout << "M: Exiting Monitor: " << getpid() << "\n";
    return EXIT_SUCCESS;
}
