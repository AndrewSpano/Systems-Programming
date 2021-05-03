#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../include/utils/parsing.hpp"
#include "../../include/utils/macros.hpp"



void parsing::arguments::parse_travel_monitor_args(const int & argc, char* argv[], structures::Input & input, ErrorHandler & handler)
{
    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {
        handler.status = HELP_TRAVEL_MONITOR;
        return;
    }
    else if (argc != 9)
    {
        handler.status = INVALID_NUM_ARGS_TRAVEL_MONITOR;
        handler.invalid_value = std::to_string(argc);
        return;
    }

    bool flag_m = false;
    bool flag_b = false;
    bool flag_s = false;
    bool flag_i = false;

    for (size_t i = 1; i < 9; i += 2)
    {
        std::string flag(argv[i]);
        std::string value(argv[i + 1]);

        if (flag == "-m")
        {
            if (!parsing::utils::is_valid_numerical(value) || stoi(value) == 0)
            {
                handler.status = INVALID_NUM_MONITORS;
                handler.invalid_value = value;
                return;
            }
            input.num_monitors = stoi(value);
            flag_m = true;
        }
        else if (flag == "-b")
        {
            if (!parsing::utils::is_valid_numerical(value) || stoi(value) == 0)
            {
                handler.status = INVALID_BUFFER_SIZE;
                handler.invalid_value = value;
                return;
            }
            input.buffer_size = stoi(value);
            flag_b = true;
        }
        else if (flag == "-s")
        {
            if (!parsing::utils::is_valid_numerical(value) || stoi(value) == 0)
            {
                handler.status = INVALID_BLOOM_FILTER_SIZE;
                handler.invalid_value = value;
                return;
            }
            input.bf_size = stoi(value);
            flag_s = true;
        }
        else if (flag == "-i")
        {
            if (!parsing::utils::is_directory(value))
            {
                handler.status = INVALID_ROOT_DIR;
                handler.invalid_value = value;
                return;
            }
            input.root_dir = value;
            flag_i = true;
        }
        else
        {
            handler.status = INVALID_FLAG;
            handler.invalid_value = flag;
            return;
        }
    }

    if (!flag_m || !flag_b || !flag_s || !flag_i)
    {
        handler.status = INVALID_ARGS;
        std::string missing_args = " ";
        if (!flag_m) missing_args += "-m ";
        if (!flag_b) missing_args += "-b ";
        if (!flag_s) missing_args += "-s ";
        if (!flag_i) missing_args += "-i ";
        handler.invalid_value = missing_args.replace(missing_args.length() - 1, 1, ".");
    }
}



void parsing::arguments::parse_monitor_args(const int & argc, char* argv[], structures::CommunicationPipes & pipes, ErrorHandler & handler)
{
    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {
        handler.status = HELP_MONITOR;
        return;
    }
    else if (argc != 5)
    {
        handler.status = INVALID_NUM_ARGS_MONITOR;
        handler.invalid_value = std::to_string(argc);
        return;
    }

    bool flag_cp = false;
    bool flag_dp = false;
    struct stat sb;

    for (size_t i = 1; i < 5; i += 2)
    {
        std::string flag(argv[i]);
        std::string value(argv[i + 1]);

        if (flag == "-i")
        {
            if (stat(value.c_str(), &sb) == -1 || !S_ISFIFO(sb.st_mode))
            {
                handler.status = INVALID_INPUT_PIPE;
                handler.invalid_value = value;
                return;
            }
            pipes.input = new char[value.length() + 1];
            strcpy(pipes.input, value.c_str());
            flag_cp = true;
        }
        else if (flag == "-o")
        {
            if (stat(value.c_str(), &sb) == -1 || !S_ISFIFO(sb.st_mode))
            {
                handler.status = INVALID_OUTPUT_PIPE;
                handler.invalid_value = value;
                return;
            }
            pipes.output = new char[value.length() + 1];
            strcpy(pipes.output, value.c_str());
            flag_dp = true;
        }
        else
        {
            handler.status = INVALID_FLAG;
            handler.invalid_value = flag;
            return;
        }
    }

    if (!flag_cp || !flag_dp)
    {
        handler.status = INVALID_ARGS;
        handler.invalid_value = (!flag_cp) ? " -c." : " -d.";
    }
}



