#include <iostream>
#include <iomanip>
#include "../../include/utils/utils.hpp"


bool utils::processing::date_is_between_dates(const std::string& mid_date,
                                              const std::string& date1,
                                              const std::string& date2)
{
  uint16_t mid_year = stoi(mid_date.substr(5, 4));
  uint16_t year1 = stoi(date1.substr(5, 4));
  uint16_t year2 = stoi(date2.substr(5, 4));

  if (year1 < mid_year && mid_year < year2)
    return true;
  else if (year1 > mid_year || mid_year > year2)
    return false;
  else
  {
    uint8_t mid_month = stoi(mid_date.substr(3, 2));
    uint8_t month1 = stoi(date1.substr(3, 2));
    uint8_t month2 = stoi(date2.substr(3, 2));

    if (month1 < mid_month && mid_month < month2)
      return true;
    else if (month1 > mid_month || mid_month > month2)
      return false;
    else
    {
      uint8_t mid_days = stoi(mid_date.substr(0, 2));
      uint8_t days1 = stoi(date1.substr(0, 2));
      uint8_t days2 = stoi(date2.substr(0, 2));

      return days1 <= mid_days && mid_days <= days2;
    }
  }
}


void utils::interface::output::print_population_status_output(const int& country_id,
                                                              uint64_t* vaccinated_count,
                                                              const bool& considered_dates,
                                                              uint64_t* vaccinated_dates_count,
                                                              uint64_t* non_vaccinated_count,
                                                              const std::string& country)
{
  uint64_t vaccinated = (considered_dates) ? vaccinated_dates_count[country_id]
                                           : vaccinated_count[country_id];
  double percentage = 0.0;
  if (vaccinated_count[country_id] + non_vaccinated_count[country_id] > 0)
    percentage = ((double) vaccinated) / (vaccinated_count[country_id] +
                                          non_vaccinated_count[country_id]);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << country << " " << vaccinated << " " << (percentage * 100) << "%" << std::endl;
}


void utils::interface::output::print_population_status_by_age_output(
    const int& country_id,
    uint64_t** vaccinated_date_count_by_age,
    uint64_t** total_vaccinated_count_by_age,
    const std::string& country)
{
  double percentages[4] = {0};
  for (size_t i = 0; i < 4; i++)
    if (total_vaccinated_count_by_age[country_id][i])
      percentages[i] = ((double) vaccinated_date_count_by_age[country_id][i]) /
                                 total_vaccinated_count_by_age[country_id][i];

  std::cout << std::fixed << std::setprecision(2);
  std::cout << country << std::endl
            << "0-20  " << vaccinated_date_count_by_age[country_id][0] << " "
                        << (percentages[0] * 100) << "%" << std::endl
            << "20-40 " << vaccinated_date_count_by_age[country_id][1] << " "
                        << (percentages[1] * 100) << "%" << std::endl
            << "40-60 " << vaccinated_date_count_by_age[country_id][2] << " "
                        << (percentages[2] * 100) << "%" << std::endl
            << "60+   " << vaccinated_date_count_by_age[country_id][3] << " "
                        << (percentages[3] * 100) << "%" << std::endl << std::endl;

}
