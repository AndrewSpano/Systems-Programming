#ifndef _STRUCTURES
#define _STRUCTURES

#include <iostream>


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

    typedef struct commPipes
    {
        char* coordination_pipe = NULL;
        char* data_pipe = NULL;

        void print(void)
        {
            printf("Coordination Named Pipe: \"%s\", Data Named Pipe: \"%s\"\n", this->coordination_pipe, this->data_pipe);
        }
    } commPipes;
}


#endif