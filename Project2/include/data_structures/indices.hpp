#ifndef _INDEX
#define _INDEX

#include <iostream>

#include "record.hpp"
#include "list.hpp"
#include "bloom_filter.hpp"
#include "virus_list.hpp"
#include "../utils/structures.hpp"


/// structure to handle the data stored in a Monitor process
typedef struct MonitorIndex
{
    List<Record>* records = NULL;
    uint16_t num_countries = 0;
    std::string* countries = NULL;
    List<std::string>** files_per_country = NULL;
    VirusList* virus_list = NULL;

    MonitorIndex(const uint64_t & bf_size): num_countries(0), countries(NULL), files_per_country(NULL)
    {
        records = new List<Record>();
        virus_list = new VirusList(bf_size);
    }

    ~MonitorIndex(void)
    {
        delete records;
        if (countries) delete[] countries;
        if (files_per_country)
        {
            for (size_t i = 0; i < num_countries; i++)
                delete files_per_country[i];
            delete[] files_per_country;            
        }
        delete virus_list;
    }

    void init_countries(List<std::string> & countries_list)
    {
        num_countries = countries_list.get_size();
        countries = new std::string[num_countries];
        
        std::string** countries_arr = countries_list.get_as_arr();
        for (size_t i = 0; i < num_countries; i++)
            countries[i] = *countries_arr[i];
        delete[] countries_arr;

        files_per_country = new List<std::string>*[num_countries];
        for (size_t i = 0; i < num_countries; i++)
            files_per_country[i] = new List<std::string>;
    }

    void insert(Record* existing_record, Record* new_record, const std::string & virus_name, const std::string & _status, Date* date=NULL)
    {
        /* if the record has not been seen before, insert it in the list with records */
        if (!existing_record)
        {
            records->insert(new_record);
        }
        else
        {
            delete new_record;
            new_record = existing_record;
        }

        /* insert the new entry in the specified disease list */
        bool status = _status == "YES";
        virus_list->insert(new_record, virus_name, status, date);
    }

} MonitorIndex;


/// structure to handle the data stored in the travelMonitor process
typedef struct travelMonitorIndex
{
    structures::Input* input;
    uint16_t num_countries;
    std::string* countries;
    List<BFPair>* bloom_filters;

    travelMonitorIndex(structures::Input* _inp): input(_inp), num_countries(0), countries(NULL)
    { bloom_filters = new List<BFPair>; }

    ~travelMonitorIndex(void)
    {
        if (countries) delete[] countries;
        if (bloom_filters) delete bloom_filters;
    }

    void init_countries(const size_t & _num_countries)
    {
        num_countries = _num_countries;
        countries = new std::string[num_countries];
    }

    int country_id(const std::string & country)
    {
        for (size_t i = 0; i < num_countries; i++)
            if (countries[i] == country) return i;
        return -1;
    }

    size_t monitor_with_country(const size_t & country_id)
    { return (country_id / input->num_monitors) * input->num_monitors + (country_id % input->num_monitors); }

} travelMonitorIndex;

#endif