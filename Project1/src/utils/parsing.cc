#include <iostream>
#include <fstream>
#include "../../include/utils/parsing.hpp"


/* function used to parse the information out of a string containing a Record */
void parsing::parse_record_line(const std::string& line, Index& index)
{
  /* indices of the string's individual characters */
  size_t start = 0, end = 0;

  /* parse the ID, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string _id = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_numerical(_id))
  {
    LOG_AND_RETURN(line)
  }
  uint64_t id = stoi(_id);
  start = end;

  /* parse the name, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string name = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(name))
  {
    LOG_AND_RETURN(line)
  }
  start = end;

  /* parse the surname, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string surname = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(surname))
  {
    LOG_AND_RETURN(line)
  }
  start = end;

  /* parse the country, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string country = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphabetical(country))
  {
    LOG_AND_RETURN(line)
  }
  start = end;

  /* parse the age, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string _age = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_numerical(_age))
  {
    LOG_AND_RETURN(line)
  }
  uint8_t age = stoi(_age);
  start = end;

  /* create a Record from the existing data */
  Record* new_record = new Record(id, name, surname, country, age);
  /* scan all the existing records to see if a Record with the same ID exists */
  Record* same_id_record = index.records->get(id);

  /*
  CASES:

    1. Record with the same ID but a different field exists -> ERROR, ignore it
    2. Input after (virus, yes/no, data) is incorrect -> ERROR, ignore it
    3. Same Record exists, vaccination data is contradictory to existing data -> ERROR, ignore it
  */

  /* 1. if a record with the same ID exists, and the new record is incompatible with it, skip */
  if (same_id_record && !parsing::utils::is_valid_new_record(new_record, same_id_record))
  {
    delete new_record;
    LOG_AND_RETURN(line)
  }

  /* 2. parse the disease, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string disease = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_alphanumerical(disease, true))
  {
    delete new_record;
    LOG_AND_RETURN(line)
  }
  start = end;

  /* 2. parse the status, skip the record if an error occurrs */
  parsing::utils::parse_next_substring(line, start, end);
  std::string status = line.substr(start, end - start);
  if (end == 0 || !parsing::utils::is_valid_status(status))
  {
    delete new_record;
    LOG_AND_RETURN(line)
  }
  start = end;

  /* 3. check if current record contradicts vaccination data for existing record with same ID */
  if (same_id_record)
  {

  }

  /* 2. if the status is "NO", make sure that this is the last string of the line and continue */
  if (status == "NO")
  {
    /* check for the status being the last string */
    parsing::utils::parse_next_substring(line, start, end);
    std::string dummy = line.substr(start, end - start);
    if (end != 0)
    {
      delete new_record;
      LOG_AND_RETURN(line)
    }

    /* if we get here the Record is legit, add it to the data structures */
    if (!same_id_record)
    {
      index.records->insert(new_record);
    }
    // other data structures
  }

  /* 2. if the status is "YES", parse the data and make sure that it is the last string */
  else
  {
    /* parse the date, skip the record if an error occurrs */
    parsing::utils::parse_next_substring(line, start, end);
    std::string date = line.substr(start, end - start);
    if (end == 0 || !parsing::utils::is_valid_numerical(date, true))
    {
      delete new_record;
      LOG_AND_RETURN(line)
    }
    start = end;

    /* make sure the date was the last string */
    parsing::utils::parse_next_substring(line, start, end);
    std::string dummy = line.substr(start, end - start);
    if (end != 0)
    {
      delete new_record;
      LOG_AND_RETURN(line)
    }

    /* if we get here the Record is legit, add it to the data structures */
    if (!same_id_record)
    {
      index.records->insert(new_record);
    }
    // other data structures
  }
}



/* parse the dataset file and build the corresponding data structures */
void parsing::dataset::parse_dataset(const std::string& dataset_path, Index& index)
{
  /* create an ifstream item to open and navigate the file */
  std::ifstream dataset(dataset_path, std::ios::binary);

  /* make sure that the file successfully opened */
  if (!dataset.is_open())
  {
    throw std::invalid_argument("Could not open Dataset file \"" + dataset_path + "\".");
  }

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
