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
#include "../include/utils/structures.hpp"


#include <thread>
#include <chrono>
// std::this_thread::sleep_for(std::chrono::milliseconds(x));


int main(int argc, char* argv[])
{
    /* arguments */
    structures::CommunicationPipes pipes;
    ErrorHandler handler;
    parsing::arguments::parse_monitor_args(argc, argv, pipes, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize key variables */
    structures::Input input;
    comm_utils::monitor::init_args(pipes, input);

    /* get the assigned countries */
    comm_utils::monitor::receive_countries(pipes, input);


    /* free allocated memory for the Named Pipe names */
    delete[] pipes.input;
    delete[] pipes.output;

    return EXIT_SUCCESS;
}
