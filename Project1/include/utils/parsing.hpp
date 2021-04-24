#ifndef _PARSING
#define _PARSING

#include "../data_structures/index.hpp"

#define CURRENT_YEAR 2021

struct Index;
typedef struct Index Index;

namespace parsing
{

  namespace utils
  {
    bool is_whitespace(const char& c);
    void parse_next_substring(const std::string& str, size_t& start, size_t& end);

    bool is_digit(const char& c);
    bool is_letter(const char& c);
    bool is_dash(const char& c);
    bool is_valid_numerical(const std::string& id, const bool& allow_dash=false);
    bool is_valid_alphabetical(const std::string& name, const bool& allow_dash=false);
    bool is_valid_alphanumerical(const std::string& str, const bool& allow_dash=false);
    bool is_valid_status(const std::string& status);
    bool is_valid_date(const std::string& str);

    bool date2_is_later_than_date1(const std::string& date1, const std::string& date2);

    bool file_exists(const std::string& path);

  }

  void parse_record_line(const std::string& line, Index& index);

  namespace dataset
  {
    void parse_dataset(const std::string& dataset_path, Index& index);
  }

  namespace arguments
  {
    bool parse_arguments(const int& argc, char* argv[],
                         std::string& dataset_path, uint64_t& bloom_filter_size);
    void print_help(void);
  }

  namespace user_input
  {
    int get_option(std::string& line, const bool& _print_help);
    void print_options(void);

    bool parse_vaccine_status(const std::string& line,
                              std::string& citizen_id,
                              std::string& virus_name,
                              const bool& needs_virus_name);
    bool parse_population_status(const std::string& line,
                                 std::string& country,
                                 std::string& virus_name,
                                 std::string& date1,
                                 std::string& date2);
    bool parse_insert_vaccinate(const std::string& line,
                                std::string& citizen_id,
                                std::string& first_name,
                                std::string& last_name,
                                std::string& country,
                                uint8_t& age,
                                std::string& virus_name,
                                const bool& needs_status_and_date,
                                bool& status,
                                std::string& date);
    bool parse_non_vaccinated_persons(const std::string& line,
                                      std::string& virus_name);
  }
}



#endif
