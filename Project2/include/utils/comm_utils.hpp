#ifndef _COMM_UTILS
#define _COMM_UTILS


#include "structures.hpp"
#include "../data_structures/indices.hpp"
#include "../data_structures/bloom_filter.hpp"


typedef enum communicationID
{
    ACK = 0,
    REJECT = 1,
    NUMERIC = 2,
    INPUT = 3,
    SEND_COUNTRY = 4,
    COUNTRIES_SENT = 5,
    SEND_VIRUS_NAME = 6,
    SEND_BF = 7,
    TRAVEL_REQUEST_SEND_DATA = 8,
    TRAVEL_REQUEST_NOT_VACCINATED = 9,
    TRAVEL_REQUEST_NEED_VACCINATION = 10,
    TRAVEL_REQUEST_OK = 11
} communicationID;



namespace comm_utils
{
    void _poll_until_read(const int & fd);
    size_t _poll_until_any_read(int fds[], bool fd_has_read[], const size_t & num_fds, const size_t & count_read);
    void _wait_ack(const int & fd);
    void _send_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size);
    void _receive_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size);
    void _notify_and_wait_ack(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const size_t & bytes_out, const uint64_t & buffer_size);
    void _send_message(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size);
    void _receive_message(const int & input_fd, const int & output_fd, uint8_t & msg_id, char buf[], size_t & bytes_in, const uint64_t & buffer_size);

    namespace travel_monitor
    {
        void send_args(const structures::CommunicationPipes pipes[], const structures::Input & input);
        void assign_countries(travelMonitorIndex* tm_index, const structures::CommunicationPipes pipes[], const structures::Input & input);
        void receive_bloom_filters(travelMonitorIndex* tm_index, const structures::CommunicationPipes pipes[], const structures::Input & input);
    }

    namespace monitor
    {
        void init_args(const structures::CommunicationPipes* pipes, structures::Input & input);
        void receive_countries(MonitorIndex* m_index, const structures::CommunicationPipes* pipes, structures::Input & input);
        void send_bloom_filters(MonitorIndex* m_index, const structures::CommunicationPipes* pipes, structures::Input & input);
        void wait_for_command(MonitorIndex* m_index, const int & input_fd, const int & output_fd, structures::Input & input, uint8_t & msg_id, char message[]);
    }
}


#endif