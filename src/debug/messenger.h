#pragma once

#include <spirit_header.h> 

/**
 * @brief Functions to output and log debug messeges. Use the debug macros pls not the function itself
 * @author Kael Johnston
 * @date Feb 17 2022
 */


//should enable 
#define ENABLE_FATAL // log a unrecoverable error
#define ENABLE_ERROR // log a failure that might not crash, but behavoir is undefined
#define ENABLE_WARNING
#define ENABLE_VALIDATION
#define ENABLE_INFO
#define ENABLE_DEBUG

typedef enum {
    DEBUG_FATAL,
    DEBUG_ERROR,
    DEBUG_WARNING,
    DEBUG_VALIDATION,
    DEBUG_INFO,
    DEBUG_DEBUG,
    DEBUG_TYPE_MAX // count for use in fn
} DebugSeverity;

// check if debug messeges are enabled, otherwise disable the corrospining messege
#ifdef ENABLE_FATAL
#define log_fatal(messege, ...) debug_log (DEBUG_FATAL, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_fatal(messege, ...) ((void)0)
#endif

// a failure within a function that may cause a crash or undefined behavior
#ifdef ENABLE_ERROR
#define log_error(messege, ...) debug_log (DEBUG_ERROR, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_error(messege, ...) ((void)0)
#endif

// warn about improper parameters, or other trivial errors
#ifdef ENABLE_WARNING
#define log_warning(messege, ...) debug_log (DEBUG_WARNING, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_warning(messege, ...) ((void)0)
#endif

// log vulkan validation layer errors
#ifdef ENABLE_VALIDATION
#define log_validation(messege, ...) debug_log (DEBUG_VALIDATION, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_validation(messege, ...) ((void)0)
#endif

// log basic info, like hardware selections
#ifdef ENABLE_INFO
#define log_info(messege, ...) debug_log (DEBUG_INFO, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_info(messege, ...) ((void)0)
#endif

// log debug messeges, that will be removed for final builds
#ifdef ENABLE_DEBUG
#define log_debug(messege, ...) debug_log (DEBUG_DEBUG, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define log_debug(messege, ...) ((void)0)
#endif

// debbuging functionality for the errno unix library
#ifdef __unix
// messege can be NULL if you don't want it.

// stupid workaround to convert line number to string
#define int_to_str_2(int) #int
#define int_to_str(int) int_to_str_2(int)

#define log_perror(messege, ...) unix_log_perror(\
    __FILE__,\
    __func__,\
    int_to_str(__LINE__),\
    messege,\
    ##__VA_ARGS__)


// Params:
//      s - the automatic string identifying the file and whatnot
//      m - an optional messege that can be left by the user
int unix_log_perror (
    const char *restrict file,
    const char *restrict func,
    const char *restrict line,
    const char *restrict m,
    ...);
#else // disable the function on non-unix systems
#define log_perror(string) ((void)0)
#endif

// log debugging messege
int debug_log (
    DebugSeverity severity, 
    const char *restrict file,
    const char *restrict func,
    const int            line,
    const char *restrict format, ...);