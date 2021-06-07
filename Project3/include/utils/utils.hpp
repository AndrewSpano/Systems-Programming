#ifndef _UTILS
#define _UTILS

#include <iostream>
#include <cstring>


namespace utils
{
    void write_to_buf(char buf[], size_t & offset, size_t* len, char data[]);
    void read_from_buf(char buf[], size_t & offset, size_t* len, char data[]);

    void perror_exit(const std::string & error_message);
    void strerror_exit(const std::string & error_message, const int & err);
}

std::string file_basename(char* path);
std::string parent_directory(char* path);
std::string parent_directory_name(char* path); 

#endif
