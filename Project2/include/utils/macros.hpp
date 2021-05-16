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


#endif
