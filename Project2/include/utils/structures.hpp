#ifndef _STRUCTURES
#define _STRUCTURES

#include <iostream>
#include <cstring>
#include "date.hpp"

namespace structures
{
    typedef struct Input
    {
        uint16_t num_monitors = 0;
        uint64_t buffer_size = 0;
        uint64_t bf_size = 0;
        std::string root_dir = "";

        void print(void)
        {
            std::cout << "Number of monitors: " << this->num_monitors << ", Buffer Size: " << this->buffer_size
                      << ", Bloom Filter Size: " << this->bf_size << ", Root Directory: " << this->root_dir << std::endl;
        }
    } Input;

    typedef struct CommunicationPipes
    {
        char* input = NULL;
        char* output = NULL;

        void print(void)
        {
            printf("Input Named Pipe: \"%s\", Output Named Pipe: \"%s\"\n", this->input, this->output);
        }
    } CommunicationPipes;

    typedef struct TravelRequestData
    {
        std::string citizen_id = "";
        Date* date;
        std::string country_from = "";
        std::string country_to = "";
        std::string virus_name = "";

        TravelRequestData(const std::string & _citizen_id, Date* _date, const std::string _country_from, const std::string _country_to, const std::string _virus_name):
        citizen_id(_citizen_id), date(_date), country_from(_country_from), country_to(_country_to), virus_name(_virus_name)
        { }

        TravelRequestData(char str_representation[]): country_from(""), country_to("")
        {
            size_t len = 0;
            size_t offset = 0;
            char data[128]= {0};

            /* read the citizen ID */
            read_from_buf(str_representation, offset, &len, data);
            citizen_id = std::string(data);
            memset(data, 0, 128);

            /* read the date */
            read_from_buf(str_representation, offset, &len, data);
            date = new Date(data);
            memset(data, 0, 128);

            /* read the virus */
            read_from_buf(str_representation, offset, &len, data);
            virus_name = std::string(data);
        }

        void write_to_buf(char buf[], size_t & offset, size_t* len, char data[])
        {
            memcpy(buf + offset, len, sizeof(size_t));
            offset += sizeof(size_t);
            memcpy(buf + offset, data, *len);
            offset += *len;
        }

        void read_from_buf(char buf[], size_t & offset, size_t* len, char data[])
        {
            memcpy(len, buf + offset, sizeof(size_t));
            offset += sizeof(size_t);
            memcpy(data, buf + offset, *len);
            offset += *len;
        }

        size_t to_str(char buf[])
        {
            size_t len = 0;
            size_t offset = 0;
            
            /* write citizen ID */
            len = citizen_id.length() + 1;
            write_to_buf(buf, offset, &len, (char *) citizen_id.c_str());

            /* write date */
            char date_buf[20];
            len = date->to_str(date_buf) + 1;
            write_to_buf(buf, offset, &len, date_buf);

            /* write virus */
            len = virus_name.length() + 1;
            write_to_buf(buf, offset, &len, (char *) virus_name.c_str());

            return offset;
        }
    } TravelRequestData;
}


#endif