#ifndef _PARSING
#define _PARSING

#include "../data_structures/index.hpp"


#define LOG_AND_RETURN(line)                             \
{                                                        \
  std::cout << "ERROR IN RECORD " << line << std::endl;  \
  return;                                                \
}

#define DELETE_LOG_AND_RETURN(record, line)              \
{                                                        \
  std::cout << "ERROR IN RECORD " << line << std::endl;  \
  delete record;                                         \
  return;                                                \
}


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

    bool is_valid_new_record(Record* new_record, Record* existing_record);
  }

  void parse_record_line(const std::string& line, Index& index);

  namespace dataset
  {
    void parse_dataset(const std::string& dataset_path, Index& index);
  }

  namespace arguments
  {
    bool parse_arguments(const int& argc, const char* argv[], std::string& dataset_path, uint64_t& bloom_filter_size);
    void print_help(void);
  }
}



#endif
