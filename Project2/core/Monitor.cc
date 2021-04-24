#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"


int main(int argc, char* argv[])
{
    std::cout  << std::endl;

    /* arguments */
    std::string pipe1_path = "";
    std::string pipe2_path = "";
    ErrorHandler handler;
    parsing::arguments::parse_monitor_args(argc, argv, pipe1_path, pipe2_path, handler);
    if (handler.status == HELP_MONITOR) { handler.print_help_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize the seed for the SkipList */
    srand(time(NULL));

    std::cout << "peosss\n";
    

    return EXIT_SUCCESS;
}
