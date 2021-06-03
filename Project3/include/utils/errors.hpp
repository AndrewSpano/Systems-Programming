#ifndef _ERRORS
#define _ERRORS

#include <iostream>


/// Error code used to distinguish which errors have occurred.
typedef enum ErrorCode
{
    FAIL,
    SUCCESS,
    HELP_TRAVEL_MONITOR,
    HELP_MONITOR,
    INVALID_NUM_ARGS_TRAVEL_MONITOR,
    INVALID_NUM_ARGS_MONITOR,
    INVALID_ARGS,
    INVALID_NUM_MONITORS,
    INVALID_SOCKET_BUFFER_SIZE,
    INVALID_CYCLIC_BUFFER_SIZE,
    INVALID_BLOOM_FILTER_SIZE,
    INVALID_ROOT_DIR,
    INVALID_NUM_THREADS,
    INVALID_PORT,
    INVALID_COUNTRIES,
    INVALID_FLAG,
    INVALID_RECORD,
    INVALID_CITIZEN_ID,
    INVALID_DATE,
    INVALID_DATE1,
    INVALID_DATE2,
    INVALID_CONSISTENT_DATES,
    INVALID_COUNTRY,
    INVALID_COUNTRY_FROM,
    INVALID_COUNTRY_TO,
    INVALID_AGE,
    INVALID_VIRUS_NAME,
    INVALID_NUM_PARAMETERS,
    UNKNOWN_COYNTRY,
    UNKNOWN_VIRUS,
    UNKNOWN_ID
} ErrorCode;


/// Error Handler structure used to monitor the ErrorCode and the mistake associated with it.
typedef struct ErrorHandler
{
    ErrorCode status = SUCCESS;
    std::string invalid_value = "";

    void print_help_travel_monitor(void)
    {
        std::cout << "Usage: bin/travelMonitorClient -m numMonitors -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads" << std::endl
                  << std::endl << "\tnumMonitors        =  The number of (child) Monitor processes to create."
                  << std::endl << "\tsocketBufferSize   =  The size (in bytes) for reading/writing through the sockets."
                  << std::endl << "\tcyclicBufferSize   =  The size of the buffer used to store filesnames for Monitors to read and parse."
                  << std::endl << "\tsizeOfBloom        =  The size (in bytes) of the Bloom Filter."
                  << std::endl << "\tinput_dir          =  The relative/absolute path to the root directory containing the subdiretories for the countries."
                  << std::endl << "\tnumThreads         =  The number of threads that each MonitorServer will create."
                  << std::endl << std::endl;
    }

    void print_help_monitor(void)
    {
        std::cout << "Usage: bin/monitorServer -p port -t numThreads -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom path1 path2 ... pathN" << std::endl
                  << std::endl << "\tport                    =  Port used for communication."
                  << std::endl << "\tnumThreads              =  The number of threads that each MonitorServer will create."
                  << std::endl << "\tsocketBufferSize        =  The size (in bytes) for reading/writing through the sockets."
                  << std::endl << "\tcyclicBufferSize        =  The size of the buffer used to store filesnames for Monitors to read and parse."
                  << std::endl << "\tsizeOfBloom             =  The size (in bytes) of the Bloom Filter."
                  << std::endl << "\tpath1 path 2... pathN   =  Paths to directories containing countries and their data."
                  << std::endl << std::endl;
    }

    bool check_and_print(void)
    {
        bool return_status = this->status != SUCCESS;

        if (this->status != SUCCESS)
        {
            switch(this->status)
            {
                case INVALID_NUM_ARGS_TRAVEL_MONITOR:
                    std::cout << "ERROR: Invalid number of arguments (" << this->invalid_value << ") provided. "
                              << "The number of arguments should be exactly 9. Run ./travelMonitor -h for "
                              << "more information." << std::endl << std::endl;
                    break;
                case INVALID_NUM_ARGS_MONITOR:
                    std::cout << "ERROR: Invalid number of arguments (" << this->invalid_value << ") provided. "
                              << "The number of arguments should be exactly 5. Run ./Monitor -h for more information."
                              << std::endl << std::endl;
                    break;
                case INVALID_ARGS:
                    std::cout << "ERROR: The following flags were not provided:" << this->invalid_value << std::endl << std::endl;
                    break;
                case INVALID_NUM_MONITORS:
                    std::cout << "ERROR: The value for the \"number of Monitors\" (-m flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_SOCKET_BUFFER_SIZE:
                    std::cout << "ERROR: The value for the \"socket buffer size\" (-b flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_CYCLIC_BUFFER_SIZE:
                    std::cout << "ERROR: The value for the \"cyclic buffer size\" (-c flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_BLOOM_FILTER_SIZE:
                    std::cout << "ERROR: The value for the \"bloom filter size\" (-s flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_ROOT_DIR:
                    std::cout << "ERROR: The value for the \"root directory\" (-i flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_NUM_THREADS:
                    std::cout << "ERROR: The value for the \"number of threards\" (-t flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_PORT:
                    std::cout << "ERROR: The value for the \"listening port\" (-p flag): \"" << this->invalid_value
                              << "\" is invalid." << std::endl << std::endl;
                case INVALID_FLAG:
                    std::cout << "ERROR: Unrecognized flag: \"" << this->invalid_value << "\"." << std::endl << std::endl;
                    break;
                case INVALID_RECORD:
                    std::cout << "ERROR: In Record: \"" << this->invalid_value << "\"." << std::endl;
                    break;
                case INVALID_CITIZEN_ID:
                    std::cout << "ERROR: The \"citizenID\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_DATE:
                    std::cout << "ERROR: The \"date\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_DATE1:
                    std::cout << "ERROR: The \"date1\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_DATE2:
                    std::cout << "ERROR: The \"date2\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_CONSISTENT_DATES:
                    std::cout << "ERROR: The \"date2\" should \"occur\" later than \"date1\" in the line: \""
                              << this->invalid_value << "\"." << std::endl << std::endl;
                    break;
                case INVALID_COUNTRY:
                    std::cout << "ERROR: The \"country\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_COUNTRY_FROM:
                    std::cout << "ERROR: The \"countryFrom\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_COUNTRY_TO:
                    std::cout << "ERROR: The \"countryTo\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_AGE:
                    std::cout << "ERROR: The \"age\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_VIRUS_NAME:
                    std::cout << "ERROR: The \"virusName\" parameter provided in the following line: \""
                              << this->invalid_value << "\" is invalid." << std::endl << std::endl;
                    break;
                case INVALID_NUM_PARAMETERS:
                    std::cout << "ERROR: The number of parameters provided in the following line "
                              << "exceeds the number of needed parameters: \"" << this->invalid_value
                              << "\"." << std::endl << std::endl;
                    break;
                case UNKNOWN_COYNTRY:
                    std::cout << "ERROR: CountryFrom \"" << this->invalid_value << "\" does not exist in "
                              << "the database." << std::endl << std::endl;
                    break;
                case UNKNOWN_VIRUS:
                    std::cout << "ERROR: Virus \"" << this->invalid_value << "\" does not exist in "
                              << "the database." << std::endl << std::endl;
                    break;
                case UNKNOWN_ID:
                    std::cout << "ERROR: Citizen with ID \"" << this->invalid_value
                              << "\" does not exist in the database." << std::endl;
                    break;
            }
        }

        this->status = SUCCESS;
        return return_status;
    }
} ErrorHandler;



#endif