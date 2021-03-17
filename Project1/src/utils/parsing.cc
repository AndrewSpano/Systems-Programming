#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/macros.hpp"
#include "../../include/utils/parsing.hpp"


/* function used to parse the information out of a string containing a Record */
void parsing::parse_record_line(const std::string& line, Index& index)
{
  /* indices of the string's individual characters */
  size_t start = 0, end = 0;

  /* parse the ID, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string id = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_numerical(id))
    LOG_AND_RETURN(line)
  start = end;

  /* parse the name, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string name = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(name))
    LOG_AND_RETURN(line)
  start = end;

  /* parse the surname, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string surname = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(surname))
    LOG_AND_RETURN(line)
  start = end;

  /* parse the country, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string country = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(country))
    LOG_AND_RETURN(line)
  start = end;

  /* parse the age, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string _age = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_numerical(_age))
    LOG_AND_RETURN(line)
  uint8_t age = stoi(_age);
  start = end;

  /* create a Record from the existing data */
  Record* new_record = new Record(id, name, surname, country, age);
  /* scan all the existing records to see if a Record with the same ID exists */
  Record* same_id_record = index.records_list->get(id);

  /*
  CASES:
    1. A Record with the same ID but a different field exists -> ERROR, continue
    2. Input after Record (virus, YES/NO, date) is incorrect -> ERROR, continue
    3. Same Record exists, vaccination data is contradictory to new data -> ERROR, continue
  */

  /* 1. if a record with the same ID exists, and the new record is incompatible with it, continue */
  if (same_id_record && !parsing::processing::is_valid_new_record(new_record, same_id_record))
    DELETE_LOG_AND_RETURN(new_record, line)

  /* 2. parse the virusName, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string virus_name = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphanumerical(virus_name, true))
    DELETE_LOG_AND_RETURN(new_record, line)
  start = end;

  /* 2. parse the status, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string status = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_status(status))
    DELETE_LOG_AND_RETURN(new_record, line)
  start = end;

  /* 3. check if current record contradicts vaccination data for existing record with same ID */
  if (same_id_record && index.virus_list->exists_in_virus_name(new_record->id, virus_name, false))
    DELETE_LOG_AND_RETURN(new_record, line)

  /* 2. if the status is "NO", make sure that this is the last string of the line and continue */
  if (status == "NO")
  {
    /* check for the status being the last string */
    parsing::utils::parse_next_substring(line, start, end);
    std::string dummy = line.substr(start, end - start);
    if (end != 0)
      DELETE_LOG_AND_RETURN(new_record, line)

    /* if we get here the Record is legit, add it to the data structures */
    index.insert(same_id_record, new_record, virus_name, status);
  }

  /* 2. if the status is "YES", parse the data and make sure that it is the last string */
  else
  {
    /* parse the date, skip the record if an error occurrs */
    parsing::utils::parse_next_substring(line, start, end);
    std::string date = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_date(date))
      DELETE_LOG_AND_RETURN(new_record, line)
    start = end;

    /* make sure the date was the last string */
    parsing::utils::parse_next_substring(line, start, end);
    std::string dummy = line.substr(start, end - start);
    if (end != 0)
      DELETE_LOG_AND_RETURN(new_record, line)

    /* if we get here the Record is legit, add it to the data structures */
    index.insert(same_id_record, new_record, virus_name, status, date);
  }
}


/* parse the dataset file and build the corresponding data structures */
void parsing::dataset::parse_dataset(const std::string& dataset_path, Index& index)
{
  /* create an ifstream item to open and navigate the file */
  std::ifstream dataset(dataset_path, std::ios::binary);

  /* make sure that the file successfully opened */
  if (!dataset.is_open())
    throw std::invalid_argument("Could not open Dataset file \"" + dataset_path + "\".");

  /* start reading the records (lines) one by one */
  std::string line = "";
  std::getline(dataset, line);

  while (line != "")
  {
    /* parse the next record, and if it is valid, add it to the index (data structures) */
    parsing::parse_record_line(line, index);
    /* read the next line */
    std::getline(dataset, line);
  }

  /* everything is done, close the file and return */
  dataset.close();
}


/* parse the command-line arguments and store them in the 'dataset_path' and
  'bloom_filter_size' variables */
