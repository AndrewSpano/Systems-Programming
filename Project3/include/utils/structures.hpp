#ifndef _STRUCTURES
#define _STRUCTURES

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include "date.hpp"
#include "utils.hpp"


namespace structures
{

    /// defines the travelMonitorClient input parameters
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
            std::cout << "Number of Monitors: " << num_monitors << ", Socket Buffer Size: " << socket_buffer_size
                      << ", Cyclic Buffer Size: " << cyclic_buffer_size << ", Bloom Filter Size: " << bf_size
                      << ", Root Directory: " << root_dir << ", Number of Threads: " << num_threads << std::endl;
        }
    } travelMonitorInput;





    /// defines the monitorServer input parameters
    typedef struct MonitorInput
    {
        uint16_t port = 0;
        uint16_t num_threads = 0;
        uint64_t socket_buffer_size = 0;
        uint16_t cyclic_buffer_size = 0;
        uint64_t bf_size = 0;
        std::string root_dir = "";
        uint16_t num_countries = 0;
        std::string* countries = NULL;

        ~MonitorInput(void)
        { if (countries != NULL) delete[] countries; }

        void print(void)
        {
            std::cout << "Port: " << port << ", Number of Threads: " << num_threads
                      << ", Socket Buffer Size: " << socket_buffer_size << ", Cyclic Buffer Size: " << cyclic_buffer_size
                      << ", Bloom Filter Size: " << bf_size << ", Root Directory = " << root_dir
                      << ", Number of countries: " << num_countries << std::endl;
            for (size_t i = 0; i < num_countries; i++)
                std::cout << "\t" << countries[i] << std::endl;            
        }
    } MonitorInput;





    /// defines the sockets (fds) used for communication
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





    /// defines the cyclic buffer from which the monitorServer threads will get files to process
    typedef struct CyclicBuffer
    {
        uint16_t size = 0;
        char** buffer = NULL;
        uint16_t start = 0;
        uint16_t end = 0;
        uint16_t count = 0;

        CyclicBuffer(const uint16_t & _size): size(_size), start(0), end(0), count(0)
        {
            buffer = new char*[size];
            for (size_t i = 0; i < size; i++)
                buffer[i] = new char[256];
            
        }

        ~CyclicBuffer(void)
        {
            if (buffer)
            {
                for (size_t i = 0; i < size; i++)
                    delete[] buffer[i];
                delete[] buffer;
            }
            buffer = NULL;
        }

        void insert(char str[])
        {
            memcpy(buffer[end], str, strlen(str) + 1);
            end = (end + 1) % size;
            count++;
        }

        char* remove(void)
        {
            char* str = new char[256];
            strcpy(str, buffer[start]);
            memset(buffer[start], 0, 256);
            start = (start + 1) % size;
            count--;
            return str;
        }

        bool is_empty(void)
        { return count == 0; }

        bool is_full(void)
        { return count == size; }
    } CyclicBuffer;



    /// defines the pthread variables used to ensure that race conditions are not violated
    typedef struct RaceConditions
    {
        pthread_mutex_t buffer_access;
        pthread_mutex_t data_structures_access;
        pthread_mutex_t counter_access;
        pthread_cond_t cond_non_empty;
        pthread_cond_t cond_non_full;
        pthread_cond_t counter_is_max;
    } RaceConditions;





    /// defines the structure of a Travel Request Query
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





    /// defines the structure of a Travel Request Query Data
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





    /// defines the structure of a Travel Stats Query
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





    /// defines the structure of a Vaccination Status Query
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