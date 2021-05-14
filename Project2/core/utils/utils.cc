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