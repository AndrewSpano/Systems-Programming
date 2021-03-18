#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include "../include/data_structures/index.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/data_structures/record_list.hpp"


int main(int argc, char* argv[])
{
  std::cout  << std::endl;

  /* arguments */
  std::string dataset_path = "";
  uint64_t bloom_filter_size = 0;
  if (!parsing::arguments::parse_arguments(argc, argv, dataset_path, bloom_filter_size))
  {
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* initialize the seed for the SkipList */
  srand(time(NULL));

  /* index struct that will contain all the data structures grouped together */
  Index index(bloom_filter_size, DEFAULT_NUM_BUCKETS);

  /* parse the dataset and insert information into the data structures */
  parsing::dataset::parse_dataset(dataset_path, index);

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
      std::string virus_name = "";

      if (parsing::user_input::parse_vaccine_status(line, citizen_id, virus_name, true))
        index.virus_list->in_bloom_filter_of_virus(citizen_id, virus_name)
          ? std::cout << "MAYBE" << std::endl
          : std::cout << "NOT VACCINATED" << std::endl;

    }
    else if (command == 2)
    {
      std::string citizen_id = "";
      std::string virus_name = "";

      if (parsing::user_input::parse_vaccine_status(line, citizen_id, virus_name, false))
        index.virus_list->vaccine_status(citizen_id, virus_name);
    }
    else if (command == 3)
    {
      std::string country = "";
      std::string virus_name = "";
      std::string date1 = "";
      std::string date2 = "";

      if (parsing::user_input::parse_population_status(line, country, virus_name, date1, date2))
        index.virus_list->population_status(index.countries_hash_table,
                                            country, virus_name, date1, date2, false);
    }
    else if (command == 4)
    {
      std::string country = "";
      std::string virus_name = "";
      std::string date1 = "";
      std::string date2 = "";

      if (parsing::user_input::parse_population_status(line, country, virus_name, date1, date2))
        index.virus_list->population_status(index.countries_hash_table,
                                            country, virus_name, date1, date2, true);
    }
    else if (command == 5)
    {
      std::string citizen_id = "";
      std::string first_name = "";
      std::string last_name = "";
      std::string country = "";
      uint8_t age = 0;
      std::string virus_name = "";
      bool status = false;
      std::string date = "";

      if (parsing::user_input::parse_insert_vaccinate(line, citizen_id, first_name, last_name,
                                                      country, age, virus_name, true, status, date))
        index.insert_citizen_record(citizen_id, first_name, last_name, country, age, virus_name,
                                    status, date);
      }
    else if (command == 6)
    {
      std::string citizen_id = "";
      std::string first_name = "";
      std::string last_name = "";
      std::string country = "";
      uint8_t age = 0;
      std::string virus_name = "";
      bool dummy_status = false;
      std::string dummy_date = "";

      if (parsing::user_input::parse_insert_vaccinate(line, citizen_id, first_name, last_name,
                                                      country, age, virus_name, false,
                                                      dummy_status, dummy_date))
        index.vaccinate_now(citizen_id, first_name, last_name, country, age, virus_name);
    }
    else if (command == 7)
    {
      std::string virus_name = "";

      if (parsing::user_input::parse_non_vaccinated_persons(line, virus_name))
        index.virus_list->print_virus(virus_name, false, true);
    }

    /* get the next command */
    std::cout << std::endl;
    command = parsing::user_input::get_option(line, false);
  }

  std::cout << std::endl;
  return EXIT_SUCCESS;
}
