#include <iostream>
#include <string>
#include <memory>
#include <cstring>

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
    structures::Input input;
    ErrorHandler handler;
    parsing::arguments::parse_travel_monitor_args(argc, argv, input, handler);
    if (handler.status == HELP_TRAVEL_MONITOR) { handler.print_help_travel_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* create two named pipes for each child process: 1 for coordination and 1 for data tranfer */
    pid_t monitor_pids[input.num_monitors] = {0};
    structures::commPipes comm_pipes[input.num_monitors];
    process_utils::travel_monitor::create_pipes(comm_pipes, input.num_monitors);

    /* create the monitor processes and send the appropriate values */
    process_utils::travel_monitor::create_monitors(monitor_pids, comm_pipes, input.num_monitors);
    process_utils::travel_monitor::send_args(comm_pipes, input);

    /* assign countries to each monitor */
    process_utils::travel_monitor::assign_countries(comm_pipes, input);

    int returnStatus;
    while (wait(&returnStatus) > 0);

    process_utils::travel_monitor::free_and_delete_pipes(comm_pipes, input.num_monitors);


    std::cout << "\n\nTravel Monitor Exiting!\n\n";
    return 0;









    /* get an option from the user for which command to execute */
    std::string line = "";
    int command = parsing::user_input::get_option(line, true);

    /* iterate until user gives the "/exit" command */
    while (command)
    {
        std::cout << std::endl;

        /* distinguish which command the user wants to use, and execute it if it's format is correct */
        if (command == 1)
        {
            std::string citizen_id = "";
            Date date;
            std::string country_from = "";
            std::string country_to = "";
            std::string virus_name = "";

            parsing::user_input::parse_travel_request(line, citizen_id, date, country_from, country_to, virus_name, handler);
            if (!handler.check_and_print())
            {
                // execute query
            }
        }
        else if (command == 2)
        {
            std::string virus_name = "";
            Date date1;
            Date date2;
            std::string country = "";

            parsing::user_input::parse_travel_stats(line, virus_name, date1, date2, country, handler);
            if (!handler.check_and_print())
            {
                // execute query
            }
        }
        else if (command == 3)
        {
            std::string country = "";

            parsing::user_input::parse_add_vaccination_records(line, country, handler);
            if (!handler.check_and_print())
            {
                // execute query
            }
        }
        else if (command == 4)
        {
            std::string citizen_id = "";

            parsing::user_input::parse_search_vaccination_status(line, citizen_id, handler);
            if (!handler.check_and_print())
            {
                // execute query
            }
        }

        /* get the next command */
        // std::cout << std::endl;
        command = parsing::user_input::get_option(line);
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
