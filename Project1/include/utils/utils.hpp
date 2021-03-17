#ifndef _PROCESSING_UTILS
#define _PROCESSING_UTILS

#include <iostream>
#include "../data_structures/record.hpp"


namespace parsing
{
  namespace processing
  {
    bool is_valid_new_record(Record* new_record, Record* existing_record);
  }
}


namespace utils
{
  namespace processing
  {
    bool date_is_between_dates(const std::string& mid_date,
                               const std::string& date1,
                               const std::string& date2);
  }

  namespace interface
  {
    namespace output
    {
      void print_population_status_output(const int& country_id,
                                          uint64_t* vaccinated_count,
                                          const bool& considered_dates,
                                          uint64_t* vaccinated_dates_count,
                                          uint64_t* non_vaccinated_count,
                                          const std::string& country);

      void print_population_status_by_age_output(const int& country_id,
                                                 uint64_t** vaccinated_date_count_by_age,
                                                 uint64_t** total_vaccinated_count_by_age,
                                                 const std::string& country);
    }
  }
}


#endif
