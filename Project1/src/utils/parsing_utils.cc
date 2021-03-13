#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "../../include/utils/parsing.hpp"


/* true if the character passed is a whitespace character (space, tab, newline); Else false */
bool parsing::utils::is_whitespace(const char& c)
{
  return c == ' ' || c == '\t' || c == '\n';
}


/* given a starting position, find the next substring by assigning correct values to: start, end */
void parsing::utils::parse_next_substring(const std::string& str, size_t& start, size_t& end)
{
  /* skip leading whitespaces */
  while (str[start] != '\0' && parsing::utils::is_whitespace(str[start]))
    start++;

  /* assign 0 to start, end if no next substring was found and return */
  if (str[start] == '\0')
  {
    start = end = 0;
    return;
  }

  /* the start of the substring has been found, now find the end of the substring */
  end = start;
  while (str[end] != '\0' && !parsing::utils::is_whitespace(str[end]))
    end++;
}


/* true if the character given is a digit */
bool parsing::utils::is_digit(const char& c)
{
  return c >= '0' && c <= '9';
}


/* true if the character given is a letter (uppercase or lowercase) */
bool parsing::utils::is_letter(const char& c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}


/* true if the character given is a dash: '-' */
bool parsing::utils::is_dash(const char& c)
{
  return c == '-';
}


/* true if the string corresponds to a valid ID (only digits and/or dash: '-') */
bool parsing::utils::is_valid_numerical(const std::string& id, const bool& allow_dash)
{
  for (const char& c: id)
  {
    if (!(parsing::utils::is_digit(c) || (allow_dash && parsing::utils::is_dash(c))))
    {
      return false;
    }
  }
  return true;
}


/* true if the string corresponds to a valid name (only alphabetical characters and/or dash: '-') */
bool parsing::utils::is_valid_alphabetical(const std::string& name, const bool& allow_dash)
{
  for (const char& c: name)
  {
    if (!(parsing::utils::is_letter(c) || (allow_dash && parsing::utils::is_dash(c))))
    {
      return false;
    }
  }
  return true;
}


/* true if the string corresponds to a valid alphanumerical: digits, letters and/or dash: '-' */
bool parsing::utils::is_valid_alphanumerical(const std::string& str, const bool& allow_dash)
{
  for (const char& c: str)
  {
    if (!(parsing::utils::is_digit(c) || parsing::utils::is_letter(c) ||
         (allow_dash && parsing::utils::is_dash(c))))
    {
      return false;
    }
  }
  return true;
}


/* true if the status given is valid ("YES" or "NO") */
bool parsing::utils::is_valid_status(const std::string& status)
{
  return status == "YES" || status == "NO";
}


/* true if the new record has the same ID with an existing record, and is valid */
bool parsing::utils::is_valid_new_record(Record* new_record, Record* existing_record)
{
  return new_record->name == existing_record->name &&
  new_record->surname == existing_record->surname &&
  new_record->country == existing_record->country &&
  new_record->age == existing_record->age;
}


/* true if the file specified by the path exists */
bool parsing::utils::file_exists(const std::string& path)
{
  /* create a path variable */
  struct stat buffer;
  /* check if the exists by trying access its stats */
  return (stat (path.c_str(), &buffer) == 0);
}


/* print some information regarding the format of the command line parameters */
void parsing::arguments::print_help(void)
{
  std::cout << "Usage: ./vaccineMonitor -c citizenRecordsFile -b bloomSize\n\n"
            << "\twhere, citizenRecordsFile = The path to the Records Dataset\n"
            << "\t       bloomSize = The size of the Bloom Filter in bytes.";
}
