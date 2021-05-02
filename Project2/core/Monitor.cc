#include <string>
#include <memory>
 
#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/utils/comm_utils.hpp"
#include "../include/utils/structures.hpp"


int main(int argc, char* argv[])
{
    /* arguments */
    structures::commPipes comm_pipes;
    ErrorHandler handler;
    parsing::arguments::parse_monitor_args(argc, argv, comm_pipes, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize key variables */
    structures::Input input;
    process_utils::monitor::init_args(comm_pipes, input);

    // int comm_fd = open(coordination_pipe.c_str(), O_RDWR | O_NONBLOCK);
    // int data_fd = open(data_pipe.c_str(), O_RDWR | O_NONBLOCK);
    // std::cout << "comm fd = " << comm_fd << std::endl;
    // std::cout << "data fd = " << data_fd << std::endl;
    
    return EXIT_SUCCESS;
}