void parsing::parse_record_line(const std::string & line, ErrorHandler & handler)
{
    size_t start = 0, end = 0;

    /* parse the citizen ID */
    parsing::utils::parse_next_substring(line, start, end);
    std::string id = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_numerical(id)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* parse the name */
    parsing::utils::parse_next_substring(line, start, end);
    std::string name = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_alphabetical(name)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* parse the surname */
    parsing::utils::parse_next_substring(line, start, end);
    std::string surname = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_alphabetical(surname)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* parse the country */
    parsing::utils::parse_next_substring(line, start, end);
    std::string country = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_alphabetical(country)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* parse the age */
    parsing::utils::parse_next_substring(line, start, end);
    std::string _age = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_numerical(_age)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    uint8_t age = stoi(_age);
    start = end;

    /* create a Record from the existing data */
    // Record* new_record = new Record(id, name, surname, country, age);
    /* scan all the existing records to see if a Record with the same ID exists */
    // Record* same_id_record = index.records_list->get(id);

    /*
    CASES:
        1. A Record with the same ID but a different field exists -> ERROR, continue
        2. Input after Record (virus, YES/NO, date) is incorrect -> ERROR, continue
        3. Same Record exists, vaccination data is contradictory to new data -> ERROR, continue
    */

    /* 1. if a record with the same ID exists, and the new record is incompatible with it, continue */
    // if (same_id_record && !parsing::processing::is_valid_new_record(new_record, same_id_record))
    //     DELETE_LOG_AND_RETURN(new_record, line)

    /* 2. parse the virusName */
    parsing::utils::parse_next_substring(line, start, end);
    std::string virus_name = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_alphanumerical(virus_name, true)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* 2. parse the status */
    parsing::utils::parse_next_substring(line, start, end);
    std::string status = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_status(status)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
    start = end;

    /* 3. check if current record contradicts vaccination data for existing record with same ID */
    // if (same_id_record && index.virus_list->exists_in_virus_name(new_record->id, virus_name,
    //                                                             false, false))
    //     DELETE_LOG_AND_RETURN(new_record, line)

    /* 2. if the status is "NO", make sure that this is the last string of the line and continue */
    if (status == "NO")
    {
        /* check for the status being the last string */
        parsing::utils::parse_next_substring(line, start, end);
        std::string dummy = line.substr(start, end - start);
        if (end != 0) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
        // DELETE_LOG_AND_RETURN(new_record, line)

        /* if we get here the Record is legit, add it to the data structures */
        // index.insert(same_id_record, new_record, virus_name, status);
    }

    /* 2. if the status is "YES", parse the data and make sure that it is the last string */
    else
    {
        /* parse the date, skip the record if an error occurrs */
        parsing::utils::parse_next_substring(line, start, end);
        std::string date = line.substr(start, end - start);
        if (end == 0 || !parsing::utils::is_valid_date(date)) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
        // DELETE_LOG_AND_RETURN(new_record, line)
        start = end;

        /* make sure the date was the last string */
        parsing::utils::parse_next_substring(line, start, end);
        std::string dummy = line.substr(start, end - start);
        if (end != 0) HANDLE_AND_RETURN(handler, INVALID_RECORD, line)
        // DELETE_LOG_AND_RETURN(new_record, line)

        /* if we get here the Record is legit, add it to the data structures */
        // index.insert(same_id_record, new_record, virus_name, status, date);
    }
}



int parsing::user_input::get_option(std::string & line, const bool & _print_help)
{
    size_t start = 0;
    size_t end = 0;

    if (_print_help)
        parsing::user_input::print_options();

    while (4 + 20 != 420)
    {
        std::cout << "Enter your command: ";
        std::getline(std::cin, line);

        parsing::utils::parse_next_substring(line, start, end);
        std::string command = line.substr(start, end - start);

        if (command == "/travelRequest") return 1;
        else if (command == "/travelStats") return 2;
        else if (command == "/addVaccinationRecords") return 3;
        else if (command == "/searchVaccinationStatus") return 4;
        else if (command == "/exit") return 0;
        else if (command == "/help") parsing::user_input::print_options();
        else std::cout << std::endl << "Unknown command: " << command << std::endl << std::endl;
    }
}