bool parsing::arguments::parse_arguments(const int& argc, char* argv[],
                                         std::string& dataset_path, uint64_t& bloom_filter_size)
{
  if (argc != 5)
  {
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")))
      parsing::arguments::print_help();
    else
      std::cout << "ERROR: Incorrect number of arguments. Run with flag \"-h\" for more "
                << "information. Aborting .." << std::endl;
    return false;
  }

  bool flag_c = false;
  bool flag_b = false;

  for (size_t i = 1; i < 5; i += 2)
  {
    std::string flag(argv[i]);
    std::string value(argv[i + 1]);

    if (flag == "-c")
    {
      if (!parsing::utils::file_exists(value))
      {
        std::cout << "ERROR: Dataset path \"" << value << "\" does not correspond to an existing "
                  << "file. Run with flag \"-h\" for more information. Aborting .." << std::endl;
        return false;
      }
      dataset_path = value;
      flag_c = true;
    }
    else if (flag == "-b")
    {
      if (!parsing::utils::is_valid_numerical(value))
      {
        std::cout << "ERROR: Bloom Filter size \"" << value << "\" is not a valid integer positive "
                  << "value. Run with flag \"-h\" for more information. Aborting .." << std::endl;
        return false;
      }
      bloom_filter_size = stoi(value);
      if (bloom_filter_size == 0)
      {
        std::cout << "ERROR: Bloom Filter size can't be equal to 0. Run with flag \"-h\" for more "
                  << "information. Aborting .." << std::endl;
        return false;
      }
      flag_b = true;
    }
    else
    {
      std::cout << "ERROR: Unrecognized argument " << flag << ". Run with flag \"-h\" for more "
                << "information. Aborting .." << std::endl;
      return false;
    }
  }

  if (!flag_c)
  {
    std::cout << "ERROR: Flag -c (citizenRecordsFile) was not provided. Run with flag \"-h\" for "
              << "more information. Aborting .." << std::endl;
    return false;
  }
  else if (!flag_b)
  {
    std::cout << "ERROR: Flag -b (bloomSize) was not provided. Run with flag \"-h\" for more "
              << "information. Aborting .." << std::endl;
    return false;
  }

  return true;
}


/* parse the user input */
int parsing::user_input::get_option(std::string& line, const bool& _print_help)
{
  /* see which command the user wants to execute */
  size_t start = 0;
  size_t end = 0;

  /* print available commands */
  if (_print_help)
    parsing::user_input::print_options();

  /* loop until a valid command is given */
  while (4 + 20 != 420)
  {
    /* prompt the user to give a command */
    std::cout << "Enter your command: ";
    std::getline(std::cin, line);

    /* parse the command */
    parsing::utils::parse_next_substring(line, start, end);
    std::string command = line.substr(start, end - start);

    if (command == "/vaccineStatusBloom")
      return 1;
    else if (command == "/vaccineStatus")
      return 2;
    else if (command == "/populationStatus")
      return 3;
    else if (command == "/popStatusByAge")
      return 4;
    else if (command == "/insertCitizenRecord")
      return 5;
    else if (command == "/vaccinateNow")
      return 6;
    else if (command == "/list-nonVaccinated-Persons")
      return 7;
    else if (command == "/exit")
      return 0;
    else if (command == "/help")
      parsing::user_input::print_options();
    else
      std::cout << "\nUnknown command: " << command << "\n\n";
  }
}


