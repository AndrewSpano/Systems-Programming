#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/stat.h>

#include "../../include/utils/parsing.hpp"


bool parsing::utils::is_whitespace(const char & c)
{ return c == ' ' || c == '\t' || c == '\n'; }


void parsing::utils::parse_next_substring(const std::string & str, size_t & start, size_t & end)
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


bool parsing::utils::is_digit(const char & c)
{ return c >= '0' && c <= '9'; }


bool parsing::utils::is_letter(const char & c)
{ return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }


bool parsing::utils::is_hyphen(const char & c)
{ return c == '-'; }


bool parsing::utils::is_valid_numerical(const std::string & str, const bool & allow_hyphen)
{
    for (const char & c: str)
    {
        if (!(parsing::utils::is_digit(c) || (allow_hyphen && parsing::utils::is_hyphen(c))))
        {
            return false;
        }
    }
    return true;
}


bool parsing::utils::is_valid_alphabetical(const std::string & str, const bool & allow_hyphen)
{
    for (const char & c: str)
        if (!(parsing::utils::is_letter(c) || (allow_hyphen && parsing::utils::is_hyphen(c))))
            return false;
    return true;
}


bool parsing::utils::is_valid_alphanumerical(const std::string & str, const bool & allow_hyphen)
{
    for (const char & c: str)
        if (!(parsing::utils::is_digit(c) || parsing::utils::is_letter(c) ||
            (allow_hyphen && parsing::utils::is_hyphen(c))))
            return false;
    return true;
}


bool parsing::utils::is_valid_status(const std::string & str)
{ return str == "YES" || str == "NO"; }


bool parsing::utils::is_valid_date(const std::string & str)
{
    struct tm tm_date;
    return strptime(str.c_str(), "%d-%m-%Y", &tm_date) != NULL;
}


bool parsing::utils::is_directory(const std::string & path)
{
    struct stat sb;
    return stat(path.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode);
}


void parsing::user_input::print_options(void)
{
    std::cout << std::endl << "The available commands are:" << std::endl << std::endl
              << "\t\t1. /travelRequest citizenID date countryFrom countryTo virusName" << std::endl
              << "\t\t2. /travelStats virusName date1 date2 [country]" << std::endl
              << "\t\t3. /addVaccinationRecords country" << std::endl
              << "\t\t4. /searchVaccinationStatus citizenID" << std::endl
              << "\t\t5. /exit" << std::endl
              << "\t\t6. /help" << std::endl << std::endl;
}