void parsing::user_input::parse_travel_request(const std::string & line,
                                               std::string & citizen_id,
                                               Date & date,
                                               std::string & country_from,
                                               std::string & country_to,
                                               std::string & virus_name,
                                               ErrorHandler & handler)
{
    size_t start = 0;
    size_t end = 0;

    /* skip the command sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    start = end;

    /* parse the citizen ID */
    parsing::utils::parse_next_substring(line, start, end);
    citizen_id = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_numerical(citizen_id)) HANDLE_AND_RETURN(handler, INVALID_CITIZEN_ID, line)
    start = end;

    /* parse the date */
    parsing::utils::parse_next_substring(line, start, end);
    std::string date_str = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_date(date_str)) HANDLE_AND_RETURN(handler, INVALID_DATE, line)
    date.construct(date_str);
    start = end;

    /* parse the "from country" */
    parsing::utils::parse_next_substring(line, start, end);
    country_from = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphabetical(country_from)) HANDLE_AND_RETURN(handler, INVALID_COUNTRY_FROM, line)
    start = end;

    /* parse the "to country" */
    parsing::utils::parse_next_substring(line, start, end);
    country_to = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphabetical(country_to)) HANDLE_AND_RETURN(handler, INVALID_COUNTRY_TO, line)
    start = end;

    /* parse the virus name */
    parsing::utils::parse_next_substring(line, start, end);
    virus_name = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphanumerical(virus_name, true)) HANDLE_AND_RETURN(handler, INVALID_VIRUS_NAME, line)
    start = end;

    /* make sure that was the last sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    if (end) HANDLE_AND_RETURN(handler, INVALID_NUM_PARAMETERS, line)
}


void parsing::user_input::parse_travel_stats(const std::string & line,
                                             std::string & virus_name,
                                             Date & date1,
                                             Date & date2,
                                             std::string & country,
                                             ErrorHandler & handler)
{
    size_t start = 0;
    size_t end = 0;

    /* skip the command sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    start = end;

    /* parse the virus name */
    parsing::utils::parse_next_substring(line, start, end);
    virus_name = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphanumerical(virus_name, true)) HANDLE_AND_RETURN(handler, INVALID_VIRUS_NAME, line)
    start = end;

    /* parse the first date */
    parsing::utils::parse_next_substring(line, start, end);
    std::string date_str = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_date(date_str)) HANDLE_AND_RETURN(handler, INVALID_DATE1, line)
    date1.construct(date_str);
    start = end;

    /* parse the second date */
    parsing::utils::parse_next_substring(line, start, end);
    date_str = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_date(date_str)) HANDLE_AND_RETURN(handler, INVALID_DATE2, line)
    date2.construct(date_str);
    start = end;

    /* make sure that date2 occurs "later" than date 1 */
    if (date2 < date1) HANDLE_AND_RETURN(handler, INVALID_CONSISTENT_DATES, line)

    /* if the end of the line has been reached, it is fine: return; else, the country is the next parameter */
    parsing::utils::parse_next_substring(line, start, end);
    if (!end) return;

    country = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphabetical(country)) HANDLE_AND_RETURN(handler, INVALID_COUNTRY, line)
    start = end;

    /* make sure that was the last sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    if (end) HANDLE_AND_RETURN(handler, INVALID_NUM_PARAMETERS, line)
}


void parsing::user_input::parse_add_vaccination_records(const std::string & line,
                                                        std::string & country,
                                                        ErrorHandler & handler)
{
    size_t start = 0;
    size_t end = 0;

    /* skip the command sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    start = end;

    /* parse the country */
    parsing::utils::parse_next_substring(line, start, end);
    country = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_alphabetical(country)) HANDLE_AND_RETURN(handler, INVALID_COUNTRY, line)
    start = end;

    /* make sure that was the last sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    if (end) HANDLE_AND_RETURN(handler, INVALID_NUM_PARAMETERS, line)
}


void parsing::user_input::parse_search_vaccination_status(const std::string & line,
                                                          std::string & citizen_id,
                                                          ErrorHandler & handler)
{
    size_t start = 0;
    size_t end = 0;

    /* skip the command sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    start = end;

    /* parse the citizen ID */
    parsing::utils::parse_next_substring(line, start, end);
    citizen_id = line.substr(start, end - start);
    if (!end || !parsing::utils::is_valid_numerical(citizen_id)) HANDLE_AND_RETURN(handler, INVALID_CITIZEN_ID, line)
    start = end;

    /* make sure that was the last sub-string */
    parsing::utils::parse_next_substring(line, start, end);
    if (end) HANDLE_AND_RETURN(handler, INVALID_NUM_PARAMETERS, line)
}