/* parse the commands "/vaccineStatusBloom" and "/vaccineStatus" */
bool parsing::user_input::parse_vaccine_status(const std::string& line,
                                               std::string& citizen_id,
                                               std::string& virus_name,
                                               const bool& needs_virus_name=false)
{
  size_t start = 0;
  size_t end = 0;

  /* skip the command sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  start = end;

  /* parse the citizenID */
  parsing::utils::parse_next_substring(line, start, end);
  citizen_id = line.substr(start, end - start);
  if (!end || !parsing::utils::is_valid_numerical(citizen_id))
    LOG_COMMAND_AND_RETURN(line);
  start = end;

  /* parse the virusName (if it is not given and it is not needed, this is acceptable) */
  parsing::utils::parse_next_substring(line, start, end);
  if (!needs_virus_name && !end)
    return true;
  virus_name = line.substr(start, end - start);
  if (!end || !parsing::utils::is_valid_alphanumerical(virus_name, true))
    LOG_COMMAND_AND_RETURN(line);
  start = end;

  /* make sure that was the last sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  if (end)
    LOG_COMMAND_AND_RETURN(line);

  return true;
}


/* parse the commands "/populationStatus" and "/popStatusByAge" */
bool parsing::user_input::parse_population_status(const std::string& line,
                                                 std::string& country,
                                                 std::string& virus_name,
                                                 std::string& date1,
                                                 std::string& date2)
{
  size_t start = 0;
  size_t end = 0;

  /* skip the command sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  start = end;

  /* keep track of the arguments given */
  uint8_t argc = 0;
  std::string argv[5];

  /* parse them one by one and store them in an array */
  parsing::utils::parse_next_substring(line, start, end);
  while (end && argc < 5)
  {
    argv[argc++] = line.substr(start, end - start);
    start = end;
    parsing::utils::parse_next_substring(line, start, end);
  }

  /* check for errors */
  if (argc == 0 || argc == 5)
    LOG_COMMAND_AND_RETURN(line);

  /* determine which arguments were given by the number of arguments given */
  switch (argc)
  {
    /* only virusName */
    case 1:
    {
      if (!parsing::utils::is_valid_alphanumerical(argv[0], true))
        LOG_COMMAND_AND_RETURN(line);
      virus_name = argv[0];
      break;
    }
    /* country + virusName */
    case 2:
    {
      if (!parsing::utils::is_valid_alphabetical(argv[0], false) ||
          !parsing::utils::is_valid_alphanumerical(argv[1], true))
        LOG_COMMAND_AND_RETURN(line);
      country = argv[0];
      virus_name = argv[1];
      break;
    }
    /* virusName + date1 + date2 */
    case 3:
    {
      if (!parsing::utils::is_valid_alphanumerical(argv[0], true) ||
          !parsing::utils::is_valid_date(argv[1]) ||
          !parsing::utils::is_valid_date(argv[2]) ||
          !parsing::utils::date2_is_later_than_date1(argv[1], argv[2]))
        LOG_COMMAND_AND_RETURN(line);
      virus_name = argv[0];
      date1 = argv[1];
      date2 = argv[2];
      break;
    }
    /* country + virusName + date1 + date2 */
    case 4:
    {
      if (!parsing::utils::is_valid_alphabetical(argv[0], false) ||
          !parsing::utils::is_valid_alphanumerical(argv[1], true) ||
          !parsing::utils::is_valid_date(argv[2]) ||
          !parsing::utils::is_valid_date(argv[3]) ||
          !parsing::utils::date2_is_later_than_date1(argv[2], argv[3]))
        LOG_COMMAND_AND_RETURN(line);
      country = argv[0];
      virus_name = argv[1];
      date1 = argv[2];
      date2 = argv[3];
      break;
    }
  }

  return true;
}


/* parse the commands "/insertCitizenRecord" and "/vaccinateNow" */
bool parsing::user_input::parse_insert_vaccinate(const std::string& line,
                                                 std::string& citizen_id,
                                                 std::string& first_name,
                                                 std::string& last_name,
                                                 std::string& country,
                                                 uint8_t& age,
                                                 std::string& virus_name,
                                                 const bool& needs_status_and_date,
                                                 bool& status,
                                                 std::string& date)
{
  size_t start = 0;
  size_t end = 0;

  /* skip the command sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  start = end;

  /* determine the max number of arguments that the command can have */
  uint8_t max_args = needs_status_and_date ? 8 : 6;

  /* keep track of the arguments given */
  uint8_t argc = 0;
  std::string argv[max_args + 1];

  /* parse them one by one and store them in an array */
  parsing::utils::parse_next_substring(line, start, end);
  while (end && argc < max_args + 1)
  {
    argv[argc++] = line.substr(start, end - start);
    start = end;
    parsing::utils::parse_next_substring(line, start, end);
  }

  /* check for errors */
  if ((!needs_status_and_date && argc != max_args) ||
       (needs_status_and_date && (argc < max_args - 1 && argc > max_args)))
    LOG_COMMAND_AND_RETURN(line)

  /* make sure tha arguments are legit */
  if (!parsing::utils::is_valid_numerical(argv[0], false) ||
      !parsing::utils::is_valid_alphabetical(argv[1], false) ||
      !parsing::utils::is_valid_alphabetical(argv[2], false) ||
      !parsing::utils::is_valid_alphabetical(argv[3], false) ||
      !parsing::utils::is_valid_numerical(argv[4], false) ||
      !parsing::utils::is_valid_alphanumerical(argv[5], true) ||
     (needs_status_and_date &&
     (!parsing::utils::is_valid_status(argv[6]) ||
      (argv[6] == "YES" && !parsing::utils::is_valid_date(argv[7])))))
    LOG_COMMAND_AND_RETURN(line)

  citizen_id = argv[0];
  first_name = argv[1];
  last_name = argv[2];
  country = argv[3];
  age = stoi(argv[4]);
  virus_name = argv[5];
  if (needs_status_and_date)
  {
    status = argv[6] == "YES";
    date = argv[7];
  }

  return true;
}


/* parse the command "/list-nonVaccinated-Persons" */
bool parsing::user_input::parse_non_vaccinated_persons(const std::string& line,
                                                       std::string& virus_name)
{
  size_t start = 0;
  size_t end = 0;

  /* skip the command sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  start = end;

  /* parse the virusName */
  parsing::utils::parse_next_substring(line, start, end);
  virus_name = line.substr(start, end - start);
  if (!end || !parsing::utils::is_valid_alphanumerical(virus_name, true))
    LOG_COMMAND_AND_RETURN(line);
  start = end;

  /* make sure that was the last sub-string */
  parsing::utils::parse_next_substring(line, start, end);
  if (end)
    LOG_COMMAND_AND_RETURN(line);

  return true;
}
