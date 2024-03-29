#ifndef _MACROS
#define _MACROS

#define LOG_AND_RETURN(line)                             \
{                                                        \
  std::cout << "ERROR IN RECORD " << line << std::endl;  \
  return;                                                \
}

#define DELETE_LOG_AND_RETURN(record, line)              \
{                                                        \
  std::cout << "ERROR IN RECORD " << line << std::endl;  \
  delete record;                                         \
  return;                                                \
}

#define DELETE_LOG_VACCINATION_AND_RETURN(record, date)                                          \
{                                                                                                \
  std::cout << "ERROR: " << record->id << " ALREADY VACCINATED ON " << date << "." << std::endl; \
  delete record;                                                                                 \
  return;                                                                                        \
}

#define DELETE_LOG_NON_VACCINATION_AND_RETURN(record)                              \
{                                                                                  \
  std::cout << "ERROR: " << record->id << " ALREADY NOT VACCINATED." << std::endl; \
  delete record;                                                                   \
  return;                                                                          \
}

#define LOG_COMMAND_AND_RETURN(line)                                                            \
{                                                                                               \
  std::cout << "ERROR: The format of the command \"" << line << "\" is invalid." << std::endl;  \
  return false;                                                                                 \
}

#endif
