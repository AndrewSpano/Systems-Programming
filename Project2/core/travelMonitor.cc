#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <fcntl.h>

#include "../include/utils/errors.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/ipc/ipc.hpp"
#include "../include/utils/structures.hpp"
#include "../include/utils/process_utils.hpp"
#include "../include/ipc/queries.hpp"
#include "../include/data_structures/indices.hpp"


travelMonitorIndex* tm_index;
structures::CommunicationPipes* pipes;
pid_t* monitor_pids;


// #include <thread>
// #include <chrono>
// std::this_thread::sleep_for(std::chrono::milliseconds(x));


int main(int argc, char* argv[])
{
    /* arguments */
    structures::Input input;
    ErrorHandler handler;
    parsing::arguments::parse_travel_monitor_args(argc, argv, input, handler);
    if (handler.status == HELP_TRAVEL_MONITOR) { handler.print_help_travel_monitor(); return EXIT_SUCCESS; }
    else if (handler.check_and_print()) return EXIT_FAILURE;


    /* initialize important variables */
    tm_index = new travelMonitorIndex(&input);

    /* create two named pipes for each child process: 1 for coordination and 1 for data tranfer */
    pipes = new structures::CommunicationPipes[input.num_monitors];
    process_utils::travel_monitor::create_pipes(pipes, input.num_monitors);

    /* create the monitor processes and send the appropriate values */
    monitor_pids = new pid_t[input.num_monitors];
    process_utils::travel_monitor::create_monitors(monitor_pids, pipes, input.num_monitors);
    ipc::travel_monitor::send_args(pipes, input);

    /* assign countries to each monitor */
    ipc::travel_monitor::assign_countries(tm_index, pipes);

    /* receive the bloom filters (per virus) from the monitors */
    ipc::travel_monitor::receive_bloom_filters(tm_index, pipes);


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
                bool was_accepted = false;
                structures::TRData tr_data(citizen_id, &date, country_from, country_to, virus_name);

                ipc::travel_monitor::queries::travel_request(tm_index, pipes, tr_data, handler, was_accepted);

                if (!handler.check_and_print())
                {
                    structures::TRQuery* query = new structures::TRQuery(&date, country_from, virus_name, was_accepted);
                    tm_index->logger->insert(query);
                }
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
                structures::TSData ts_data(virus_name, &date1, &date2, country);
                ipc::travel_monitor::queries::travel_stats(tm_index, ts_data, handler);
                handler.check_and_print();
            }
        }
        else if (command == 3)
        {
            std::string country = "";

            parsing::user_input::parse_add_vaccination_records(line, country, handler);
            if (!handler.check_and_print())
            {
                ipc::travel_monitor::queries::add_vaccination_records(tm_index, monitor_pids, pipes, country, handler);
                handler.check_and_print();
            }
        }
        else if (command == 4)
        {
            std::string citizen_id = "";

            parsing::user_input::parse_search_vaccination_status(line, citizen_id, handler);
            if (!handler.check_and_print())
            {
                ipc::travel_monitor::queries::search_vaccination_status(tm_index, pipes, citizen_id);
            }
        }

        /* get the next command */
        std::cout << std::endl;
        command = parsing::user_input::get_option(line);
    }

    /* kill all the monitors, then make sure they have died, write to the logfiles and cleanup the allocated memory */
    process_utils::travel_monitor::kill_minitors_and_wait(monitor_pids, tm_index, input);
    tm_index->logger->write_to_logfile();
    process_utils::travel_monitor::cleanup(tm_index, pipes, monitor_pids);

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
