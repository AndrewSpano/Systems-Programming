#ifndef _DATE
#define _DATE

#include <iostream>
#include <time.h>


typedef struct Date
{
    uint8_t day = 0;
    uint8_t month = 0;
    uint16_t year = 0;

    Date(void)
    { }

    Date(const std::string & date_str)
    {
        struct tm tm_date;
        strptime(date_str.c_str(), "%d-%m-%Y", &tm_date);
        this->day = tm_date.tm_mday;
        this->month = tm_date.tm_mon + 1;
        this->year = tm_date.tm_year + 1900;
    }

    ~Date(void)
    { }

    void construct(const std::string & date_str)
    {
        struct tm tm_date;
        strptime(date_str.c_str(), "%d-%m-%Y", &tm_date);
        this->day = tm_date.tm_mday;
        this->month = tm_date.tm_mon + 1;
        this->year = tm_date.tm_year + 1900;
    }

    bool operator < (const Date & date)
    {
        return (this->year < date.year) ||
               (this->year == date.year && this->month < date.month) ||
               (this->year == date.year && this->month == date.month && this->day < date.day);
    }

    bool operator <= (const Date & date)
    {
        return (this->year < date.year) ||
               (this->year == date.year && this->month < date.month) ||
               (this->year == date.year && this->month == date.month && this->day <= date.day);
    }

    bool operator == (const Date & date)
    {
        return this->year == date.year && this->month == date.month && this->day == date.day;
    }

    bool operator > (const Date & date)
    {
        return (this->year > date.year) ||
               (this->year == date.year && this->month > date.month) ||
               (this->year == date.year && this->month == date.month && this->day > date.day);
    }

    bool operator >= (const Date & date)
    {
        return (this->year > date.year) ||
               (this->year == date.year && this->month > date.month) ||
               (this->year == date.year && this->month == date.month && this->day >= date.day);
    }

    friend std::ostream & operator << (std::ostream & out, const Date & date)
    {
        out << date.day << '-' << date.month << '-' << date.year;
        return out;
    }
} Date;


#endif