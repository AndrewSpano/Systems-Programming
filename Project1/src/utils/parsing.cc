#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
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
  if (same_id_record && !parsing::utils::is_valid_new_record(new_record, same_id_record))
    DELETE_LOG_AND_RETURN(new_record, line)

  /* 2. parse the disease, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string disease = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphanumerical(disease, true))
    DELETE_LOG_AND_RETURN(new_record, line)
  start = end;

  /* 2. parse the status, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string status = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_status(status))
    DELETE_LOG_AND_RETURN(new_record, line)
  start = end;

  /* 3. check if current record contradicts vaccination data for existing record with same ID */
  if (same_id_record && index.record_exists_in_disease(new_record, disease))
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
    /* ToDo: function below */
    index.insert(same_id_record, new_record, disease, status);
  }

  /* 2. if the status is "YES", parse the data and make sure that it is the last string */
  else
  {
    /* parse the date, skip the record if an error occurrs */
    parsing::utils::parse_next_substring(line, start, end);
    std::string date = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_numerical(date, true))
      DELETE_LOG_AND_RETURN(new_record, line)
    start = end;

    /* make sure the date was the last string */
    parsing::utils::parse_next_substring(line, start, end);
    std::string dummy = line.substr(start, end - start);
    if (end != 0)
      DELETE_LOG_AND_RETURN(new_record, line)

    /* if we get here the Record is legit, add it to the data structures */
    /* ToDo: function below */
    index.insert(same_id_record, new_record, disease, status, date);
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


/* parse the command-line arguments and store them in the 'dataset_path' and 'bloom_filter_size' variables */
bool parsing::arguments::parse_arguments(const int& argc, char* argv[], std::string& dataset_path, uint64_t& bloom_filter_size)
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
