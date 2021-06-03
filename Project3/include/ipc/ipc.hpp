#ifndef _IPC
#define _IPC



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
    TRAVEL_REQUEST_OK = 11,
    SEARCH_VACCINATION_STATUS_SEND_DATA = 12,
    SEARCH_VACCINATION_STATUS_DOES_NOT_CONTAIN = 13,
    SEARCH_VACCINATION_STATUS_CONTAINS = 14,
    SEARCH_VACCINATION_STATUS_DATA = 15
} communicationID;



namespace ipc
{
    void _poll_until_read(const int & fd);
    size_t _poll_until_any_read(int fds[], bool fd_has_read[], const size_t & num_fds, const size_t & count_read);
    int ready_fd(struct pollfd fdarr[], size_t num_fds);
    void _wait_ack(const int & fd);

    void _send_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size);
    void _receive_numeric(const int & input_fd, const int & output_fd, const uint64_t & numeric, const uint64_t & buffer_size);

    void _notify_and_wait_ack(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const size_t & bytes_out, const uint64_t & buffer_size);
    void _send_message(const int & input_fd, const int & output_fd, const uint8_t & msg_id, const char buf[], size_t bytes_out, const uint64_t & buffer_size);
    void _receive_message(const int & input_fd, const int & output_fd, uint8_t & msg_id, char buf[], size_t & bytes_in, const uint64_t & buffer_size);
    void wait_for_command(const int & input_fd, const int & output_fd, uint8_t & msg_id, char message[], const uint64_t & buffer_size);
}


#endif