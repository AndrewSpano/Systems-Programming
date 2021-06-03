#ifndef _STRUCTURES
#define _STRUCTURES

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "date.hpp"
#include "utils.hpp"


namespace structures
{

    typedef struct travelMonitorInput
    {
        uint16_t num_monitors = 0;
        uint64_t socket_buffer_size = 0;
        uint16_t cyclic_buffer_size = 0;
        uint64_t bf_size = 0;
        std::string root_dir = "";
        uint16_t num_threads = 0;

        void print(void)
        {
            std::cout << "Number of Monitors: " << this->num_monitors << ", Socket Buffer Size: " << this->socket_buffer_size
                      << ", Cyclic Buffer Size: " << this->cyclic_buffer_size << ", Bloom Filter Size: " << this->bf_size
                      << ", Root Directory: " << this->root_dir << ", Number of Threads: " << this->num_threads << std::endl;
        }
    } travelMonitorInput;


    typedef struct MonitorInput
    {
        uint16_t port = 0;
        uint16_t num_threads = 0;
        uint64_t socket_buffer_size = 0;
        uint16_t cyclic_buffer_size = 0;
        uint64_t bf_size = 0;
        uint16_t num_countries = 0;
        std::string* countries_paths = NULL;

        ~MonitorInput(void)
        { if (countries_paths != NULL) delete[] countries_paths; }

        void print(void)
        {
            std::cout << "Port: " << this->port << ", Number of Threads: " << this->num_threads
                      << ", Socket Buffer Size: " << this->socket_buffer_size << ", Cyclic Buffer Size: " << this->cyclic_buffer_size
                      << ", Bloom Filter Size: " << this->bf_size << ", Number of countries: " << this->num_countries << std::endl;
            for (size_t i = 0; i < this->num_countries; i++)
                std::cout << "\t" << countries_paths[i] << std::endl;            
        }
    } MonitorInput;



    typedef struct NetworkCommunication
    {
        int client_socket = -1;
        int server_socket = -1;
        int port = -1;
        sockaddr_in server_address;

        NetworkCommunication(void): client_socket(-1), server_socket(-1), port(-1)
        { }

        NetworkCommunication(const int &_port): client_socket(-1), server_socket(-1), port(_port)
        { }
    } NetworkCommunication;



    typedef struct TRQuery
    {
        Date* date = NULL;
        std::string country = "";
        std::string virus_name = "";
        bool was_accepted = false;

        TRQuery(Date* _date, const std::string & _country, const std::string & _virus_name, const bool & _was_accepted):
        country(_country), virus_name(_virus_name), was_accepted(_was_accepted)
        { date = new Date(_date); }

        ~TRQuery(void)
        { delete date; }

        bool operator < (const std::string & str)
        { return *date < Date(str); }

        bool operator <= (const std::string & str)
        { return *date <= Date(str); }

        bool operator == (const std::string & str)
        { return *date == Date(str); }

        bool operator != (const std::string & str)
        { return *date != Date(str); }

        bool operator > (const std::string & str)
        { return *date > Date(str); }

        bool operator >= (const std::string & str)
        { return *date >= Date(str); }

        bool operator < (const TRQuery & query)
        { return *date < *(query.date); }

        bool operator <= (const TRQuery & query)
        { return *date <= *(query.date); }

        bool operator == (const TRQuery & query)
        { return *date == *(query.date); }

        bool operator != (const TRQuery & query)
        { return *date != *(query.date); }

        bool operator > (const TRQuery & query)
        { return *date > *(query.date); }

        bool operator >= (const TRQuery & query)
        { return *date >= *(query.date); }


    } TRQuery;



    typedef struct TRData
    {
        std::string citizen_id = "";
        Date* date;
        std::string country_from = "";
        std::string country_to = "";
        std::string virus_name = "";

        TRData(const std::string & _citizen_id, Date* _date, const std::string _country_from, const std::string _country_to, const std::string _virus_name):
        citizen_id(_citizen_id), date(_date), country_from(_country_from), country_to(_country_to), virus_name(_virus_name)
        { }

        TRData(char str_representation[]): country_from(""), country_to("")
        {
            size_t len = 0;
            size_t offset = 0;
            char data[128]= {0};

            /* read the citizen ID */
            utils::read_from_buf(str_representation, offset, &len, data);
            citizen_id = std::string(data);
            memset(data, 0, 128);

            /* read the date */
            utils::read_from_buf(str_representation, offset, &len, data);
            date = new Date(data);
            memset(data, 0, 128);

            /* read the virus */
            utils::read_from_buf(str_representation, offset, &len, data);
            virus_name = std::string(data);
        }

        size_t to_str(char buf[])
        {
            size_t len = 0;
            size_t offset = 0;
            
            /* write citizen ID */
            len = citizen_id.length() + 1;
            utils::write_to_buf(buf, offset, &len, (char *) citizen_id.c_str());

            /* write date */
            char date_buf[20];
            len = date->to_str(date_buf) + 1;
            utils::write_to_buf(buf, offset, &len, date_buf);

            /* write virus */
            len = virus_name.length() + 1;
            utils::write_to_buf(buf, offset, &len, (char *) virus_name.c_str());

            return offset;
        }
    } TRData;



    typedef struct TSData
    {
        std::string virus_name = "";
        Date* date_1 = NULL;
        Date* date_2 = NULL;
        std::string country = "";

        TSData(const std::string & _vn, Date* _date_1, Date* _date_2, const std::string _country=""):
        virus_name(_vn), date_1(_date_1), date_2(_date_2), country(_country)
        { }
    } TSData;



    typedef struct VaccinationStatus
    {
        std::string virus_name = "";
        bool status = false;
        Date* date = NULL;

        VaccinationStatus(const std::string & _vn, const bool & _status, Date* _date): virus_name(_vn), status(_status), date(_date)
        { }

        VaccinationStatus(char str_representation[]): date(NULL)
        {
            size_t len = 0;
            size_t offset = 0;
            char data[128]= {0};

            /* read the virus */
            utils::read_from_buf(str_representation, offset, &len, data);
            virus_name = std::string(data);
            memset(data, 0, 128);

            /* read the status */
            memcpy(&status, str_representation + offset, sizeof(bool));
            offset++;

            /* read the date */
            if (status)
            {
                utils::read_from_buf(str_representation, offset, &len, data);
                date = new Date(data);
            }
        }

        size_t to_str(char buf[])
        {
            size_t len = 0;
            size_t offset = 0;
            
            /* write the virus */
            len = virus_name.length() + 1;
            utils::write_to_buf(buf, offset, &len, (char *) virus_name.c_str());

            /* write status */
            memcpy(buf + offset, &status, sizeof(bool));
            offset++;

            /* write date */
            if (status)
            {                
                char date_buf[128];
                len = date->to_str(date_buf) + 1;
                utils::write_to_buf(buf, offset, &len, date_buf);
            }

            return offset;
        }
    } VaccinationStatus;
}


#endif