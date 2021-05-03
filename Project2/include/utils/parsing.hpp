#ifndef _PARSING
#define _PARSING

#define CURRENT_YEAR 2021

#include <iostream>

#include "date.hpp"
#include "errors.hpp"
#include "structures.hpp"


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

    void parse_record_line(const std::string & line, ErrorHandler & handler);

    // namespace dataset
    // {
    //     void parse_dataset(const std::string& dataset_path, Index& index);
    // }


  namespace arguments
  {
    
    /**
     * Parse the command line arguments of the travelMonitor process. 
     * 
     * @param[in] argc
     *      The number of arguments provided when executing the travelMonitor process.
     * @param[in] argv
     *      The command line arguments.
     * @param[out] num_monitors
     *      The number of Monitor (child) processes to create.
     * @param[out] buffer_size
     *      The size (in bytes) of the buffer of the Named Pipes used for
     *      communication between the processes.
     * @param[out] bloom_filter_size
     *      The size (in bytes) of the Bloom Filter.
     * @param[out] root_dir
     *      The relative/absolute path of the root directory that will contain
     *      a sub-directory for each country.
     * @param[out] handler
     *      An error handler used to distinguish errors.
     */
    void parse_travel_monitor_args(const int & argc, char* argv[],
                                   structures::Input & input,
                                   ErrorHandler & handler);

    /**
     * Parse the command line arguments of the Monitor process.
     * 
     * @param[in] argc
     *    The number of arguments provided when executing the travelMonitor process.
     * @param[in] argv
     *    The command line arguments.
     * @param[out] pipe1_path
     *    The name (path) of the pipe used for coordination. (???)
     * @param[out] pipe2_path
     *    The name (path) of the pipe used to transfer data. (???)
     * @param[out] handler
     *    An error handler used to distinguish errors.
     */
    void parse_monitor_args(const int & argc, char* argv[], structures::CommunicationPipes & pipes, ErrorHandler & handler);
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
