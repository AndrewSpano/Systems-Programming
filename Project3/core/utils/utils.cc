#include <iostream>
#include <cstring>

#include "../../include/utils/utils.hpp"



void utils::write_to_buf(char buf[], size_t & offset, size_t* len, char data[])
{
    memcpy(buf + offset, len, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(buf + offset, data, *len);
    offset += *len;
}


void utils::read_from_buf(char buf[], size_t & offset, size_t* len, char data[])
{
    memcpy(len, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(data, buf + offset, *len);
    offset += *len;
}


void utils::perror_exit(const std::string & error_message)
{
    perror(error_message.c_str());
    exit(1);
}


void utils::strerror_exit(const std::string & error_message, const int & err)
{
    fprintf(stderr , "%s: %s\n", error_message.c_str(), strerror(err));
    exit(1);
}



std::string file_basename(char* path)
{
    char* start = strrchr(path, '/') + 1;
    char buffer[256] = {0};
    size_t i = 0;
    while (start[i] != '\0')
    {
        buffer[i] = start[i];
        i++;
    }

    return std::string(buffer);
}



std::string parent_directory(char* path)
{
    char* last_forslash = strrchr(path, '/');
    while (*last_forslash == '/')
        last_forslash--;
    last_forslash++;

    size_t i = 0;
    char buffer[256] = {0};
    while (path + i != last_forslash)
    {
        buffer[i] = path[i];
        i++;
    }

    return std::string(buffer);
}



std::string parent_directory_name(char* path)
{
    std::string parent_dir = parent_directory(path);

    char buffer[256] = {0};
    memcpy(buffer, parent_dir.c_str(), parent_dir.length() + 1);

    char* start = strrchr(buffer, '/') + 1;
    char dest_buffer[256] = {0};
    strcpy(dest_buffer, start);

    return std::string(dest_buffer);
}


// void utils::parent_dirname(char* path, char dest[])
// {
//     char* last_forslash = strrchr(path, '/');
//     while (*last_forslash == '/' || *last_forslash == '\\')
//         last_forslash--;
//     size_t len = 1;
//     char* start = last_forslash - 1;
//     while (*start != '/' && *start != '\\')
//     {
//         start--;
//         len++;
//     }
//     start++;

//     for (size_t i = 0; i < len; i++)
//         dest[i] = start[i];
// }