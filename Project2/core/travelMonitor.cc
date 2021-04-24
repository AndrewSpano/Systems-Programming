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
    uint16_t num_monitors = 0;
    uint64_t buffer_size = 0;
    uint64_t bloom_filter_size = 0;
    std::string root_dir = "";
    ErrorHandler handler;
    parsing::arguments::parse_travel_monitor_args(argc, argv, num_monitors, buffer_size, bloom_filter_size, root_dir, handler);
    if (handler.status == HELP_TRAVEL_MONITOR) { handler.print_help_travel_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;

    /* initialize the seed for the SkipList */
    srand(time(NULL));

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
