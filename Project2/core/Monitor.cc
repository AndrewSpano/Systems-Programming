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


    std::string path = "bash/generated_data/root_dir/Albania/Albania-1.txt";
    std::string* c = new std::string("Albania");
    parsing::dataset::parse_country_dataset(c, path, m_index, handler);
    path = "bash/generated_data/root_dir/Albania/Albania-2.txt";
    parsing::dataset::parse_country_dataset(c, path, m_index, handler);

    /* free allocated memory */
    delete[] pipes->input;
    delete[] pipes->output;
    delete pipes;
    delete m_index;

    return EXIT_SUCCESS;
}
