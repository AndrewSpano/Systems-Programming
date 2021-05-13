#ifndef _MACROS
#define _MACROS


#define HANDLE_AND_RETURN(handler, error_code, error_value) \
{                                                           \
    handler.status = error_code;                            \
    handler.invalid_value = error_value;                    \
    return;                                                 \
}


#define DELETE_HANDLE_AND_RETURN(record, handler, error_code, error_value) \
{                                                                          \
    delete record;                                                         \
    handler.status = error_code;                                           \
    handler.invalid_value = error_value;                                   \
    return;                                                                \
}

// #define LOG_AND_RETURN(line)                             \
// {                                                        \
//   std::cout << "ERROR IN RECORD " << line << std::endl;  \
//   return;                                                \
// }

// #define DELETE_LOG_AND_RETURN(record, line)              \
// {                                                        \
//   std::cout << "ERROR IN RECORD " << line << std::endl;  \
//   delete record;                                         \
//   return;                                                \
// }

// #define DELETE_LOG_VACCINATION_AND_RETURN(record, date)                                          \
// {                                                                                                \
//   std::cout << "ERROR: " << record->id << " ALREADY VACCINATED ON " << date << "." << std::endl; \
//   delete record;                                                                                 \
//   return;                                                                                        \
// }

// #define DELETE_LOG_NON_VACCINATION_AND_RETURN(record)                              \
// {                                                                                  \
//   std::cout << "ERROR: " << record->id << " ALREADY NOT VACCINATED." << std::endl; \
//   delete record;                                                                   \
//   return;                                                                          \
// }

// #define LOG_COMMAND_AND_RETURN(line)                                                            \
// {                                                                                               \
//   std::cout << "ERROR: The format of the command \"" << line << "\" is invalid." << std::endl;  \
//   return false;                                                                                 \
// }

#endif
