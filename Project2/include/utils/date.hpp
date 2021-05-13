#ifndef _DATE
#define _DATE

#include <iostream>
#include <cstring>
#include <time.h>


typedef struct Date
{
    uint8_t day = 0;
    uint8_t month = 0;
    uint16_t year = 0;

    Date(void)
    { }

    Date(const uint8_t & _day, const uint8_t & _month, const uint16_t & _year): day(_day), month(_month), year(_year)
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

    size_t to_str(char date_buf[])
    {
        sprintf(date_buf, "%u-%u-%u", day, month, year);
        return strlen(date_buf);
    }

    bool is_within_6_months_in_the_past(Date* _date)
    {
        uint8_t month_six_months_earlier = (this->month > 6) ? this->month - 6 : 12 - (6 - this->month);
        uint16_t year_six_months_earlier = (this->month > 6) ? this->year : this->year - 1;
        Date six_month_earliers_date(this->day, month_six_months_earlier, year_six_months_earlier);
        return six_month_earliers_date < *_date && *_date < *this;
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
        out << +date.day << '-' << +date.month << '-' << date.year;
        return out;
    }
} Date;


#endif