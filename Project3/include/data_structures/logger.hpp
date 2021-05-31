#ifndef _LOGGER
#define _LOGGER

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

#include "indices.hpp"
#include "list.hpp"
#include "../utils/structures.hpp"


typedef struct Logger
{
    pid_t pid = 0;
    uint16_t num_countries = 0;
    std::string* countries = NULL;
    size_t accepted_requests = 0;
    size_t rejected_requests = 0;
    List<structures::TRQuery>* query_list = NULL;
    std::string outfile_path = "";

    Logger(const pid_t & _pid, const uint16_t & _num_countries, std::string* _countries):
    pid(_pid), num_countries(_num_countries), countries(_countries), accepted_requests(0), rejected_requests(0)
    {
        query_list = new List<structures::TRQuery>;
        outfile_path = "logfiles/log_file." + std::to_string(pid);
    }

    ~Logger(void)
    { delete query_list; }

    void insert(structures::TRQuery* query)
    {
        query_list->insert_unsorted(query, 0);
        (query->was_accepted) ? accepted_requests++
                              : rejected_requests++;
    }

    void write_to_logfile(void)
    {
        std::ofstream outfile(outfile_path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!outfile.is_open())
            throw std::invalid_argument("Could not open log file \"" + outfile_path + "\".");

        for (size_t i = 0; i < num_countries; i++)
            outfile << countries[i] << "\n";

        outfile << "TOTAL TRAVEL REQUESTS " << (accepted_requests + rejected_requests) << "\n";
        outfile << "ACCEPTED " << accepted_requests << "\n";
        outfile << "REJECTED " << rejected_requests << "\n";        

        outfile.close();
    }


} Logger;



#endif