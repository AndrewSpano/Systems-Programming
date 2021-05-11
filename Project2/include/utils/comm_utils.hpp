#ifndef _COMM_UTILS
#define _COMM_UTILS


#include "structures.hpp"
#include "../data_structures/indices.hpp"


typedef enum communicationID
{
    ACK = 0,
    REJECT = 1,
    NUMERIC = 2,
    INPUT = 3,
    SEND_COUNTRY = 4,
    COUNTRIES_SENT = 5
} communicationID;



namespace comm_utils
{
    void _poll_until_read(const int & fd);
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
        void send_bloom_filterts(MonitorIndex* m_index, const structures::CommunicationPipes* pipes, structures::Input & input);
    }
}


#endif