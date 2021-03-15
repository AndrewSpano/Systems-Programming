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


/* true if the string given corresponds to a valid date of the format: dd-mm-yyyy */
bool parsing::utils::is_valid_date(const std::string& str)
{
  return str.length() == 10 &&
         is_digit(str[0]) && is_digit(str[1]) &&
         is_dash(str[2]) &&
         is_digit(str[3]) && is_digit(str[4]) &&
         is_dash(str[5]) &&
         is_digit(str[6]) && is_digit(str[7]) && is_digit(str[8]) && is_digit(str[9]) &&
         stoi(str.substr(0, 2)) <= 30 &&
         stoi(str.substr(3, 2)) <= 12 &&
         stoi(str.substr(5, 4)) <= CURRENT_YEAR;
}


/* true if the second date is "later" than date1; else false */
bool parsing::utils::date2_is_later_than_date1(const std::string& date1, const std::string& date2)
{
  uint16_t year1 = stoi(date1.substr(5, 4));
  uint16_t year2 = stoi(date2.substr(5, 4));

  if (year2 > year1)
    return true;
  else if (year1 > year2)
    return false;
  else
  {
    uint8_t month1 = stoi(date1.substr(3, 2));
    uint8_t month2 = stoi(date2.substr(3, 2));

    if (month2 > month1)
      return true;
    else if (month1 > month2)
      return false;
    else
    {
      uint8_t days1 = stoi(date1.substr(0, 2));
      uint8_t days2 = stoi(date2.substr(0, 2));

      return days2 >= days1;
    }
  }
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


/* print some informaton regarding the available commands for the user to execute */
void parsing::user_input::print_options(void)
{
  std::cout << "The available commands are:\n\n"
            << "\t\t1. /vaccineStatusBloom citizenID virusName\n"
            << "\t\t2. /vaccineStatus citizenID virusName\n"
            << "\t\t3. /vaccineStatus citizenID\n"
            << "\t\t4. /populationStatus [country] virusName [date1 date2]\n"
            << "\t\t5. /popStatusByAge [country] virusName [date1 date2]\n"
            << "\t\t6. /insertCitizenRecord citizenID firstName lastName country age virusName " <<
               "YES/NO [date]\n"
            << "\t\t7. /vaccinateNow citizenID firstName lastName country age virusName\n"
            << "\t\t8. /list-nonVaccinated-Persons virusName\n"
            << "\t\t9. /exit\n"
            << "\t\t10. /help\n\n";
}
