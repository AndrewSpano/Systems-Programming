#ifndef _INDEX
#define _INDEX

#include <iostream>
#include <unistd.h>

#include "record.hpp"
#include "list.hpp"
#include "bloom_filter.hpp"
#include "virus_list.hpp"
#include "logger.hpp"
#include "../utils/structures.hpp"



/// structure to handle the data stored in a monitorServer process
typedef struct MonitorIndex
{
    structures::MonitorInput* input;
    List<Record>* records = NULL;
    List<std::string>** files_per_country = NULL;
    VirusList* virus_list = NULL;
    Logger* logger = NULL;

    MonitorIndex(structures::MonitorInput* _inp): input(_inp), files_per_country(NULL), logger(NULL)
    {
        records = new List<Record>();
        files_per_country = new List<std::string>*[input->num_countries];
        for (size_t i = 0; i < input->num_countries; i++)
            files_per_country[i] = new List<std::string>();
        virus_list = new VirusList(input->bf_size);
        logger = new Logger(getpid(), input->num_countries, input->countries);        
    }

    ~MonitorIndex(void)
    {
        delete records;
        for (size_t i = 0; i < input->num_countries; i++)
            delete files_per_country[i];
        delete[] files_per_country;            
        delete logger;
        delete virus_list;
    }

    int country_id(const std::string & country)
    {
        for (size_t i = 0; i < input->num_countries; i++)
            if (input->countries[i] == country) return i;
        return -1;
    }

    std::string* country_ptr(const std::string & country)
    {
        int id = country_id(country);
        if (id == -1) return NULL;
        return (id == -1) ? NULL
                          : &input->countries[id];
    }

    void insert(Record* new_record, const std::string & virus_name, const std::string & _status, Date* date=NULL)
    {
        /* insert the new record in the list with records */
        records->insert(new_record);
        /* insert the new entry in the specified virus list */
        bool status = _status == "YES";
        virus_list->insert(new_record, virus_name, status, date);
    }

} MonitorIndex;



/// structure to handle the data stored in the travelMonitorClient process
typedef struct travelMonitorIndex
{
    structures::travelMonitorInput* input;
    uint16_t num_countries;
    std::string* countries;
    List<BFPair>* bloom_filters;
    Logger* logger;
    bool is_receiving_data;
    bool has_sent_sigkill;

    travelMonitorIndex(structures::travelMonitorInput* _inp): input(_inp), num_countries(0), countries(NULL), logger(NULL), is_receiving_data(false), has_sent_sigkill(false)
    { bloom_filters = new List<BFPair>; }

    ~travelMonitorIndex(void)
    {
        if (countries) delete[] countries;
        if (bloom_filters) delete bloom_filters;
        if (logger) delete logger;
    }

    void init_countries(const size_t & _num_countries)
    {
        num_countries = _num_countries;
        countries = new std::string[num_countries];
        logger = new Logger(getpid(), num_countries, countries);
    }

    int country_id(const std::string & country)
    {
        for (size_t i = 0; i < num_countries; i++)
            if (countries[i] == country) return i;
        return -1;
    }

    size_t monitor_with_country(const size_t & country_id)
    { return country_id % input->num_monitors; }

    size_t num_countries_of_monitor(const size_t & monitor_id)
    { return num_countries / input->num_monitors + ((num_countries % input->num_monitors > 0) && ((monitor_id < num_countries % input->num_monitors)) ? 1 : 0); }

    void get_countries_of_monitor(std::string target_arr[], const size_t & monitor_id)
    {
        size_t index = 0;
        for (size_t i = monitor_id; i < num_countries; i += input->num_monitors)
            target_arr[index++] = countries[i];
    }

} travelMonitorIndex;

#endif