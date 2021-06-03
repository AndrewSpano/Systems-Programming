#ifndef _PARSING
#define _PARSING

#define CURRENT_YEAR 2021

#include <iostream>

#include "date.hpp"
#include "errors.hpp"
#include "structures.hpp"
#include "../data_structures/indices.hpp"


namespace parsing
{

    namespace utils
    {
        bool is_whitespace(const char& c);
        void parse_next_substring(const std::string & str, size_t & start, size_t & end);

        bool is_digit(const char & c);
        bool is_letter(const char & c);
        bool is_hyphen(const char & c);
        bool is_valid_numerical(const std::string & str, const bool & allow_hyphen=false);
        bool is_valid_alphabetical(const std::string & str, const bool & allow_hyphen=false);
        bool is_valid_alphanumerical(const std::string & str, const bool & allow_hyphen=false);
        bool is_valid_status(const std::string & str);
        bool is_valid_date(const std::string & str);
        bool is_directory(const std::string & path);

    }

    void parse_record_line(std::string* country, const std::string & line, MonitorIndex* m_index, ErrorHandler & handler);

    namespace dataset
    {
        void parse_country_dataset(std::string* country, const std::string & dataset_path, MonitorIndex* m_index, ErrorHandler & handler);
    }


  namespace arguments
  {
    void parse_travel_monitor_args(const int & argc, char* argv[], structures::travelMonitorInput & input, ErrorHandler & handler);
    void parse_monitor_args(const int & argc, char* argv[], structures::MonitorInput & input, ErrorHandler & handler);
  }


  namespace user_input
  {
    void print_options(void);
    int get_option(std::string & line, const bool & _print_help=false);

    void parse_travel_request(const std::string & line,
                              std::string & citizen_id,
                              Date & date,
                              std::string & country_from,
                              std::string & country_to,
                              std::string & virus_name,
                              ErrorHandler & handler);

    void parse_travel_stats(const std::string & line,
                            std::string & virus_name,
                            Date & date1,
                            Date & date2,
                            std::string & country,
                            ErrorHandler & handler);

    void parse_add_vaccination_records(const std::string & line,
                                       std::string & country,
                                       ErrorHandler & handler);

    void parse_search_vaccination_status(const std::string & line,
                                         std::string & citizen_id,
                                         ErrorHandler & handler);
  }
}


#endif
